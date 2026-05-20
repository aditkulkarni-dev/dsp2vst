#pragma once
#include <vector>
#include "AudioBuffer_opt.h"

class AudioNode{
public:
    virtual ~AudioNode() = default;

    // Keeps track of effect nodes which provide input to current node
    std::vector<AudioNode*> inputs;

    // output buffer
    AudioBlock outputBuffer;

    // AudioNode must have a process function
    virtual void process() = 0;

    void addInput(AudioNode* inputNode){
        inputs.push_back(inputNode);
    }

};