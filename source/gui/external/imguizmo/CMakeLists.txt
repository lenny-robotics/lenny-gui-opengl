#Source: https://github.com/CedricGuillemet/ImGuizmo

project(imguizmo)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}
        )

target_link_libraries(${PROJECT_NAME}
    PUBLIC "imgui" #
)
