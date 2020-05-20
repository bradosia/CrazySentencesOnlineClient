# set(CMAKE_EXE_LINKER_FLAGS " -static -static-libgcc -static-libstdc++")

add_executable(viewer
    main.cpp
)

target_include_directories(viewer PUBLIC
    ../../third_party/imgui/include/
    ../../third_party/rapidjson/include/
    ../../third_party/ogre/include/
    ../../third_party/ogre/include/OGRE/
    #${OGRE_INCLUDE_DIRS}
    ../../src/
    ../../src/modules
)

target_link_directories(viewer PUBLIC
    ../../third_party/imgui/lib/
    ../../third_party/ogre/lib/
    ../../third_party/ogre/lib/OGRE/
    #${OGRE_LIBRARY_DIRS}
    ../../third_party/Module-Manager/lib/
    ../../third_party/Settings-Manager/lib/
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(viewer PUBLIC
    CrazySentencesOnlineClient
)

message( STATUS "Viewer RELEASE_DIR = ${RELEASE_DIR}" )

add_custom_command(TARGET viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:viewer> "${RELEASE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/lib/mingw64/" "${RELEASE_DIR}"
)
