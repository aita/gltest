add_executable(triangle
    main.cc
)

target_link_libraries(triangle
    LINK_PUBLIC
    glad
    glfw
    OpenGL
    glm
    spdlog
)
