add_executable(Cube
    main.cc
)

target_link_libraries(Cube
    LINK_PUBLIC
    glad
    glfw
    OpenGL
    glm
    spdlog
)
