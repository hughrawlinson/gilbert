#pragma once

#include "ofMain.h"
#include "fft.h"
#include "ofxUI.h"
#include <vector>
#include "math.h"
#include "sndfile.h"
#include "sndfile.hh"
#include "util.h"
#include "gilbertAnalysis.h"


#define BUFFER_SIZE 256
#define NUM_WINDOWS 80

class gilbert : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    //UI
    void setGUI1();
    ofxUICanvas *gui1;
    void guiEvent(ofxUIEventArgs &e);

    void audioIn(float *input, int bufferSize, int nChannels);
    void analyseHitBuffer(vector<float>& hitBuffer, string drum);
    void calcRoomRMS(float currRMS);
    
    std::string lookupClosest(sfs input);
    
    int initialBufferSize;
    int sampleRate;
    float * buffer;
    float maxRoomRMS;
    
    bool aPressed, bPressed, cPressed, dPressed;
    std::vector<float> aBuffer = std::vector<float>(1,0);
    std::vector<float> bBuffer = std::vector<float>(1,0);
    std::vector<float> cBuffer = std::vector<float>(1,0);
    std::vector<float> dBuffer = std::vector<float>(1,0);
    
    ofSoundPlayer snare;
    ofSoundPlayer kick;
    ofSoundPlayer hat;
    ofSoundPlayer crash;
    
    bool audioFinished = false;
    bool hitDetected = false;
    
    std::vector<sfs> inputSfsSet;
    
    // FFT vars
    fft myfft;
    
    gilbertAnalysis analysis;
    
    float bufrms = 0;
    
    float * magnitude;
    float * phase;
    float * power;
    float avg_power;
    
    float freq[NUM_WINDOWS][BUFFER_SIZE/2];
    float freq_phase[NUM_WINDOWS][BUFFER_SIZE/2];
		
};
