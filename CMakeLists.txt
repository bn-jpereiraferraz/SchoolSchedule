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
    src/addclassdialog.cpp
    src/addclassdialog.h
    src/addeventdialog.cpp
    src/addeventdialog.h
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
# REST API Server
# ============================================
add_executable(SchoolCalendarServer
    server/main.cpp
)

target_link_libraries(SchoolCalendarServer
    SchoolCalendarShared
    pthread
)

target_include_directories(SchoolCalendarServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
)
