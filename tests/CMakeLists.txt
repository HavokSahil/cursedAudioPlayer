add_compile_definitions(TEST_MEDIA_DIR="${CMAKE_SOURCE_DIR}/tests/media")

add_executable(audio_test
        test_audioMain.cpp
        test_audioSystem.cpp
        test_audioDecoder.cpp
        test_audioPlayer.cpp
        test_ringBuffer.cpp
)

target_link_libraries(audio_test
        audio_system)

target_include_directories(audio_test PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/audio-system)

# Add the audio module test
add_test(NAME audio_test COMMAND audio_test)