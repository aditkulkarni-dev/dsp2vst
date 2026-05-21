#include "FDNReverb.h"

void FDNReverb::process(float* data, int numSamples) {
    // Standard c weights (input to delay lines) - assuming 1.0 for simplicity
    constexpr float c = 1.0f; 
    constexpr float g = 0.7f;

    for (int n = 0; n < numSamples; ++n) {
        float x = data[n]; // Current input sample
        
        // Stack-allocated arrays for this specific sample. Zero heap allocation!
        std::array<float, 4> Y = {0.0f, 0.0f, 0.0f, 0.0f};
        std::array<float, 4> d_out;

        // 1. Read the delayed signals: D(n-k)
        for (int i = 0; i < 4; ++i) {
            d_out[i] = delayLines[i].read() * g; // Assuming CircularBuffer has a read()
        }

        // 2. Perform the Matrix Multiplication + Input addition
        // Y = (matrix * d_out) + (c * x)
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                Y[row] += matrix[row][col] * d_out[col];
            }
            Y[row] += c * x; // Adding the direct input injection
        }

        // 3. The Transform (Linear combination for the output)
        float v = (Y[0] + Y[1] + Y[2] + Y[3]) * 0.25f;

        // 4. Write Y back into the delay lines for the next feedback loop
        for (int i = 0; i < 4; ++i) {
            delayLines[i].write(Y[i]); // Assuming CircularBuffer has a write()
            delayLines[i].advance();   // Move the buffer pointers forward
        }

        // 5. Output the sample
        data[n] = v; // Or mix with dry signal: (x * dry) + (v * wet)
    }
}

std::unique_ptr<Effect> FDNReverb::clone() const
{
    return std::make_unique<FDNReverb>(*this); 
}
