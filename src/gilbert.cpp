#include "gilbert.h"
#include "math.h"
#include "sndfile.h"
#include "sndfile.hh"

//--------------------------------------------------------------
float* gilbert::normalizeComplement(float* arr, int size){
    float highestvalue = 0;
    float* outarr = arr;
    for (int x = 0; x < size; x++) {
        if(arr[x]>highestvalue){
            highestvalue = arr[x];
        }
    }
    for (int x = 0; x < size; x++){
        arr[x] = 1-(arr[x]/highestvalue);
    }
    return outarr;
}

//--------------------------------------------------------------
float* gilbert::normalize(float* arr, int size){
    float highestvalue = 0;
    float* outarr = arr;
    for (int x = 0; x < size; x++) {
        if(arr[x]>highestvalue){
            highestvalue = arr[x];
        }
    }
    for (int x = 0; x < size; x++){
        arr[x] = arr[x]/highestvalue;
    }
    return outarr;
}

//--------------------------------------------------------------
void gilbert::setup(){
    
    ofSetVerticalSync(true);
	ofEnableSmoothing();
    ofSetFrameRate(60);
    ofBackground(59,89,152);
    
	initialBufferSize = 512;
	sampleRate = 44100;
    avg_power = 0.0f;
    
    string __dir = ofDirectory().getAbsolutePath();
    
    SndfileHandle sn = SndfileHandle(__dir+"sounds/snare.wav");
    SndfileHandle ki = SndfileHandle(__dir+"sounds/kick.wav");
    
    maxRoomRMS = 0;
    snare.loadSound("sounds/snare.wav");
    float snbu [sn.frames()];
    sn.read(snbu, sn.frames());
    float pow [sn.frames()];
    float magn [sn.frames()];
    float phase [sn.frames()];
    float avg_pow;
    myfft.powerSpectrum(0, 4096, snbu, 8192, magn, phase, pow, &avg_pow);
    snspec = normalizeComplement(pow,sn.frames());
    
    kick.loadSound("sounds/kick.wav");
    float kibu [ki.frames()];
    ki.read (kibu, ki.frames());
    float pow2 [sn.frames()];
    myfft.powerSpectrum(0, 4096, kibu, 8192, magn, phase, pow2, &avg_pow);
    normalizeComplement(pow2,ki.frames());
    kispec = pow2;
    
    setGUI1();
    gui1->loadSettings("gui1Settings.xml");
	
	buffer = new float[initialBufferSize];
	memset(buffer, 0, initialBufferSize * sizeof(float));
    
	// 0 output channels,
	// 1 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 1 buffer
	ofSoundStreamSetup(0, 1, this, sampleRate, initialBufferSize, 1);
    
    for (int i = 0; i < NUM_WINDOWS; i++)
	{
		for (int j = 0; j < BUFFER_SIZE/2; j++)
		{
			freq[i][j] = 0;
		}
	}
    

    
    buffer = new float[initialBufferSize];
	memset(buffer, 0, initialBufferSize * sizeof(float));
    
    myfft.powerSpectrum(0, (int)BUFFER_SIZE/2, buffer, BUFFER_SIZE, &magnitude[0], &phase[0], &power[0], &avg_power);
    power = normalize(power, BUFFER_SIZE);
}

//--------------------------------------------------------------
void gilbert::update(){

}

//--------------------------------------------------------------
void gilbert::draw(){
    float avg_power = 0.0f;
    myfft.powerSpectrum(0, (int)BUFFER_SIZE/2, buffer, BUFFER_SIZE, &magnitude[0], &phase[0], &power[0], &avg_power);

    
    while(ofGetElapsedTimeMillis() < 1000){
        calcRoomRMS(calcRMS());
    }
    
    ofPushStyle();
    ofSetColor(255);
    ofSetLineWidth(1);
    
    float y1 = ofGetHeight() * 0.5;
    ofLine(0, y1, ofGetWidth(), y1);
    
	for(int i=0; i<initialBufferSize; i++){
        float p = i / (float)(initialBufferSize-1);
        float x = p * ofGetWidth();
        float y2 = y1 + buffer[i] * 200;
        
		ofLine(x, y1, x, y2);
	}
    ofPopStyle();
    
    if(calcRMS()>maxRoomRMS){
        ofLog(OF_LOG_NOTICE,"SC: " + ofToString(calcSC()));
        if(calcSC()>1000){
            snare.setVolume(calcRMS()*2);
            snare.play();
        }
        else{
            kick.setVolume(calcRMS()*2);
            kick.play();
        }
    }
    
    ofPushStyle();
	ofSetColor(255);
    ofDrawBitmapString("SC: " + ofToString(calcSC()),20, ofGetHeight()-60);
    ofDrawBitmapString("AP: " + ofToString(avg_power),20, ofGetHeight()-40);
    ofDrawBitmapString("RMS: " + ofToString(calcRMS()), 20, ofGetHeight()-20);
    ofPopStyle();
}

void gilbert::exit(){
    gui1->saveSettings("gui1Settings.xml");
    delete gui1;
}

//--------------------------------------------------------------
void gilbert::keyPressed(int key){
    ofLog(OF_LOG_NOTICE, "key: %d",key);
}

//--------------------------------------------------------------
void gilbert::keyReleased(int key){

}

//--------------------------------------------------------------
void gilbert::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void gilbert::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void gilbert::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void gilbert::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void gilbert::windowResized(int w, int h){

}

//--------------------------------------------------------------
void gilbert::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void gilbert::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void gilbert::audioIn(float *input, int bufferSize, int nChannels){
    int minBufferSize = MIN(initialBufferSize, bufferSize);
	for(int i=0; i<minBufferSize; i++) {
		buffer[i] = input[i];
	}
    myfft.powerSpectrum(0, (int)BUFFER_SIZE/2, buffer, BUFFER_SIZE, &magnitude[0], &phase[0], &power[0], &avg_power);
    power = normalize(power, BUFFER_SIZE);
    if(kick.getIsPlaying()){
        for(int x = 0; x < BUFFER_SIZE; x++){
            power[x] = power[x]*kispec[x];
        }
    }
    if(snare.getIsPlaying()){
        for(int x = 0; x < BUFFER_SIZE; x++){
            power[x] = power[x]*snspec[x];
        }
    }
}

//--------------------------------------------------------------
float gilbert::calcRMS(){
    float count = 0;
    for(int i=0; i<initialBufferSize; i++){
        count += pow(buffer[i],2);
    }
    count = count/initialBufferSize;
    return sqrt(count);
}

//--------------------------------------------------------------
float gilbert::calcSC(){
    float centroid;
    
    float sumMags;
    float sumFreqByMags;
    
    for(int i = 0; i< BUFFER_SIZE; i++){
        sumMags += magnitude[i];
        sumFreqByMags += magnitude[i]*(i*44100/BUFFER_SIZE);
    }
    
    centroid = sumFreqByMags/sumMags;
    
    return centroid;
}

//--------------------------------------------------------------
void gilbert::setGUI1(){
    gui1 = new ofxUISuperCanvas("");
    gui1->setDrawBack(false);
    
    gui1->addSpectrum("SPECTRUM", power, 256, 0, 3, 298, 100);
    gui1->addSpectrum("snspec", snspec, 256, 0, 1.0, 298, 100);
    gui1->addSpectrum("kispec", kispec, 256, 0, 1.0, 298, 100);
    gui1->addLabel("SAMPLES", OFX_UI_FONT_SMALL);
    gui1->setGlobalButtonDimension(50);
    gui1->addToggleMatrix("1X4 MATRIX", 1, 4, 73, 50);
    gui1->autoSizeToFitWidgets();
    ofAddListener(gui1->newGUIEvent,this,&gilbert::guiEvent);
}

//--------------------------------------------------------------
void gilbert::guiEvent(ofxUIEventArgs &e){
    ofLog(OF_LOG_NOTICE, "Thanks!");
}

//--------------------------------------------------------------
void gilbert::calcRoomRMS(float currRMS){
    if(currRMS > maxRoomRMS){
        maxRoomRMS = currRMS;
    }
}



