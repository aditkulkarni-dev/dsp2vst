#include "audio/AudioEngine.h"
#include "effects/Gain.h"
#include "effects/ffDelay.h"
#include "effects/FDNReverb.h"

int main() {
    AudioEngine engine("input.wav", "output.wav", 1024);

    // Add ur effects here
    engine.addEffect(std::make_unique<Gain>(0.7f));
    engine.addEffect(std::make_unique<FDNReverb>(std::vector<int>{1103, 1321, 1487, 1699, 1777, 1987, 2113, 2399}));

    engine.run();
    return 0;
}