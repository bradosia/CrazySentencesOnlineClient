add_executable(viewer
    main.cpp
)
target_include_directories(viewer PUBLIC
    ../../include/
)
target_link_libraries(viewer PUBLIC
    CrazySentencesOnlineClient
)

add_custom_command(TARGET viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:viewer> "${RELEASE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/lib/windows/" "${RELEASE_DIR}/"
)
