set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

set(UI_HEADERS include/ui/mainwindow.h
	include/ui/draw_widget.h
	include/ui/sim_interface.h
	)

set(UI_SRCS src/ui/mainwindow.cpp
	src/ui/draw_widget.cpp
	src/ui/sim_interface.cpp
	)

qt5_wrap_ui(ui_wrap mainwindow.ui)
qt5_wrap_cpp(moc_sources ${UI_HEADERS})
