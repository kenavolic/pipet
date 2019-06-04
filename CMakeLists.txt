# Minimum CMake required
cmake_minimum_required(VERSION 3.9)

# Project
project(pipet LANGUAGES CXX VERSION 0.2.0)

# Options
option(PIPET_BUILD_TESTS "Build tests" ON)
option(PIPET_BUILD_EXAMPLES "Build examples" ON)
option(PIPET_INCLUDE_EXTRA "Include extra headers" ON)

if (PIPET_BUILD_EXAMPLES AND NOT PIPET_INCLUDE_EXTRA)
    message(FATAL_ERROR "Building examples require the PIPET_INCLUDE_EXTRA option")
endif()

# General Config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compilation flags
if (MSVC)
    set(PIPET_TEST_COMPILE_OPTIONS_DEBUG "/MTd")
    set(PIPET_TEST_COMPILE_OPTIONS_RELEASE "/MT")
endif()

# Dependencies
add_subdirectory(third_party)

# Lib
set (PIPET_LIB pipet)

set (PIPET_CORE_INCL
    ${PROJECT_SOURCE_DIR}/include/pipet/filter.h
    ${PROJECT_SOURCE_DIR}/include/pipet/pipet.h
)

set (PIPET_HELPERS_INCL
    ${PROJECT_SOURCE_DIR}/include/pipet/helpers/reflect.h
    ${PROJECT_SOURCE_DIR}/include/pipet/helpers/typelist.h
    ${PROJECT_SOURCE_DIR}/include/pipet/helpers/utils.h
)

set (PIPET_EXTRA_INCL
    ${PROJECT_SOURCE_DIR}/include/pipet/extra/bit.h
    ${PROJECT_SOURCE_DIR}/include/pipet/extra/cxstring.h
    ${PROJECT_SOURCE_DIR}/include/pipet/extra/random.h
)

set (PIPET_INCL ${PIPET_CORE_INCL} ${PIPET_HELPERS_INCL})

if (PIPET_INCLUDE_EXTRA)
    set (PIPET_INCL ${PIPET_INCL} ${PIPET_EXTRA_INCL})
endif()

add_library(${PIPET_LIB} INTERFACE)
target_include_directories(
    ${PIPET_LIB} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(${PIPET_LIB} INTERFACE cxx_std_17)

if (MSVC)
    add_custom_target(${PIPET_LIB}_headers SOURCES ${PIPET_INCL})
    set_target_properties(${PIPET_LIB}_headers PROPERTIES FOLDER "lib")
endif()

# Tests
if (PIPET_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if (PIPET_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/PipetConfigVersion.cmake" 
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)
install(TARGETS ${PIPET_LIB} 
    EXPORT PipetTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/PipetConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/PipetConfig.cmake"
    INSTALL_DESTINATION cmake/
)
install(EXPORT PipetTargets NAMESPACE pipet:: DESTINATION cmake/)
install(FILES "${PROJECT_BINARY_DIR}/PipetConfig.cmake" "${PROJECT_BINARY_DIR}/PipetConfigVersion.cmake" DESTINATION cmake/)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION .)

# Summary
message(STATUS "Configuration summary")
message(STATUS "-- Project name                 : ${PROJECT_NAME}")
message(STATUS "-- Project version              : ${PROJECT_VERSION}")
message(STATUS "-- Include extra                : ${PIPET_INCLUDE_EXTRA}")
message(STATUS "-- Build examples               : ${PIPET_BUILD_EXAMPLES}")
message(STATUS "-- Build tests                  : ${PIPET_BUILD_TESTS}")
message(STATUS "-- Install dir                  : ${CMAKE_INSTALL_PREFIX}")
