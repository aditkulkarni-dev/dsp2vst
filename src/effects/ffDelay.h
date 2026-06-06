#include "Effect.h"
#include "../audio/AudioBuffer_opt.h"
#include "../audio/AudioParameter.h"
#include "../dsp/CircularBuffer.h"
#include <memory>
#include <atomic>

class ffDelay : public Effect{
public:
    ffDelay(int delay=22050, float mix = 0.3f, int bufferSize=44100) : delay(static_cast<float>(delay)), mix(mix){
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
