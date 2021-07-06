add_executable(hello_imgui
    main.cc
)

target_link_libraries(hello_imgui
    LINK_PUBLIC
    glfw
    OpenGL
    glm
    imgui
)
