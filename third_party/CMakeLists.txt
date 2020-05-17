cmake_minimum_required(VERSION 3.11)
# The FetchModule is only available since CMake 3.11.

include(FetchContent)

FetchContent_Populate(
    assimp-XNAlara
    GIT_REPOSITORY "https://github.com/bradosia/assimp-XNAlara.git"
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/assimp-xnalara
)
