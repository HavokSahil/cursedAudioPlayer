#include "CQTBackend.h"
#include "ecqt.h"

struct CQTBackend {
    CQTContext* context;
    CQTResult* result;
};

CQTBackend* cap_ecqt_create(float sampleRate, float minFrequency, int binsPerOctave) {
    CQTBackend* backend = calloc(1, sizeof(CQTBackend));
    if (!backend) return NULL;
    backend->context = cqtcontext_init(minFrequency, sampleRate, binsPerOctave, 0.001f);
    backend->result = cqtresult_init(minFrequency, sampleRate, binsPerOctave);
    if (!backend->context || !backend->result) {
        cap_ecqt_destroy(backend);
        return NULL;
    }
    return backend;
}

void cap_ecqt_destroy(CQTBackend* backend) {
    if (!backend) return;
    cqtcontext_deinit(backend->context);
    cqtresult_deinit(backend->result);
    free(backend);
}

int cap_ecqt_length(const CQTBackend* backend) {
    return backend->context->inlen;
}

int cap_ecqt_bins(const CQTBackend* backend) {
    return backend->context->bins;
}

int cap_ecqt_transform(CQTBackend* backend, const float* input, int size, float* output) {
    if (!backend || !input || !output || size < 0) return -1;
    int status = cqtcontext_transform(backend->context, (float*)input, size, backend->result);
    if (status != 0) return status;
    // Compensate the real sine's half-amplitude and the Hamming window's mean.
    const float gain = 2.0f / (25.0f / 46.0f);
    for (int i = 0; i < backend->result->bins; ++i)
        output[i] = gain * ecqt_cabs(backend->result->prv->entries[i]);
    return 0;
}
