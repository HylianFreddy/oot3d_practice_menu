build_patch() {
    REGION=$1
    TITLE_ID=$2

    PATCH_PATH_3DS="./patch_files/3DS/00040000000$TITLE_ID"
    PATCH_PATH_CITRA="./patch_files/Citra/00040000000$TITLE_ID"

    make clean
    make -j12 REGION=$REGION
    mkdir -p "$PATCH_PATH_3DS"
    mkdir -p "$PATCH_PATH_CITRA"
    cp -v code.ips "$PATCH_PATH_3DS"
    cp -v exheader_3ds.bin "$PATCH_PATH_3DS/exheader.bin"
    cp -v code.ips "$PATCH_PATH_CITRA"
    cp -v exheader_citra.bin "$PATCH_PATH_CITRA/exheader.bin"
    echo
}

REGIONS=(USA EUR JPN KOR TWN)
TITLE_IDS=('33500' '33600' '33400' '8F800' '8F900')

for R_IDX in ${!REGIONS[@]}; do
    build_patch ${REGIONS[$R_IDX]} ${TITLE_IDS[$R_IDX]}
done
