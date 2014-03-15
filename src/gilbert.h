#pragma once

#include "ofMain.h"
#include "fft.h"
#include "ofxUI.h"

#define BUFFER_SIZE 512
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

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void audioIn(float *input, int bufferSize, int nChannels);
    float* normalizeComplement(float* arr, int size);
    float* normalize(float* arr, int size);
    
    int initialBufferSize;
    int sampleRate;
    float * buffer;
    
    float calcRMS();
    float calcSC();
    
    // FFT vars
    fft myfft;
    
    ofSoundPlayer snare;
    ofSoundPlayer kick;
    
    float * magnitude = new float[BUFFER_SIZE];
    float * phase = new float[BUFFER_SIZE];
    float * power = new float[BUFFER_SIZE];
    float avg_power;
    
    float *kispec;
    float *snspec;
    
    float freq[NUM_WINDOWS][BUFFER_SIZE/2];
    float freq_phase[NUM_WINDOWS][BUFFER_SIZE/2];
		
};
