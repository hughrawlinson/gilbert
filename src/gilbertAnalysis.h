//
//  gilbertAnalysis.h
//  mySketch
//
//  Created by Nevo Segal on 19/03/2014.
//
//

#ifndef __mySketch__gilbertAnalysis__
#define __mySketch__gilbertAnalysis__

#include <iostream>
#include <vector>
#include <math.h>
#include "fft.h"
#include "util.h"
#include "sndfile.h"

typedef struct soundFeatureSet{
    std::string id;
    float centroid;
    float rms;
} sfs;

class gilbertAnalysis {
    fft myfft;
    float* lastMags;
    
public:
    gilbertAnalysis();
    float calcRMS(float* b, int size);
    float calcRMS(std::vector<float>& exactHit);
    float calcSC(float *b, int size);
    float calcSC(std::vector<float>& shortBuffer);
    float calcSF(float *magns, int size);
    sfs analyseHitBuffer(std::vector<float>& hitBuffer, std::string drum, float ambientRMS);
    void writeWAV(std::vector<float>& buffer, int bufferSize, std::string drum, sfs info);
    
};

#endif /* defined(__mySketch__gilbertAnalysis__) */
