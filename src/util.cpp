//
//  util.cpp
//  mySketch
//
//  Created by Nevo Segal on 19/03/2014.
//
//

#include "util.h"



//--------------------------------------------------------------
float* util::normalizeComplement(float* arr, int size){
    float * outarr = normalize(arr, size);
    for (int i = 0; i < size; i++) {
        outarr[i] = complement(outarr[i]);
    }
    return outarr;
}

//--------------------------------------------------------------
float* util::normalize(float* arr, int size){
    float highestValue = 0;
    float* outarr = arr;
    for (int x = 0; x < size; x++) {
        if(outarr[x]>highestValue){
            highestValue = outarr[x];
        }
    }
    for (int x = 0; x < size; x++){
        if (highestValue > 0){
            outarr[x] = outarr[x]/highestValue;
        }
    }
    return outarr;
}

//--------------------------------------------------------------
float util::complement(float i){
    return 1.0f - i;
}