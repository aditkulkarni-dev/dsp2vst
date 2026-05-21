#include "FDNReverb.h"
#include <cmath>
#include <algorithm>

FDNReverb::FDNReverb(std::vector<int> k) : k(k), N(k.size()) {
    delayLines.resize(N);
    dampingFilters.resize(N);
    
    // PRE-ALLOCATE processing vectors to match size N
    Y.resize(N, 0.0f);
    d_out.resize(N, 0.0f);

    for (int i = 0; i < N; i++) {
        delayLines[i].setSize(k[i]);
        dampingFilters[i].setCoefficient(0.5f);
    }

    // Generate the dynamic N x N matrix
    generateHouseholderMatrix();

    // Default target
    setReverbTime(2.5f, 44100.0f);
}

void FDNReverb::generateHouseholderMatrix() {
    // Resize the 2D vector to N x N
    matrix.resize(N, std::vector<float>(N, 0.0f));
    
    // The Householder multiplier constant: 2 / N
    float multiplier = 2.0f / static_cast<float>(N);
    
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            if (row == col) {
                // The diagonal gets (1 - 2/N)
                matrix[row][col] = 1.0f - multiplier;
            } else {
                // Everywhere else gets (-2/N)
                matrix[row][col] = -multiplier;
            }
        }
    }
}

void FDNReverb::setReverbTime(float rt60InSeconds, float sampleRate) {
    gains.resize(N);
    for (int i = 0; i < N; ++i) {
        float exponent = (-3.0f * static_cast<float>(k[i])) / (rt60InSeconds * sampleRate);
        gains[i] = std::pow(10.0f, exponent);
    }
}

void FDNReverb::process(float* data, int numSamples) {
    constexpr float c = 1.0f; 
    
    // Dynamic output scaler (e.g., 1/4 for 4 lines, 1/8 for 8 lines)
    float outputScaler = 1.0f / static_cast<float>(N);

    for (int n = 0; n < numSamples; ++n) {
        float x = data[n]; 
        
        // Reset the Y vector to zero for this sample's accumulation
        std::fill(Y.begin(), Y.end(), 0.0f);

        // Read the delayed signals
        for (int i = 0; i < N; ++i) {
            float rawDelay = delayLines[i].read();
            d_out[i] = dampingFilters[i].process(rawDelay) * gains[i]; 
        }

        // Perform the Matrix Multiplication + Input addition
        for (int row = 0; row < N; ++row) {
            for (int col = 0; col < N; ++col) {
                Y[row] += matrix[row][col] * d_out[col];
            }
            Y[row] += c * x; 
        }

        // The Transform (Dynamic Linear Combination)
        float v = 0.0f;
        for (int i = 0; i < N; ++i) {
            v += Y[i];
        }
        v *= outputScaler;

        // Write Y back into the delay lines 
        for (int i = 0; i < N; ++i) {
            delayLines[i].write(Y[i]);
            delayLines[i].advance();   
        }

        // Output the sample
        data[n] = v; 
    }
}

std::unique_ptr<Effect> FDNReverb::clone() const {
    return std::make_unique<FDNReverb>(*this); 
}