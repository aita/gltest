add_executable(Matrix
    main.cc
)

target_link_libraries(Matrix
    LINK_PUBLIC
    glad
    glfw
    OpenGL
    glm
    spdlog
)
