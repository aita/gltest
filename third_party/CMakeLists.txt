set(ASSIMP_INSTALL OFF)
add_subdirectory(assimp EXCLUDE_FROM_ALL)

set(GLAD_ALL_EXTENSIONS ON)
add_subdirectory(glad EXCLUDE_FROM_ALL)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw EXCLUDE_FROM_ALL)

add_subdirectory(glm EXCLUDE_FROM_ALL)

add_subdirectory(spdlog EXCLUDE_FROM_ALL)

# set(SAIL_BUILD_EXAMPLES OFF)
# add_subdirectory(sail)