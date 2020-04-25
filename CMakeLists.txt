project(logicsim)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(make_ui)
include_directories(include include/ui)

add_executable( ${PROJECT_NAME} main.cpp
	${UI_SRCS}
	${ui_wrap}
	${moc_sources}
    src/ui/mainwindow.cpp
	)
target_link_libraries(${PROJECT_NAME} stdc++fs)

qt5_use_modules( ${PROJECT_NAME} Core Gui Widgets)

enable_testing()
add_executable(unit_and tests/unit_and/main.cpp)
#add_executable(unit_or tests/unit_or/main.cpp)
#add_executable(unit_not tests/unit_not/main.cpp)
add_test(unit_and unit_and 0 0)

