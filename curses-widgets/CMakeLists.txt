FIND_PACKAGE(Curses REQUIRED)

add_library(curses_widgets
        MainWindow.cpp
        Column.cpp
        Row.cpp
        Button.cpp
        ProgressBar.cpp
        BoxedContainer.cpp
        Widget.cpp
        TextBox.cpp
        TextBox.h
        BarPlot.cpp
        BarPlot.h
        FHexStream.cpp
        FHexStream.h
)

target_include_directories(curses_widgets PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CURSES_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/curses-widgets
)

target_link_libraries(curses_widgets PRIVATE
        ${CURSES_LIBRARIES}
)