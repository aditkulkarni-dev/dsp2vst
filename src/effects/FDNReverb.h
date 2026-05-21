#pragma once // Good practice to prevent double inclusions
#include "Effect.h"
#include "../dsp/LowPassFilter.h"
#include <iostream>
#include "../dsp/CircularBuffer.h"
#include <vector>
#include <memory>

class FDNReverb : public Effect {
public:
    // Constructor now takes 'k' and dynamically scales everything
    FDNReverb(std::vector<int> k);

    void process(float* data, int numSamples) override; // assuming it overrides from Effect
    void setReverbTime(float rt60InSeconds, float sampleRate);
    std::unique_ptr<Effect> clone() const override;

private:
    // Helper function to build the dynamic Householder matrix
    void generateHouseholderMatrix();

    int N; // The dynamic size of our FDN (number of delay lines)
    std::vector<int> k;
    std::vector<CircularBuffer> delayLines;
    std::vector<LowPassFilter> dampingFilters;
    std::vector<float> gains;

    // A dynamically sized 2D matrix
    std::vector<std::vector<float>> matrix;

    // Pre-allocated memory for the audio thread
    std::vector<float> Y;
    std::vector<float> d_out;
};