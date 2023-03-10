# set new cmake policy
set_new_cmake_policy()

# glad
if (NOT TARGET glad)
    set(OpenGL_GL_PREFERENCE GLVND)
    add_subdirectory(glad)
endif ()

# glfw
if (NOT TARGET glfw)
    FetchContent_Declare(
            glfw
            URL https://github.com/glfw/glfw/archive/3.3.8.zip
            URL_HASH MD5=b06789a44e82a8a3ebd2e0301fb0484a
            UPDATE_COMMAND "" CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND ""
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
    set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
    fetch_repository(glfw)
    add_subdirectory(${glfw_SOURCE_DIR} glfw)
endif ()

# glm
if (NOT TARGET glm)
    FetchContent_Declare(
            glm #
            URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
            URL_HASH MD5=69895110052f0d711c9c54fbf385f6f5 #
    )
    set(GLM_TEST_ENABLE OFF CACHE BOOL "GLM lib only")
    fetch_repository(glm)
    add_subdirectory(${glm_SOURCE_DIR} glm)
endif ()

# imgui
if (NOT TARGET imgui)
    #FetchContent_Declare(
    #        imgui #
    #        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.88.zip #
    #        URL_HASH MD5=dcbd7d7786e1b507af3be287d37fd1ae #
    #)
    #fetch_repository(imgui)

    ###Comment: Downloaded imgui from `docking` on 15.01.2023 (https://github.com/ocornut/imgui/tree/docking)
    set(imgui_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking)

    file(GLOB IMGUI_SRC #
            ${imgui_SOURCE_DIR}/*.cpp #
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp #
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp #
            )

    add_library(imgui STATIC ${IMGUI_SRC})

    target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLAD)

    set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

    target_include_directories(imgui PUBLIC
            ${imgui_SOURCE_DIR} #
            ${imgui_SOURCE_DIR}/backends #
            )

    target_link_libraries(imgui PUBLIC
            glfw #
            glad #
            )

    set(IMGUI_FONT_FOLDER
            "\"${imgui_SOURCE_DIR}/misc/fonts\""
            CACHE STRING ""
            )
endif ()

# imguizmo
if (NOT TARGET imguizmo)
    add_subdirectory(imguizmo)
endif ()

# implot
if (NOT TARGET implot)
    add_subdirectory(implot)
endif ()

# stb_image
if (NOT TARGET stb_image)
    add_subdirectory(stb_image)
endif ()

# assimp
if (NOT TARGET assimp)
    FetchContent_Declare(
            assimp #
            URL https://github.com/assimp/assimp/archive/refs/tags/v5.2.4.zip #
            URL_HASH MD5=056d1c419daab16e4227fccad711fc97 #
    )
    fetch_repository(assimp)

    #Settings
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build package with shared libraries.")
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "If the official samples are built as well (needs Glut).")
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library.")
    set(ASSIMP_INSTALL OFF CACHE BOOL "Disable this if you want to use assimp as a submodule.")

    #Importers
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "default value of all ASSIMP_BUILD_XXX_IMPORTER values")
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "OBJ Importer")
    set(ASSIMP_BUILD_STL_IMPORTER ON CACHE BOOL "STL Importer")
    set(ASSIMP_BUILD_COLLADA_IMPORTER ON CACHE BOOL "COLLADA Importer")

    #Exporters
    set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "default value of all ASSIMP_BUILD_XXX_EXPORTER values")
    set(ASSIMP_BUILD_OBJ_EXPORTER ON CACHE BOOL "OBJ Exporter")

    add_subdirectory(${assimp_SOURCE_DIR} assimp)
endif ()

# meshoptimizer
if (NOT TARGET meshoptimizer)
    FetchContent_Declare(
            meshoptimizer #
            URL https://github.com/zeux/meshoptimizer/archive/refs/tags/v0.18.zip #
            URL_HASH MD5=E2BF4D9E82410E9EABA8776C2DE98D7B #
    )
    fetch_repository(meshoptimizer)
    add_subdirectory(${meshoptimizer_SOURCE_DIR} meshoptimizer)
endif ()