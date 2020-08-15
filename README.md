# nti56acad.arx
# build

start cmake-gui from vs2017 command line
NMake Makefiles
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -Hnti56acad.arx -Bnti56acad.arx/build -G "Visual Studio 15 2017" -T 
## build with nmake
on vs2017 command line:
cd /d h:\nti56acad.arx\build
nmake