cmake_minimum_required(VERSION 3.4)
# The FetchModule is only available since CMake 3.11.
# FetchContent_MakeAvailable since CMake 3.14.

# FetchContent vs ExternalProject
# FetchContent populates content from the other project at configure time.
# ExternalProject populates content from the other project at build time. 
include(FetchContent)

FetchContent_Declare(
    CrazySentencesOnlineModuleRenderFetch
    GIT_REPOSITORY "https://github.com/bradosia/CrazySentencesOnline-Module-Render.git"
    SOURCE_DIR "${PROJECT_SOURCE_DIR}/modules/CrazySentencesOnline-Module-Render"
)
FetchContent_MakeAvailable(CrazySentencesOnlineModuleRenderFetch)

FetchContent_Declare(
    CrazySentencesOnlineModuleSocketsFetch
    GIT_REPOSITORY "https://github.com/bradosia/CrazySentencesOnline-Module-Sockets.git"
    SOURCE_DIR "${PROJECT_SOURCE_DIR}/modules/CrazySentencesOnline-Module-Sockets"
)
FetchContent_MakeAvailable(CrazySentencesOnlineModuleSocketsFetch)

