add_compile_definitions(TEST_MEDIA_DIR="${CMAKE_SOURCE_DIR}/tests/media")

add_executable(audio_test
        test_audioMain.cpp
        test_audioSystem.cpp
        test_audioDecoder.cpp
        test_audioPlayer.cpp
        test_ringBuffer.cpp
        test_analysis.cpp
        test_cqt.cpp
)

target_link_libraries(audio_test
        audio_system
        audio_analysis)

target_include_directories(audio_test PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/audio-system
        ${CMAKE_SOURCE_DIR})

# Add the audio module test
add_test(NAME audio_test COMMAND audio_test)
option(TEST_AUDIO_DEVICE "Run tests requiring a working audio output device" OFF)
if(TEST_AUDIO_DEVICE)
    add_test(NAME audio_device_test COMMAND audio_test --device)
    set_tests_properties(audio_device_test PROPERTIES TIMEOUT 120)
endif()
# Assertions contain the checks (and decoder operations), including in Release builds.
if(MSVC)
    target_compile_options(audio_test PRIVATE /UNDEBUG)
else()
    target_compile_options(audio_test PRIVATE -UNDEBUG)
endif()
