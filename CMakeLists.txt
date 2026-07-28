cmake_minimum_required(VERSION 3.16)

project(DingooPackageTool VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

configure_file(
    src/package_tool_version.h.in
    generated/package_tool_version.h
    @ONLY
)

set(DINGOO_PACKAGE_TOOL_CORE_SOURCES
    src/package_format.cpp
    src/package_format.h
    src/file_util.cpp
    src/file_util.h
    src/json_manifest.cpp
    src/json_manifest.h
    src/stx_editor.cpp
    src/stx_editor.h
)

add_executable(dingoo-package-tool
    ${DINGOO_PACKAGE_TOOL_CORE_SOURCES}
    src/main.cpp
)

if(WIN32)
    target_link_libraries(dingoo-package-tool PRIVATE shell32)
    add_executable(dingoo-package-tool-gui WIN32
        ${DINGOO_PACKAGE_TOOL_CORE_SOURCES}
        src/package_gui.cpp
    )
    target_link_libraries(dingoo-package-tool-gui PRIVATE shell32 comdlg32 comctl32 ole32)
endif()

set(DINGOO_PACKAGE_TOOL_TARGETS dingoo-package-tool)
if(WIN32)
    list(APPEND DINGOO_PACKAGE_TOOL_TARGETS dingoo-package-tool-gui)
endif()

foreach(target_name IN LISTS DINGOO_PACKAGE_TOOL_TARGETS)
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -pedantic)
        if(MINGW)
            target_link_options(${target_name} PRIVATE -static-libgcc -static-libstdc++ -static)
        endif()
    endif()
endforeach()
