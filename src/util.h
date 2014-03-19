//
//  util.h
//  mySketch
//
//  Created by Nevo Segal on 19/03/2014.
//
//



#ifndef __mySketch__util__
#define __mySketch__util__

#include <iostream>
#include "math.h"

class util {
    
public:
    static float* normalizeComplement(float* arr, int size);
    static float* normalize(float* arr, int size);
    static float complement(float i);
};
#endif /* defined(__mySketch__util__) */
