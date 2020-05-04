cmake_minimum_required(VERSION 3.0)

set(lib_base_name CrazySentencesOnlineClient)
project(${lib_base_name} VERSION 1.0.0 LANGUAGES CXX)
set(RELEASE_DIR "${CMAKE_BINARY_DIR}/${lib_base_name}/")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIR})

# Configurable Options
OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_STATIC_LIBS "Build static libraries" ON)

set(CMAKE_CXX_STANDARD 17)

find_package(OGRE 1.09 REQUIRED)
find_package(Boost 1.56 REQUIRED COMPONENTS
             system filesystem)

# third party
set(INCLUDE_DIRECTORIES
  ${OGRE_INCLUDE_DIRS}
  third_party/imgui/include/
  third_party/rapidjson/include/
  third_party/Module-Manager/include/
  third_party/Settings-Manager/include/
  third_party/OIS-1.5/includes/
  src/modules
)

set(LINK_DIRECTORIES
  ${OGRE_LIBRARY_DIRS}
  third_party/imgui/lib/
  lib/
  third_party/Module-Manager/lib/
  third_party/Settings-Manager/lib/
)

# MODULES
add_subdirectory(src/modules/)
add_subdirectory(third_party/imgui/)

# Set up source files
set(SOURCES
  src/Module.cpp

  src/UI/Client.cpp
  src/UI/SceneMenu.cpp
)

set(HEADERS
  src/Module.hpp
  src/Interface.hpp
  src/core/config.hpp

  src/UI/Client.hpp
  src/UI/SceneMenu.hpp
)

set(SHARED_COMPILE_DEFINITIONS
  -DSHARED_LIBRARY
  #-DGLEW_STATIC
)

set(STATIC_COMPILE_DEFINITIONS
  -DSTATIC_LIBRARY
  #-DGLEW_STATIC
)

set(LIBRARIES ${LIBRARIES}
    OgreRTShaderSystem
    OgreMain
    imgui
    SDL2main
    SDL2

    # Boost
    Boost::system
    Boost::filesystem
)

if(WIN32)
    set(LIBRARIES ${LIBRARIES}
        #OgreGLSupport
        glew32
        opengl32

        # Windows Libraries
        stdc++
        gcc
        uuid

        windowscodecs
        opengl32
        gdi32
        comdlg32
        oleaut32
        imm32
        winmm
        winspool
        kernel32
        user32
        shell32
        ole32
        advapi32
        ws2_32
        mingw32
        rpcrt4
    )
elseif(UNIX)
    set(LIBRARIES ${LIBRARIES}
        #OgreGLSupport
        GLEW
        GL

        # Unix Libraries
        stdc++
        gcc
        uuid

        pthread
        glib-2.0
        gthread-2.0
        dl
        m
    )
endif()


set(SHARED_LINK_LIBRARIES ${LIBRARIES})
set(STATIC_LINK_LIBRARIES ${LIBRARIES})

# Set up library names
if(BUILD_SHARED_LIBS)
  set(lib_shared_name ${lib_base_name}LibShared)
  set(lib_shared_name_decorated ${lib_shared_name}${JKQtPlotter_LIBNAME_ADDITION})
  set(${lib_base_name}_SHARED_LIBRARY ${lib_shared_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_shared_name}" )
endif(BUILD_SHARED_LIBS)

if(BUILD_STATIC_LIBS)
  set(lib_name ${lib_base_name}Lib)
  set(lib_name_decorated ${lib_name}${JKQtPlotter_LIBNAME_ADDITION})
  set(${lib_base_name}_STATIC_LIBRARY ${lib_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_name}" )
endif(BUILD_STATIC_LIBS)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(CMakePackageConfigHelpers)

if(BUILD_SHARED_LIBS)
  add_library(${lib_shared_name} SHARED  ${SOURCES} ${HEADERS})
  target_compile_features(${lib_shared_name} PUBLIC cxx_std_17)
  set_property(TARGET ${lib_shared_name} PROPERTY VERSION "${PROJECT_VERSION}")
  set_property(TARGET ${lib_shared_name} PROPERTY OUTPUT_NAME  "${lib_shared_name_decorated}")
  target_include_directories(${lib_shared_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${INCLUDE_DIRECTORIES}
  )
  set_target_properties(${lib_shared_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")
  if(UNIX)
      #set_property(TARGET ${lib_shared_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
  endif()
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_shared_name}Version.cmake
    VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion )

  # Compile Definitions
  target_compile_definitions(${lib_shared_name} PUBLIC ${SHARED_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_shared_name} PUBLIC ${SHARED_LINK_LIBRARIES})
endif()

if(BUILD_STATIC_LIBS)
  add_library(${lib_name} STATIC  ${SOURCES} ${HEADERS})
  target_compile_features(${lib_name} PUBLIC cxx_std_17)
  set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
  set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME  "${lib_name_decorated}")
  target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${INCLUDE_DIRECTORIES}
  )
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
    VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion )

  # Compile Definitions
  target_compile_definitions(${lib_name} PUBLIC ${STATIC_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_name} PUBLIC ${STATIC_LINK_LIBRARIES})
endif()

# Post build
if(BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${lib_shared_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/modules/"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_shared_name}> "${RELEASE_DIR}/modules/"
        # Make a release directory without all the cmake extras.
        # This directory can easily be archived and distributed.
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/${application_name}/"
        # Create release sub-folders
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/fonts/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/modules/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/share/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/ShaderCache/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources/" "${RELEASE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/share/" "${RELEASE_DIR}/share"
    )
endif()

# Installation
if(BUILD_SHARED_LIBS)
  message(STATUS "INSTALL: ${lib_shared_name} TO root=${CMAKE_BINARY_DIR} and current=${CMAKE_CURRENT_BINARY_DIR}")
  install(TARGETS ${lib_shared_name}
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/modules"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/modules"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/modules"
  )
endif()

if(BUILD_STATIC_LIBS)
  install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/lib"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/lib"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/lib"
  )
endif()

install(FILES ${HEADERS}
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/headers"
COMPONENT Headers)

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/doc/" RENAME "${lib_name}_LICENSE.txt" )

add_executable(example
    src/example/main.cpp
)
target_include_directories(example PUBLIC
    src/
)
target_link_libraries(example PUBLIC
    ${lib_name}
    #SDL2main
    #SDL2
)

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}"
    #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:example> "${RELEASE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/lib/windows/" "${RELEASE_DIR}/"
)
