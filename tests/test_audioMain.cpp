#include <string>

int test_audio_decoder();
int test_ring_buffer();
int test_audio_player();
int test_audio_system();
int test_analysis();
int test_cqt();

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--device") {
        if (int code = test_audio_player()) return code;
        return test_audio_system();
    }
    if (int code = test_audio_decoder()) return code;
    if (int code = test_ring_buffer()) return code;
    if (int code = test_analysis()) return code;
    return test_cqt();
}
