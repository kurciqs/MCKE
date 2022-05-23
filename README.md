I'm using cmake with CLion(https://www.jetbrains.com/clion/), so that's why the .idea/ folder is here.

Precompiled executable works only on Windows(64-Bit)!


DOWNLOAD AND RUN:
1. Open git-bash (make sure you have git installed (https://git-scm.com/download/win))
2. Type:
```
git clone https://github.com/kurciqs/MinecraftClone
cd MinecraftClone/
cd cmake-build-debug/
./Minecraft.exe
```
Or click on the green button "Code" and select "Download ZIP", unzip it and thanks to static linking you can just double-click Minecraft.exe in cmake-build-debug/




To run it on a different platform than Windows 64-bit, you'll have to rebuild it.

BUILD:

Make sure you have cmake and make installed and on your path.
1. Open a terminal
2. Clean up the build directory and remake it:
```
#Windows:
del cmake-build-debug/
#Linux:
rm -rf cmake-build-debug/
#Both:
mkdir cmake-build-debug/
```
3.Rebuild:
```
cmake . -B cmake-build-debug/
cd cmake-build-debug/
make
```
The executable can be then found in cmake-build-debug/Minecraft(.exe)


I used this cmake OpenGL template: https://github.com/abhisarshukla/opengl-cmake-template