# Minecraft kurciqs edition


Precompiled executable for Minecraft kurciqs edition: https://kurciqs1.itch.io/mcke

Double-click on `cmake-build-debug/Minecraft.exe` to start the game.

If you don't have Windows 64-bit, you'll have to build the game (see below)

## Build instructions
Make sure you have cmake and make installed and on your path.

Type:
```shell
mkdir cmake-build-debug/
cmake . -B cmake-build-debug/
cd cmake-build-debug/
make
```

The final executable will be in `cmake-build-debug/`
