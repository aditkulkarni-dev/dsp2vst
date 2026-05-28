#include "audio/AudioEngine.h"
#include "effects/Gain.h"
#include "effects/ffDelay.h"
#include "effects/FDNReverb.h"

int main() {
    AudioEngine engine("C:/Users/Aditya Kulkarni/audio-engine/audio-engine/input.wav", "output.wav", 1024);

    // Add ur effects here
    engine.addEffect(std::make_unique<Gain>(0.7f));   
    engine.addEffect(std::make_unique<ffDelay>(22000, 1.0f, 44100));
    engine.run();
    return 0;
}