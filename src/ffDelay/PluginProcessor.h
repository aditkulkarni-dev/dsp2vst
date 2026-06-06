/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <atomic>
#include <memory>
#include <stdexcept>
#include <vector>


// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\effects\Effect.h =====


// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\audio\AudioBuffer_opt.h =====


class AudioBuffer{
public:
    AudioBuffer(int numSamples, int numChannels, int sampleRate):numSamples(numSamples), numChannels(numChannels), sampleRate(sampleRate)
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
    int getNumChannels() const { return numChannels; }
    int getNumSamples() const { return numSamples; }
    int getSampleRate() const { return sampleRate; }

private:
    float** channels = nullptr;
    int numSamples = 0;
    int numChannels = 0;
    int sampleRate = 0;
};

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\audio\AudioBuffer_opt.h =====


class Effect{
public:
    virtual ~Effect() = default;
    // Every effect must have a process
    // An effect must not own the memory. Only the engine owns memory
    virtual void process(float* data, int numSamples) = 0;

    virtual std::unique_ptr<Effect> clone() const = 0;
    
};

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\effects\Effect.h =====


// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\audio\AudioParameter.h =====



template <typename Effect>
struct AudioParameter{
    std::string name;
    std::string id;
    float min;
    float max;
    float step;
    float default_value;
    void (Effect::*setParameter)(float); 
};

/*
    void (effect::*setParameter)(float) : pointer to a void function called setParameter which is 
    a member function of effect class, and takes in a float value which sets the parameter's value.
*/

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\audio\AudioParameter.h =====


// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\CircularBuffer.h =====


class CircularBuffer {
public:
    CircularBuffer() = default;
    
    void reset();
    void setSize(int newSize);
    int getSize() const;
    
    // Reads the oldest sample in the buffer (max delay)
    float read() const; 
    
    // Reads a specific amount of samples into the past
    float read(int delay) const; 
    
    // Writes a sample to the current index (does NOT move the index)
    void write(float sample); 
    
    // Moves the index forward and wraps around
    void advance(); 
    
    int getWriteIdx() const;

private:
    std::vector<float> buffer;
    int writeIdx{0};
    int size{0}; // Cached size for faster wrap-around checking
};

// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\CircularBuffer.cpp =====


void CircularBuffer::reset() {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writeIdx = 0;
}

void CircularBuffer::setSize(int newSize) {
    if (newSize <= 0) {
        throw std::invalid_argument("Buffer size must be greater than 0");
    }
    
    size = newSize;
    buffer.resize(size);
    
    // reset() handles clearing the memory to 0.0f and resetting the index
    reset(); 
}

int CircularBuffer::getSize() const {
    return size;
}

int CircularBuffer::getWriteIdx() const {
    return writeIdx;
}

void CircularBuffer::write(float sample) {
    // Just overwrite the current index. Advance is handled separately.
    buffer[writeIdx] = sample;
}

void CircularBuffer::advance() {
    writeIdx++;
    
    // Use an 'if' branch instead of '%' for massive DSP performance gains
    if (writeIdx >= size) {
        writeIdx = 0;
    }
}

float CircularBuffer::read() const {
    // Reading without a delay parameter automatically grabs the oldest sample
    return buffer[writeIdx];
}

float CircularBuffer::read(int delay) const {
    // Allow delay to be 0 (reading the current writeIdx right before writing)
    if (delay < 0 || delay > size) {
        throw std::invalid_argument("Invalid delay: must be between 0 and buffer size");
    }

    int idxToRead = writeIdx - delay;
    
    // Wrap around backwards if we drop below 0
    if (idxToRead < 0) {
        idxToRead += size; 
    }
    
    return buffer[idxToRead];
}

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\CircularBuffer.cpp =====


// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\CircularBuffer.h =====


class ffDelay : public Effect{
public:
    ffDelay(int delay=128, float mix = 0.3f, int bufferSize=512) : delay(static_cast<float>(delay)), mix(mix){
        buffer.setSize(bufferSize);
    }
    void process(float* data, int numSamples) override;
    void setBufferSize(int bufferSize);
    std::unique_ptr<Effect> clone() const override;
    std::vector<AudioParameter<ffDelay>> static const getAudioParameters();
    void setDelay(float newDelay);
    void setMix(float newMix);

private:
    float delay{0};
    float mix{0.3f};
    CircularBuffer buffer;
    inline static const std::vector<AudioParameter<ffDelay>>
     params = {{"delay", "delay", 0, 256, 1, 128, &ffDelay::setDelay}, 
                {"mix", "mix", 0.0f, 1.0f, 0.01f, 0.5f, &ffDelay::setMix} };

};

// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\effects\ffDelay.cpp =====


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

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\effects\ffDelay.cpp =====


//==============================================================================
/**
*/
class ffDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ffDelayAudioProcessor();
    ~ffDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    std::vector<AudioParameter<ffDelay>> audioParams;
    std::vector<ffDelay> userEffects;
    std::vector<std::atomic<float>*> paramPtrs;
    juce::AudioProcessorValueTreeState apvts; // should be last


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ffDelayAudioProcessor)
    
};
