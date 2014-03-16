#include "gilbert.h"
#include "math.h"

//--------------------------------------------------------------
void gilbert::setup(){
    // 0 output channels,
	// 1 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 1 buffer
    initialBufferSize = 512;
	sampleRate = 44100;
    avg_power = 0.0f;
	ofSoundStreamSetup(0, 1, this, sampleRate, initialBufferSize, 1);
    
    buffer = new float[initialBufferSize];
	memset(buffer, 0, initialBufferSize * sizeof(float));
    magnitude = new float[BUFFER_SIZE];
	memset(magnitude, 0, initialBufferSize * sizeof(float));
    power = new float[BUFFER_SIZE];
	memset(power, 0, initialBufferSize * sizeof(float));
    phase = new float[BUFFER_SIZE];
	memset(phase, 0, initialBufferSize * sizeof(float));
    
    ofSetVerticalSync(true);
	ofEnableSmoothing();
    ofSetFrameRate(60);
    ofBackground(59,89,152);

    snare.loadSound("sounds/snare.wav");
    
    kick.loadSound("sounds/kick.wav");
    
    hat.loadSound("sounds/hat.wav");
    
    crash.loadSound("sounds/crash.wav");
    
    setGUI1();
    gui1->loadSettings("gui1Settings.xml");
    
    for (int i = 0; i < NUM_WINDOWS; i++)
	{
		for (int j = 0; j < BUFFER_SIZE/2; j++)
		{
			freq[i][j] = 0;
		}
	}
}

//--------------------------------------------------------------
void gilbert::update(){

}

//--------------------------------------------------------------
void gilbert::draw(){
    
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
    int currArrPos = 0;
    int minBufferSize = MIN(initialBufferSize, bufferSize);
	for(int i=0; i<minBufferSize; i++) {
		buffer[i] = input[i];
	}
    
    myfft.powerSpectrum(0, (int)BUFFER_SIZE/2, buffer, BUFFER_SIZE, magnitude, phase, power, &avg_power);
    
    //trigger based on centroid
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
    float centroid = 0;
    
    float sumMags = 0.1;
    float sumFreqByMags = 0;
    
    for(int i = 0; i < BUFFER_SIZE; i++){
        sumMags += power[i];
        sumFreqByMags += power[i]*((float)i*(44100.0f/(float)BUFFER_SIZE));
    }
    
    centroid = sumFreqByMags/sumMags;
    
    if(isnan(centroid) || isinf(centroid)){
        ofLog(OF_LOG_NOTICE,"returning 0");
        return 0.0f;
    }
    else{
        return centroid;
    }
}

//--------------------------------------------------------------
void gilbert::setGUI1(){
    gui1 = new ofxUISuperCanvas("");
    gui1->setDrawBack(false);
    
    gui1->addSpectrum("SPECTRUM", power, 256, 0, 1, 298, 100);
}

//--------------------------------------------------------------
void gilbert::guiEvent(ofxUIEventArgs &e){
    
}
