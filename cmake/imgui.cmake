set(IMGUI_ROOT ${CMAKE_SOURCE_DIR}/third_party/imgui)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD=1)

set(IMGUI_SOURCES
    "${IMGUI_ROOT}/imgui.cpp"
    "${IMGUI_ROOT}/imgui_demo.cpp"
    "${IMGUI_ROOT}/imgui_draw.cpp"
    "${IMGUI_ROOT}/imgui_tables.cpp"
    "${IMGUI_ROOT}/imgui_widgets.cpp"
    "${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp"
)

set(IMGUI_HEADERS
    "${IMGUI_ROOT}/imconfig.h"
    "${IMGUI_ROOT}/imgui.h"
    "${IMGUI_ROOT}/imgui_internal.h"
    "${IMGUI_ROOT}/imstb_rectpack.h"
    "${IMGUI_ROOT}/imstb_textedit.h"
    "${IMGUI_ROOT}/imstb_truetype.h"
    "${IMGUI_ROOT}/backends/imgui_impl_glfw.h"
    "${IMGUI_ROOT}/backends/imgui_impl_opengl3.h"
)

add_library(imgui
    STATIC
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS}
)

target_include_directories(imgui
    PUBLIC
    ${IMGUI_ROOT}
    ${IMGUI_ROOT}/backends
)

target_link_libraries(imgui
    LINK_PUBLIC
    glad
    glfw
    OpenGL
)
