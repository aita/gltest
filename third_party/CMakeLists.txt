set(ASSIMP_INSTALL OFF)
add_subdirectory(assimp)

set(GLAD_ALL_EXTENSIONS ON)
add_subdirectory(glad)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw)

add_subdirectory(glm)

add_subdirectory(spdlog)

# set(SAIL_BUILD_EXAMPLES OFF)
# add_subdirectory(sail)