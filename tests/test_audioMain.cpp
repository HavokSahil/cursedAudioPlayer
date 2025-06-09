#define SUCCESS 0

int code;
#define CHECK(FN) code = FN; if (code != SUCCESS) return code

int test_audio_decoder();
int test_audio_encoder();
int test_ring_buffer();
int test_audio_player();
int test_audio_system();

int main() {
    CHECK(test_audio_decoder());
    CHECK(test_ring_buffer());
    CHECK(test_audio_player());
    CHECK(test_audio_system());
    return SUCCESS;
}