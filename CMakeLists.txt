cmake_minimum_required(VERSION 3.22)
project(cursedAudioPlayer VERSION 1.1.1 LANGUAGES C CXX)
include(GNUInstallDirs)

# Using latest C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing of modules
enable_testing()

# Find the Thread Package for pthread
find_package(Threads REQUIRED)

add_executable(cursedap
        cursedAudioPlayer.cpp
        CursedLayout.cpp
        CursedLayout.h
)

# Add the Testing directory
add_subdirectory(audio-system)
add_subdirectory(features)
add_subdirectory(curses-widgets)
add_subdirectory(tests)

# Include headers
target_include_directories(cursedap PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/audio-system
        ${CMAKE_SOURCE_DIR}/curses-widgets
)

target_link_libraries(cursedap
        audio_system
        audio_analysis
        curses_widgets
)

option(MAKE_DOCS "Generate API documentation" OFF)

find_package(Doxygen)
if(DOXYGEN_FOUND AND MAKE_DOCS)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs)

    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif()


# Install the binary
install(TARGETS cursedap
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (DOXYGEN_FOUND AND MAKE_DOCS)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/
            DESTINATION share/cursedAudioPlayer/docs
            OPTIONAL)
endif()
install(FILES packaging/cursedap.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/cursedap)
install(FILES external/ecqt/LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/cursedap RENAME ECQT-MIT)
install(FILES packaging/licenses/PFFFT packaging/licenses/miniaudio-MIT-0
        DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/cursedap)
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/cursedap)
include(${CMAKE_CURRENT_SOURCE_DIR}/packaging/CPackConfig.cmake)
