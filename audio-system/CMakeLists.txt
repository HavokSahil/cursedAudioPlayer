add_library(audio_system
        AudioPlayer.cpp
        AudioSystem.cpp
        AudioDecoder.cpp
        RingBuffer.cpp
        IAudioSink.h
)

target_include_directories(audio_system PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(audio_system PRIVATE
        Threads::Threads
        m
        dl
)