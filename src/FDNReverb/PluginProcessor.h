/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#pragma once // Good practice to prevent double inclusions

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


// ===== BEGIN C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\LowPassFilter.h =====


class LowPassFilter {
public:
    LowPassFilter() = default;

    // Resets the filter's memory
    void reset();

    // Sets the coefficient directly (0.0 to 1.0)
    void setCoefficient(float c);

    // Processes a single sample
    float process(float input);

private:
    float c{1.0f};  // Cutoff coefficient
    float z1{0.0f}; // One-sample memory (z^-1)
};

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\LowPassFilter.h =====


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

// ===== END C:\Users\Aditya Kulkarni\audio-engine\audio-engine\src\dsp\CircularBuffer.h =====


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

//==============================================================================
/**
*/
class FDNReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FDNReverbAudioProcessor();
    ~FDNReverbAudioProcessor() override;

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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNReverbAudioProcessor)
};
