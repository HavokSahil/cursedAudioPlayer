cmake_minimum_required(VERSION 3.22)
project(cursedAudioPlayer)

# Using latest C++ Standard
set(CMAKE_CXX_STANDARD 20)

# Enable testing of modules
enable_testing()

# Find the Thread Package for pthread
find_package(Threads)

add_executable(cap
        cursedAudioPlayer.cpp
        CursedLayout.cpp
        CursedLayout.h
)

# Add the Testing directory
add_subdirectory(audio-system)
add_subdirectory(curses-widgets)
add_subdirectory(tests)

# Include headers
target_include_directories(cap PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/audio-system
        ${CMAKE_SOURCE_DIR}/curses-widgets
)

target_link_libraries(cap
        audio_system
        curses_widgets
)

set(MAKE_DOCS OFF)

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
install(TARGETS cap
        RUNTIME DESTINATION bin)

if (DOXYGEN_FOUND AND MAKE_DOCS)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/
            DESTINATION share/cursedAudioPlayer/docs
            OPTIONAL)
endif()