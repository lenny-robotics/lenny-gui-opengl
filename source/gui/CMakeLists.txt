# External
add_subdirectory(external)

# Project
project(gui)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        )

add_library(${PROJECT_NAME} ${sources})
add_library(lenny::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
        PUBLIC glm #
        PUBLIC imgui #
        PUBLIC imguizmo #
        PUBLIC implot #
        PUBLIC stb_image #
        PUBLIC assimp #
        PUBLIC meshoptimizer #
        PUBLIC lenny::tools #
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

set(LENNY_GUI_OPENGL_FOLDER "\"${CMAKE_CURRENT_LIST_DIR}\"" CACHE STRING "")

target_compile_definitions(${PROJECT_NAME}
        PUBLIC "LENNY_GUI_OPENGL_FOLDER=${LENNY_GUI_OPENGL_FOLDER}" #
        PUBLIC "IMGUI_FONT_FOLDER=${IMGUI_FONT_FOLDER}" #
        PUBLIC "IMGUI_IMPL_OPENGL_LOADER_GLAD" #
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
