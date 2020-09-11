# nti56acad.arx
# build

start cmake-gui from vs2017 command line
NMake Makefiles
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -Hnti56acad.arx -Bnti56acad.arx/build -G "Visual Studio 15 2017" -T 
## build with nmake
on vs2017 command line:
cd /d h:\nti56acad.arx\build
nmake
pushd build && nmake && popd
## build with nmake in vscode
on vs2017 command line:
cd /d h:\nti56acad.arx
code .
Shift+Ctrl+B to run .vscode/fask.json

# docs for imgui win32 openGL docking

https://github.com/ocornut/imgui/pull/2772
https://github.com/danec020/imgui.git
https://github.com/n00bmind/imgui.git