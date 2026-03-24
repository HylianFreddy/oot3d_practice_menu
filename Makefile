#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>DEVKITARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#
# NO_SMDH: if set to anything, no SMDH file is generated.
# ROMFS is the directory which contains the RomFS, relative to the Makefile (Optional)
# APP_TITLE is the name of the app stored in the SMDH file (Optional)
# APP_DESCRIPTION is the description of the app stored in the SMDH file (Optional)
# APP_AUTHOR is the author of the app stored in the SMDH file (Optional)
# ICON is the filename of the icon (.png), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.png
#     - icon.png
#     - <libctru folder>/default_icon.png
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	$(sort $(dir $(wildcard src/*/ src/)))
DATA		:=	data
INCLUDES	:=	include $(SOURCES)
INCLUDES    +=  assets
#ROMFS		:=	romfs

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=softfp -mtp=soft -mfpu=vfpv2

CFLAGS	:=	-g -Wall -mword-relocations -D DEBUG \
			-fomit-frame-pointer -ffunction-sections \
			-fdiagnostics-color=always $(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS -O1

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	+=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,-Map,$(notdir $*.map) -T $(TOPDIR)/$(LINK_SCRIPT) -nostdlib -lgcc

LIBS	:=	-lgcc

# Region selection
REGION	?= USA

VALID_REGIONS	:=	USA EUR JPN KOR TWN DEMO_USA DEMO_EUR
MAIN_REGIONS	:=	USA EUR JPN
KOR_TWN_REGIONS	:=	KOR TWN
DEMO_REGIONS	:=	DEMO_USA DEMO_EUR
KOR_TWN			:=	0
DEMO			:=	0

ifeq ($(filter $(REGION),$(VALID_REGIONS)),)
	$(error "Invalid region: $(REGION)")
endif

REGION_INDEX		:=	$(shell py -c 'print("$(VALID_REGIONS)".split().index("$(REGION)") + 1)')
UNSELECTED_REGIONS	:=	$(subst $(REGION),,$(VALID_REGIONS))

ifneq ($(filter $(REGION),$(MAIN_REGIONS)),)
	LINK_SCRIPT	:=	linker_scripts/MAIN.ld
endif
ifneq ($(filter $(REGION),$(KOR_TWN_REGIONS)),)
	LINK_SCRIPT	:=	linker_scripts/KOR_TWN.ld
	KOR_TWN		:=	1
endif
ifneq ($(filter $(REGION),$(DEMO_REGIONS)),)
	LINK_SCRIPT	:=	linker_scripts/DEMO.ld
	DEMO		:=	1
endif

# Define region for the Assembly code
ASFLAGS	+=	-D _KOR_TWN_=$(KOR_TWN) -D _DEMO_=$(DEMO)

# Define region for the C code
CFLAGS	+=	-D REGION_KOR_TWN=$(KOR_TWN) -D DEMO_VERSION=$(DEMO)

# Define region for the Linker Script
LDFLAGS +=	-Wl,--defsym,_LD_$(REGION)=1 $(foreach region,$(UNSELECTED_REGIONS),-Wl,--defsym,_LD_$(region)=0)
LDFLAGS +=	-Wl,--defsym,_LD_CURRENT_REGION_ID=$(REGION_INDEX)

# Define extra flags
GZ3D_EXTRAS ?= 0
CFLAGS += -D GZ3D_EXTRAS=$(GZ3D_EXTRAS)

#---------------------------------------------------------------------------------
# Check existing build flags
#---------------------------------------------------------------------------------
LAST_BUILD_FLAGS_PATH	:=	$(BUILD)/_flags.txt
LAST_BUILD_FLAGS		:=	$(shell cat $(LAST_BUILD_FLAGS_PATH) 2> /dev/null)
LAST_BUILD_REGION		:=	$(word 1,$(LAST_BUILD_FLAGS))
LAST_BUILD_KOR_TWN		:=	$(word 2,$(LAST_BUILD_FLAGS))
LAST_BUILD_DEMO			:=	$(word 3,$(LAST_BUILD_FLAGS))
LAST_BUILD_GZ3D_EXTRAS	:=	$(word 4,$(LAST_BUILD_FLAGS))

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(DEVKITARM) include/citro3d


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif


.PHONY: clean all

#---------------------------------------------------------------------------------
all:
	@printf "Writing commit string..."
	@$(TOPDIR)/write_commit_string.sh
	@printf " Done\n"
ifneq ($(LAST_BUILD_KOR_TWN) $(LAST_BUILD_DEMO) $(LAST_BUILD_GZ3D_EXTRAS),$(KOR_TWN) $(DEMO) $(GZ3D_EXTRAS))
	@printf "Build setup changed, cleaning..."
	@rm -fr $(BUILD) $(TARGET).elf
	@printf " Done\n"
else ifneq ($(LAST_BUILD_REGION),$(REGION))
	@rm -fr $(TARGET).elf
endif
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@echo $(REGION) $(KOR_TWN) $(DEMO) $(GZ3D_EXTRAS) > $(LAST_BUILD_FLAGS_PATH)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@py patch.py $(OUTPUT).elf $(REGION);

#---------------------------------------------------------------------------------
clean:
	@printf "Cleaning..."
	@rm -fr $(BUILD) $(TARGET).elf
	@printf " Done\n"


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).elf	:	$(OFILES) $(TOPDIR)/$(LINK_SCRIPT)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# rules for assembling GPU shaders
#---------------------------------------------------------------------------------
define shader-as
	$(eval CURBIN := $(patsubst %.shbin.o,%.shbin,$(notdir $@)))
	picasso -o $(CURBIN) $1
	bin2s $(CURBIN) | $(AS) -o $@
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(CURBIN) | tr . _)`.h
endef

%.shbin.o : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

%.shbin.o : %.v.pica
	@echo $(notdir $<)
	@$(call shader-as,$<)

%.shbin.o : %.shlist
	@echo $(notdir $<)
	@$(call shader-as,$(foreach file,$(shell cat $<),$(dir $<)/$(file)))

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
