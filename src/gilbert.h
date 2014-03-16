#pragma once

#include "ofMain.h"
#include "fft.h"
#include "ofxUI.h"
#include <vector>

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
    void analyseHitBuffer(vector<float>& hitBuffer);
    float calcVectorSC(vector<float>& shortBuffer, int startPoint);
    float calcVectorRMS(const vector<float>& shortBuffer, int startPoint, int endPoint);
    
    int initialBufferSize;
    int sampleRate;
    float * buffer;
    float maxRoomRMS;
    
    float calcRMS();
    float calcSC();
    void calcRoomRMS(float currRMS);
    Boolean aPressed, bPressed, cPressed, dPressed;
    std::vector<float> aBuffer, bBuffer, cBuffer, dBuffer;
    
    // FFT vars
    fft myfft;
    
    ofSoundPlayer snare;
    ofSoundPlayer kick;
    
    float magnitude[BUFFER_SIZE];
    float phase[BUFFER_SIZE];
    float power[BUFFER_SIZE];
    
    float freq[NUM_WINDOWS][BUFFER_SIZE/2];
    float freq_phase[NUM_WINDOWS][BUFFER_SIZE/2];
		
};
