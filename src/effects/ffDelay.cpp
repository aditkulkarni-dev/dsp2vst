#include "ffDelay.h"
#include "audio/AudioBuffer_opt.h"

void ffDelay::setBufferSize(int bufferSize){
    buffer.setSize(bufferSize);
}

void ffDelay::process(float* data, int numSamples){
    
    
    for (int i{0}; i < numSamples; ++i){
        float input = data[i];
        
        
        float delayed = buffer.read(delay);
 
        
        float output = input + mix * delayed;
        buffer.write(input);
        data[i] = output;
        
    }
}

std::unique_ptr<Effect> ffDelay::clone() const{
    return std::make_unique<ffDelay>(*this); 
}