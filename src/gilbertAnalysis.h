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

typedef struct soundFeatureSet{
    string id;
    float centroid;
    float rms;
} sfs;

class gilbertAnalysis {
    fft myfft;
    
public:
    float calcVectorSC(std::vector<float>& shortBuffer, int startPoint, int bsize);
    float calcVectorRMS(const std::vector<float>& exactHit, int startPoint, int endPoint);
    float calcRMS(float* b, int size);
    float calcSC(float *b, int size);
    sfs analyseHitBuffer(std::vector<float>& hitBuffer, std::string drum, float ambientRMS);
};

#endif /* defined(__mySketch__gilbertAnalysis__) */