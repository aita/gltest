set(STB_ROOT ${CMAKE_SOURCE_DIR}/third_party/stb)

file(GLOB
    STB_HEADERS
    ${STB_ROOT}/*.h
)

add_library(stb
    INTERFACE
    ${STB_HEADERS}
)

target_include_directories(stb
    INTERFACE
    ${STB_ROOT}/${STB_SOURCES}
)
