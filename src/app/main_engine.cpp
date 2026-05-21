#include "audio/AudioEngine.h"
#include "effects/Gain.h"
#include "effects/ffDelay.h"
#include "effects/FDNReverb.h"

int main() {
    AudioEngine engine("input.wav", "output.wav");

    // Add ur effects here
    engine.addEffect(std::make_unique<Gain>(0.7f));
    engine.addEffect(std::make_unique<FDNReverb>(std::vector<int>{1103, 1487, 1777, 2113}));

    engine.run();
    return 0;
}