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
    float highestvalue = 0;
    float* outarr = arr;
    for (int x = 0; x < size; x++) {
        if(outarr[x]>highestvalue){
            highestvalue = outarr[x];
        }
    }
    for (int x = 0; x < size; x++){
        outarr[x] = 1-(outarr[x]/highestvalue);
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