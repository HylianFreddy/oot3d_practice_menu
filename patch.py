import os
import struct
import subprocess
import sys

print()

elf = sys.argv[1]
region = sys.argv[2]
citra = int(sys.argv[3]) != 0

result = subprocess.run([os.environ["DEVKITARM"] + r'/bin/arm-none-eabi-objdump', '--section-headers', elf], stdout=subprocess.PIPE)
lines = str(result.stdout).split('\\n')
sectionsInfo = [line.split()[1:6] for line in lines if line.split() and line.split()[0].isdigit()]
sections = ((sec[0], int(sec[2],16), int(sec[4],16), int(sec[1],16)) for sec in sectionsInfo if int(sec[2],16) != 0)

off = lambda vaddr: struct.pack(">I",vaddr - 0x100000)[1:]
sz = lambda size: struct.pack(">H", size)

memoryExtension = 0

ips = b'PATCH'
with open(elf, 'rb') as e:
    for name, vaddr, offset, size in sections:

        if name == '.text':
            memoryExtension += size

        e.seek(offset, 0)
        while size > 65535:
            patch = e.read(65535)
            # print('{:0x} {:0x} {}'.format(vaddr, vaddr + 65535, name))
            ips += off(vaddr)
            ips += sz(65535)
            ips += patch
            vaddr += 65535
            offset += 65535
            size -= 65535

        patch =  e.read(size)
        if len(patch) != 0:
            # print('{:0x} {:0x} {}'.format(vaddr, vaddr + size, name))
            ips += off(vaddr)
            ips += sz(size)
            ips += patch
ips += b'EOF'

with open("code.ips", 'wb') as patchFile:
    patchFile.write(ips)

print("created code.ips")

# Create exheader file by adding code size info to the template
exhRegion = 'KOR' if region in ['KOR', 'TWN'] else 'USA'
exhTemplatePath = os.path.join('exheader', 'exheader_template_%s.bin' % exhRegion)
with open(exhTemplatePath, 'rb') as exhTemplate, open('exheader.bin', 'wb') as exh:
    exh.write(exhTemplate.read(0x34))

    numDataPages = struct.unpack("<I", exhTemplate.read(4))[0]
    dataSize = struct.unpack("<I", exhTemplate.read(4))[0]
    bssSize = struct.unpack("<I", exhTemplate.read(4))[0]

    # The new code needs to be in BSS section on Citra, and in Data section on 3DS.
    # A buffer of 0x1000 bytes is added because the new code is injected at the first
    # multiple of 0x1000 after the BSS section.
    if citra:
        bssSize += memoryExtension + 0x1000
    else:
        dataSize += bssSize + memoryExtension + 0x1000
        numDataPages = ((dataSize + 0xFFF) & ~0xFFF) >> 0xC
        bssSize = 0

    exh.write(struct.pack("<I", numDataPages))
    exh.write(struct.pack("<I", dataSize))
    exh.write(struct.pack("<I", bssSize))

    exh.write(exhTemplate.read())

print("created exheader.bin")
print()
