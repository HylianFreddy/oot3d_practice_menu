make clean
make -j12 REGION=EUR
echo Moving to ./Patch Files/EUR/3DS/code.ips
mv code.ips "./Patch Files/EUR/3DS/code.ips"

make clean
make -j12 REGION=EUR citra=1
echo Moving to ./Patch Files/EUR/Citra/code.ips
mv code.ips "./Patch Files/EUR/Citra/code.ips"

make clean
make -j12 REGION=JPN
echo Moving to ./Patch Files/JP/3DS/code.ips
mv code.ips "./Patch Files/JP/3DS/code.ips"

make clean
make -j12 REGION=JPN citra=1
echo Moving to ./Patch Files/JP/Citra/code.ips
mv code.ips "./Patch Files/JP/Citra/code.ips"

make clean
make -j12 REGION=USA citra=1
echo Moving to ./Patch Files/USA/Citra/code.ips
mv code.ips "./Patch Files/USA/Citra/code.ips"

make clean
make -j12 REGION=USA
echo Moving to ./Patch Files/USA/3DS/code.ips
mv code.ips "./Patch Files/USA/3DS/code.ips"
