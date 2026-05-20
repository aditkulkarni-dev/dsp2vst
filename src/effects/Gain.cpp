#include "Gain.h"
#include "../audio/AudioBuffer_opt.h"
#include <memory>

Gain::Gain(float gainAmount) : gain(gainAmount) {}

void Gain::process(float* data, int numSamples){
    for (int i{0}; i < numSamples; ++i){
        data[i] *= gain;
    }
}

void Gain::setGain(float newGain){
    gain = newGain;
}

float Gain::getGain() const{
    return gain;
}

std::unique_ptr<Effect> Gain::clone() const{
    return std::make_unique<Gain>(*this); 
}