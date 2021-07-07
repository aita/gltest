add_executable(Triangle
    main.cc
)

target_link_libraries(Triangle
    LINK_PUBLIC
    glad
    glfw
    OpenGL
    glm
    spdlog
)
