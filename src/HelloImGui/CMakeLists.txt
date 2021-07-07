add_executable(HelloImGui
    main.cc
)

target_link_libraries(HelloImGui
    LINK_PUBLIC
    glfw
    OpenGL
    glm
    imgui
)
