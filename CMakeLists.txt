cmake_minimum_required(VERSION 3.16)
project(SchoolCalendar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# Shared Library (Data Model)
# ============================================
add_library(SchoolCalendarShared STATIC
    shared/scheduledata.cpp
    shared/scheduledata.h
    shared/jsonhelpers.cpp
    shared/jsonhelpers.h
    shared/entityfinder.cpp
    shared/entityfinder.h
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(SchoolCalendarShared Qt6::Core)
target_include_directories(SchoolCalendarShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/shared)

# ============================================
# Qt Client (GUI Application)
# ============================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(SchoolCalendar
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/customcalendar.cpp
    src/customcalendar.h
    src/schedulerepository.cpp
    src/schedulerepository.h
    src/basedialog.cpp
    src/basedialog.h
    src/addclassdialog.cpp
    src/addclassdialog.h
    src/addeventdialog.cpp
    src/addeventdialog.h
    src/networkrequestbuilder.cpp
    src/networkrequestbuilder.h
    src/jsonbuilders.cpp
    src/jsonbuilders.h
    src/apiresponsehandler.cpp
    src/apiresponsehandler.h
    src/displayformatter.cpp
    src/displayformatter.h
    src/filterstate.cpp
    src/filterstate.h
    src/eventlistbuilder.cpp
    src/eventlistbuilder.h
    sync/caldavtester.cpp
    sync/caldavtester.h
)

target_link_libraries(SchoolCalendar
    SchoolCalendarShared
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

target_include_directories(SchoolCalendar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
)

# ============================================
# CalDAV Connection Test
# ============================================
add_executable(caldav_test
    sync/caldav_test.cpp
    sync/caldavtester.cpp
    sync/caldavtester.h
)

target_link_libraries(caldav_test
    Qt6::Core
    Qt6::Network
)

target_include_directories(caldav_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sync
)

# ============================================
# REST API Server
# ============================================
add_executable(SchoolCalendarServer
    server/main.cpp
    server/requesthandler.cpp
    server/requesthandler.h
    server/jsonconverter.cpp
    server/jsonconverter.h
    server/idgenerator.cpp
    server/idgenerator.h
    server/schoolcalendarserver.cpp
    server/schoolcalendarserver.h
    server/handlers/authmiddleware.cpp
    server/handlers/authmiddleware.h
    server/handlers/classhandler.cpp
    server/handlers/classhandler.h
    server/handlers/eventhandler.cpp
    server/handlers/eventhandler.h
)

target_link_libraries(SchoolCalendarServer
    SchoolCalendarShared
    pthread
)

target_include_directories(SchoolCalendarServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
)

# ============================================
# Tests
# ============================================
option(BUILD_TESTS "Build the tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
