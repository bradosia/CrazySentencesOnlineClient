# Crazy Sentences Online
A game for ESL students to learn and have fun!

# Dependencies
* Boost 1.72.0
* SDL 2.0.12
* ogre3d 1.12.2
* Dear ImGui 1.75

# Build

## windows

Install MSYS2<BR>
Then, install GCC, cmake, git and dependencies. Confirmed working with Windows 10 as of 07/27/2021.
```shell
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain git make mingw-w64-x86_64-cmake
pacman -Rns cmake
# restart MSYS2 so that we use the mingw cmake
```
Libraries:
```shell
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-ogre3d
```

Ogre3D 1.12.2

```shell
cmake -G "MSYS Makefiles" -DOGRE_STATIC=ON -DRELEASE=ON -DOGRE_BUILD_COMPONENT_BITES=OFF -DOGRE_BUILD_RENDERSYSTEM_D3D11=OFF -DOGRE_BUILD_RENDERSYSTEM_D3D9=OFF -DCMAKE_BUILD_TYPE=MinSizeRel --config release ..
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

## Notes

```shell
_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE
```
caused by a missing libstdc++-6.dll
