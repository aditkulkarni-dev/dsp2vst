#include "ffDelay.h"
#include "../audio/AudioBuffer_opt.h"

void ffDelay::setBufferSize(int bufferSize){
    buffer.setSize(bufferSize);
}

void ffDelay::process(float* data, int numSamples){
    
    for (int i{0}; i < numSamples; ++i){
        float input = data[i];
        
        //Read the old sample from 'delay' samples ago
        float delayed = buffer.read(delay);
 
        //Mix the dry input with the wet delayed signal
        float output = input + mix * delayed;
        
        //Store the current input sample into the buffer
        buffer.write(input);
        
        //Update the audio block array
        data[i] = output;
        
        //Step the buffer pointer forward for the next iteration!
        buffer.advance(); 
    }
}

std::unique_ptr<Effect> ffDelay::clone() const{
    return std::make_unique<ffDelay>(*this); 
}

std::vector<AudioParameter<ffDelay>> const ffDelay::getAudioParameters()
{
    return params;
}

void ffDelay::setDelay(float newDelay)
{
    delay = static_cast<int>(newDelay);
}

void ffDelay::setMix(float newMix)
{
    mix = newMix;
}
