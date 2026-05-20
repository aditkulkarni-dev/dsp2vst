#include "Effect.h"
#include <iostream>
#include <array>
#include "../dsp/CircularBuffer.h"
#include <vector>

class FDNReverb : Effect{
    public:
    FDNReverb(std::vector<float> k) : k(k)
    
    {
        delayLines.resize(k.size());
        for(int i{0}; i < k.size(); i++){
            delayLines[i].setSize(k[i]);
        }
    }

    // data can be indexed like an array.

    /*
        we would need to do something like this.
        Y[n] = matrix * < D1(n-k1), D2(n-k2), D3(n-k3), D4(n-k4)> 
        + <c1, c2, c3, c4> * <x[n], x[n], x[n], x[n] >

        Y would be a vector of 4 elements.
        For our output sample v, it has to be some transform of our Y.
        A simple transform could be the average of the Y.
        So 1/4 (y[0] + y[1] + y[2] + y[3])

        It doesn't HAVE to be linear, but if we are modelling an actual reverb,
        it is LTI, so a linear transform makes the most sense. 

        We could potentially use the euclidean distance and see if that transform
        makes the sound more interesting, but it is a node to revisit ahead.

    */

    void process(float* data, int numSamples);
    std::unique_ptr<Effect> clone() const;

    private:
    std::vector<float> k;
    std::vector<CircularBuffer> delayLines;

    // As of now, we hard code it to 4. We will change the part below later on.
    
    static constexpr float s = 0.5f;

    std::array<std::array<float, 4>, 4> matrix = {{
        { s,  s,  s,  s},
        { s, -s,  s, -s},
        { s,  s, -s, -s},
        { s, -s, -s,  s}
    }};

};