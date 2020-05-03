# Crazy Sentences Online
A game for ESL students to learn and have fun!

# Dependencies
* Boost 1.72.0
* SDL 2.0.12
* ogre3d 1.12.2
* Dear ImGui 1.75

# Build

## windows

```shell
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-ogre3d
```

## linux

download and install OGRE3D 1.12 manually.

```shell
sudo apt-get install libxinerama-dev
sudo apt-get install libxrandr-dev
sudo apt-get install libxcursor-dev
sudo apt-get install libxi-dev
sudo apt-get install libx11-dev
sudo apt-get install libxaw7-dev
sudo apt-get install libglew-dev
sudo apt-get install libtesseract-dev libleptonica-dev liblept5
```

modules/libhocrEditModuleLibShared.so.1.0.0 exception fix:
```shell
sudo cp /usr/lib/x86_64-linux-gnu/libzzip-0.so.13.0.62 /usr/lib/x86_64-linux-gnu/libzzip.so.13
```
