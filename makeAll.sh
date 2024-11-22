build_patch() {
    REGION=$1
    TITLE_ID=$2
    IS_EMU=$3
    PLATFORM=$([ $IS_EMU -eq 0 ] && echo "3DS" || echo "Citra")
    PATCH_PATH="./patch_files/$PLATFORM/00040000000$TITLE_ID/"

    make clean
    make -j12 REGION=$REGION citra=$IS_EMU
    echo Moving to "$PATCH_PATH"
    mkdir -p "$PATCH_PATH"
    mv -v code.ips "$PATCH_PATH"
    mv -v exheader.bin "$PATCH_PATH"
    echo
}

REGIONS=(USA EUR JPN KOR TWN)
TITLE_IDS=('33500' '33600' '33400' '8F800' '8F900')
EMU_FLAGS=(0 1)

for R_IDX in ${!REGIONS[@]}; do
  for EMU_FLAG in ${EMU_FLAGS[@]}; do
    build_patch ${REGIONS[$R_IDX]} ${TITLE_IDS[$R_IDX]} $EMU_FLAG
  done
done
