cmake_minimum_required(VERSION 3.21)

project(FloatingDamageSKSE VERSION 1.0.0 LANGUAGES CXX)

set(SOURCES
    src/main.cpp
    src/Hooks.cpp
    src/Settings.cpp
    src/FloatingDamageManager.cpp
    src/Menu.cpp
    src/Scaleform/Scaleform.cpp
    src/Scaleform/FloatingDamageMenu.cpp
)

set(HEADERS
    src/PCH.h
    src/Hooks.h
    src/Settings.h
    src/FloatingDamageManager.h
    src/Menu.h
    src/Offsets.h
    src/SKSEMenuFramework.h
    src/logger.h
    src/Scaleform/Scaleform.h
    src/Scaleform/FloatingDamageMenu.h
)

set(OUTPUT_FOLDER "${CMAKE_SOURCE_DIR}/Compile")

find_package(CommonLibSSE CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CommonLibSSE::CommonLibSSE
    spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /permissive-
        /Zc:preprocessor
        /W4
        /WX-
        /wd4100
        /wd4189
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    )
endif()

if(DEFINED OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_SOURCE_DIR}/FloatingDamageSKSE.ini"
            "${DLL_FOLDER}/FloatingDamageSKSE.ini"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/Interface"
            "${OUTPUT_FOLDER}/Interface"
        VERBATIM
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_PDB_FILE:${PROJECT_NAME}>"
                "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM
        )
    endif()
endif()

