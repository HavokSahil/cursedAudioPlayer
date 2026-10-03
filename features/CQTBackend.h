#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CQTBackend CQTBackend;
CQTBackend* cap_ecqt_create(float sampleRate, float minFrequency, int binsPerOctave);
void cap_ecqt_destroy(CQTBackend* backend);
int cap_ecqt_length(const CQTBackend* backend);
int cap_ecqt_bins(const CQTBackend* backend);
int cap_ecqt_transform(CQTBackend* backend, const float* input, int size, float* output);

#ifdef __cplusplus
}
#endif
