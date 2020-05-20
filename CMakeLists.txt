cmake_minimum_required(VERSION 3.11)
# The FetchModule is only available since CMake 3.11.

set(lib_base_name CrazySentencesOnlineClient)
project(${lib_base_name} VERSION 1.0.0 LANGUAGES CXX)
set(RELEASE_DIR "${CMAKE_BINARY_DIR}/${lib_base_name}/")

# Configurable Options
OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_STATIC_LIBS "Build static libraries" ON)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)
include(CMakePackageConfigHelpers)

find_package(OGRE 1.09 REQUIRED)
find_package(Boost 1.56 REQUIRED COMPONENTS
             system filesystem)

# third party
set(INCLUDE_DIRECTORIES
  third_party/imgui/include/
  third_party/rapidjson/include/
  third_party/ogre/include/
  third_party/ogre/include/OGRE/
  #${OGRE_INCLUDE_DIRS}
  src/modules
)

set(LINK_DIRECTORIES
  third_party/imgui/lib/
  third_party/ogre/lib/
  third_party/ogre/lib/OGRE/
  #${OGRE_LIBRARY_DIRS}
  third_party/Module-Manager/lib/
  third_party/Settings-Manager/lib/
)

# MODULES
add_subdirectory(src/modules/)
add_subdirectory(third_party/)
add_subdirectory(third_party/assimp-xnalara/)
add_subdirectory(third_party/imgui/)
add_subdirectory(samples/viewer/)

# Set up source files
set(SOURCES
  src/Module.cpp

  src/core/AssimpLoader.cpp
  src/core/AssimpOgreUtility.cpp

  src/UI/Client.cpp
  src/UI/SceneMenu.cpp
  src/UI/RTShader.cpp
)

set(HEADERS
  ${PROJECT_SOURCE_DIR}/src/Module.hpp
  ${PROJECT_SOURCE_DIR}/src/Interface.hpp
  ${PROJECT_SOURCE_DIR}/src/core/config.hpp
  ${PROJECT_SOURCE_DIR}/src/core/AssimpLoader.hpp
  ${PROJECT_SOURCE_DIR}/src/core/AssimpOgreUtility.hpp

  ${PROJECT_SOURCE_DIR}/src/UI/Client.hpp
  ${PROJECT_SOURCE_DIR}/src/UI/SceneMenu.hpp
  ${PROJECT_SOURCE_DIR}/src/UI/RTShader.hpp
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
    RenderSystem_GL3PlusStatic
    OgreGLSupportStatic
    OgreMeshLodGeneratorStatic
    OgreRTShaderSystemStatic
    Codec_STBIStatic
    OgreMainStatic
    imgui
    SDL2main
    SDL2
    zzip
    zlibstatic

    # assimp
    assimp-xnalara

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
    )
elseif(UNIX)
    set(LIBRARIES ${LIBRARIES}
        #OgreGLSupport
        GLEW
        GL

        # Unix Libraries
    )
endif()


set(SHARED_LINK_LIBRARIES ${LIBRARIES})
set(STATIC_LINK_LIBRARIES ${LIBRARIES})

link_directories(${LINK_DIRECTORIES})

# Set up library names
if(BUILD_SHARED_LIBS)
  set(lib_shared_name ${lib_base_name}Shared)
  set(lib_shared_name_decorated ${lib_shared_name}${LIBNAME_ADDITION})
  set(${lib_base_name}_SHARED_LIBRARY ${lib_shared_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_shared_name}" )
endif(BUILD_SHARED_LIBS)

if(BUILD_STATIC_LIBS)
  set(lib_name ${lib_base_name})
  set(lib_name_decorated ${lib_name}${LIBNAME_ADDITION})
  set(${lib_base_name}_STATIC_LIBRARY ${lib_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_name}" )
endif(BUILD_STATIC_LIBS)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

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

# Pre build
if(BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${lib_shared_name} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/include/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/include/${lib_base_name}"
        COMMAND ${CMAKE_COMMAND} -E copy ${HEADERS} "${PROJECT_SOURCE_DIR}/include/${lib_base_name}"
    )
endif()

# Post build
if(BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${lib_shared_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/modules/"
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_shared_name}> "${RELEASE_DIR}/modules/"
        # Make a release directory without all the cmake extras.
        # This directory can easily be archived and distributed.
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/${application_name}/"
        # Create release sub-folders
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/fonts/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/modules/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/share/"
        #COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/RTShaderCache/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources/" "${RELEASE_DIR}"
        #COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/share/" "${RELEASE_DIR}/share"
    )
endif()

# Installation
if(BUILD_SHARED_LIBS)
  message(STATUS "INSTALL: ${lib_shared_name} TO root=${CMAKE_BINARY_DIR} and current=${CMAKE_CURRENT_BINARY_DIR}")
  install(TARGETS ${lib_shared_name}
    RUNTIME DESTINATION "${RELEASE_DIR}/modules"
    ARCHIVE DESTINATION "${RELEASE_DIR}/modules"
    LIBRARY DESTINATION "${RELEASE_DIR}/modules"
  )
endif()

if(BUILD_STATIC_LIBS)
  install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
    RUNTIME DESTINATION "${RELEASE_DIR}/lib"
    ARCHIVE DESTINATION "${RELEASE_DIR}/lib"
    LIBRARY DESTINATION "${RELEASE_DIR}/lib"
  )
endif()

install(FILES ${HEADERS}
  DESTINATION "${RELEASE_DIR}/headers"
COMPONENT Headers)

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${RELEASE_DIR}" RENAME "LICENSE.txt" )
