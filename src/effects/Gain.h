#include "Effect.h"
#include "../audio/AudioBuffer_opt.h"
#include "../audio/AudioParameter.h"
#include <memory>
#include <vector>

class Gain : public Effect{
public:
    Gain() = default;
    explicit Gain(float gainAmount);

    void process(float* data, int numSamples) override;

    void setGain(float newGain);
    float getGain() const;
    std::unique_ptr<Effect> clone() const override;
    
    inline static const std::vector<AudioParameter<Gain>> getAudioParameters();

private:
    float gain = 1.0f;

};