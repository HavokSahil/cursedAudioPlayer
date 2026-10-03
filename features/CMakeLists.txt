add_library(audio_analysis
        CQT.cpp
        CQTBackend.c
        ${CMAKE_SOURCE_DIR}/external/ecqt/pffft/pffft.c
)

set_target_properties(audio_analysis PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
target_include_directories(audio_analysis PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(audio_analysis PRIVATE ${CMAKE_SOURCE_DIR}/external/ecqt)
target_link_libraries(audio_analysis PRIVATE Threads::Threads m)
