project(TestApp)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

add_executable(${PROJECT_NAME} ${sources})

target_link_libraries(${PROJECT_NAME}
        PUBLIC lenny::gui
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

set(LENNY_GUI_TESTAPP_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}\"" CACHE STRING "")

target_compile_definitions(${PROJECT_NAME}
        PUBLIC LENNY_PROJECT_FOLDER=${LENNY_PROJECT_FOLDER}
        PUBLIC LENNY_GUI_TESTAPP_FOLDER=${LENNY_GUI_TESTAPP_FOLDER}
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
