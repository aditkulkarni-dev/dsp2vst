#include "FDNReverb.h"
#include <cmath>

void FDNReverb::process(float* data, int numSamples) {
    // Standard c weights (input to delay lines) - assuming 1.0 for simplicity
    constexpr float c = 1.0f; 
    constexpr float g = 0.8f;

    for (int n = 0; n < numSamples; ++n) {
        float x = data[n]; // Current input sample
        
        // Stack-allocated arrays for this specific sample.
        std::array<float, 4> Y = {0.0f, 0.0f, 0.0f, 0.0f};
        std::array<float, 4> d_out;

        // Read the delayed signals: D(n-k)
        for (int i = 0; i < 4; ++i) {
            float rawDelay = delayLines[i].read();
            d_out[i] = dampingFilters[i].process(rawDelay) * gains[i]; 
        }

        // Perform the Matrix Multiplication + Input addition
        // Y = (matrix * d_out) + (c * x)
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                Y[row] += matrix[row][col] * d_out[col];
            }
            Y[row] += c * x; // Adding the direct input injection
        }

        // The Transform (Linear combination for the output)
        float v = (Y[0] + Y[1] + Y[2] + Y[3]) * 0.25f;

        // Write Y back into the delay lines for the next feedback loop
        for (int i = 0; i < 4; ++i) {
            delayLines[i].write(Y[i]);
            delayLines[i].advance();   // Move the buffer pointers forward
        }

        // Output the sample
        data[n] = v; 
    }
}

void FDNReverb::setReverbTime(float rt60InSeconds, float sampleRate) {
    gains.resize(k.size());
    for (size_t i = 0; i < k.size(); ++i) {
        // g_i = 10^(-3 * k_i / (RT60 * SR))
        float exponent = (-3.0f * static_cast<float>(k[i])) / (rt60InSeconds * sampleRate);
        gains[i] = std::pow(10.0f, exponent);
    }
}

std::unique_ptr<Effect> FDNReverb::clone() const
{
    return std::make_unique<FDNReverb>(*this); 
}
