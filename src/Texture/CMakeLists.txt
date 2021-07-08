add_executable(Texture
    main.cc
    image_impl.cc
)

target_link_libraries(Texture
    LINK_PUBLIC
    glad
    glfw
    OpenGL
    glm
    spdlog
    stb
)
