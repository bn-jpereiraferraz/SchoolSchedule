cmake_minimum_required(VERSION 3.16)

# Enable Qt Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Enable testing
enable_testing()

# Helper function to create a test
function(add_qt_test test_name)
    add_executable(${test_name} ${ARGN})

    # Link to shared library and Qt Test
    target_link_libraries(${test_name}
        SchoolCalendarShared
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
    )

    # Include directories for headers
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/shared
        ${CMAKE_SOURCE_DIR}/src
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Helper for tests that need client sources
function(add_qt_test_with_sources test_name)
    cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})

    add_executable(${test_name} ${ARG_UNPARSED_ARGUMENTS} ${ARG_SOURCES})

    target_link_libraries(${test_name}
        SchoolCalendarShared
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
    )

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/shared
        ${CMAKE_SOURCE_DIR}/src
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add individual tests
add_subdirectory(test_jsonhelpers)
add_subdirectory(test_entityfinder)
add_subdirectory(test_displayformatter)
