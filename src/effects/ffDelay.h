#include "Effect.h"
#include "../audio/AudioBuffer_opt.h"
#include "../dsp/CircularBuffer.h"
#include <memory>

class ffDelay : public Effect{
public:
    ffDelay() = default;
    ffDelay(int delay, float mix, int bufferSize=256) : delay(delay), mix(mix){
        buffer.setSize(bufferSize);
    }
    void process(float* data, int numSamples) override;
    void setBufferSize(int bufferSize);
    std::unique_ptr<Effect> clone() const override;

private:
    int delay{0};
    float mix{0.3f};
    CircularBuffer buffer;


};