add_executable(Icosphere
    main.cc
)

target_link_libraries(Icosphere
    LINK_PUBLIC
    glad
    glfw
    OpenGL
    glm
    spdlog
)
