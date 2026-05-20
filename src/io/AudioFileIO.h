#pragma once
#include <string>
#include "../audio/AudioBuffer_opt.h"
#include <vector>

class AudioFileIO{

public:
    std::unique_ptr<AudioBuffer> readWav(const std::string& filePath);
    void writeWav(const std::string& filePath, const AudioBuffer& buffer);
    
};