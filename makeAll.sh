build_patch() {
    REGION=$1
    IS_EMU=$2
    PLATFORM=$([ $IS_EMU -eq 0 ] && echo "3DS" || echo "Citra")
    PATCH_PATH="./patch_files/$PLATFORM/$REGION/"

    make clean
    make -j12 REGION=$REGION citra=$IS_EMU
    echo Moving to "$PATCH_PATH"
    mkdir -p "$PATCH_PATH"
    mv -v code.ips "$PATCH_PATH"
    mv -v exheader.bin "$PATCH_PATH"
    echo
}

REGIONS=(USA EUR JPN KOR TWN)
EMU_FLAGS=(0 1)

for REGION in ${REGIONS[@]}; do
  for EMU_FLAG in ${EMU_FLAGS[@]}; do
    build_patch $REGION $EMU_FLAG
  done
done
