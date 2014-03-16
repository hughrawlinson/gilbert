#pragma once

#include "ofMain.h"
#include "fft.h"
#include "ofxUI.h"
#include <vector>

#define BUFFER_SIZE 512
#define NUM_WINDOWS 80

typedef struct soundFeatureSet{
    string id;
    float centroid;
    float rms;
} sfs;

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
    
    float calcRMS();
    float calcSC();
    
    string lookupClosest(sfs input);
    
    int initialBufferSize;
    int sampleRate;
    float * buffer;
    
    // FFT vars
    fft myfft;
    
    ofSoundPlayer snare;
    ofSoundPlayer kick;
    ofSoundPlayer hat;
    ofSoundPlayer crash;
    
    float bufrms = 0;
    
    float * magnitude;
    float * phase;
    float * power;
    float avg_power;
    
    float freq[NUM_WINDOWS][BUFFER_SIZE/2];
    float freq_phase[NUM_WINDOWS][BUFFER_SIZE/2];
		
};
