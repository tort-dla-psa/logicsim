project(logicsim)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON )

include(make_ui)
include_directories(include include/ui tests
	submodules/nlohmann-json/single_include
	submodules/k-tree)

add_executable( ${PROJECT_NAME} main.cpp
	${UI_SRCS}
	${ui_wrap}
	${moc_sources}
    src/ui/mainwindow.cpp
	)
target_link_libraries(${PROJECT_NAME} stdc++fs)

qt5_use_modules( ${PROJECT_NAME} Core Gui Widgets)

enable_testing()

add_executable(test_save_load tests/save_load/main.cpp)
target_link_libraries(test_save_load stdc++fs)
add_test(test_save_load test_save_load)

add_executable(test_save_load_gui tests/save_load_gui/main.cpp)
target_link_libraries(test_save_load_gui stdc++fs)
add_test(test_save_load_gui test_save_load_gui)

add_executable(test_full_adder tests/full_adder/main.cpp)
target_link_libraries(test_full_adder stdc++fs)
add_test(test_full_adder test_full_adder)
