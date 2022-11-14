include(FetchContent)
include(CMakeDependentOption)

function(capitalize output input)
    set(var ${input})
    string(REPLACE "-" "_" var ${var})
    string(TOUPPER ${var} var)
    set(${output} ${var} PARENT_SCOPE)
endfunction()

macro(fetch_repository repo)
    FetchContent_GetProperties("${repo}")
    if (NOT ${${repo}_POPULATED})
        message(STATUS "Fetching repository '${repo}' ...")
        FetchContent_Populate(${repo})
    endif ()
    mark_as_advanced(${${repo}_SOURCE_DIR})
endmacro()

macro(fetch_lenny_repository repo tag)
    FetchContent_Declare(
            lenny-${repo} #
            GIT_REPOSITORY git@github.com:lenny-robotics/lenny-${repo}.git #
            GIT_TAG ${tag} #
    )
    fetch_repository(lenny-${repo})
    capitalize(LENNY_REPO lenny-${repo})
    set(${LENNY_REPO}_BUILD_APPS OFF CACHE BOOL "Build apps")
    add_subdirectory(${lenny-${repo}_SOURCE_DIR} lenny-${repo})
endmacro()