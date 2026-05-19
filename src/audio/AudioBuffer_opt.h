#include <iostream>

class AudioBuffer{
public:
    AudioBuffer(int numSamples, int numChannels):numSamples(numSamples), numChannels(numChannels)
    {
        channels = new float*[numChannels];
        for(int ch{0}; ch < numChannels; ch++){
            channels[ch] = new float[numSamples];
            for(int sample{0}; sample < numSamples; sample++){
                channels[ch][sample] = 0.0f;
            }
        }
    }
    // Disable Copy Constructor
    AudioBuffer(const AudioBuffer& other) = delete;

    // Disable Copy Assignment Operator 
    AudioBuffer& operator=(const AudioBuffer& other) = delete;

    ~AudioBuffer(){
        for(int ch{0}; ch < numChannels; ch++){
            
            delete[] channels[ch];
        }
        delete[] channels;
    }

    const float* getReadPtr(int channelNum) const{
        return channels[channelNum];
    }

    float* getWritePtr(int channelNum){
        return channels[channelNum];
    }

private:
    float** channels = nullptr;
    int numSamples = 0;
    int numChannels = 0;
};