#include "gilbert.h"
#include "math.h"

//--------------------------------------------------------------
void gilbert::setup(){
    
    ofSetVerticalSync(true);
	ofEnableSmoothing();
    ofSetFrameRate(60);
    ofBackground(59,89,152);
    
	initialBufferSize = 512;
	sampleRate = 44100;
    maxRoomRMS = 0;
    snare.loadSound("sounds/snare.wav");
    kick.loadSound("sounds/kick.wav");
    
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
    
    ofLog(OF_LOG_NOTICE, "max room rms : %f", maxRoomRMS);
    
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
    
    gui1->addSpectrum("SPECTRUM", power, 256, 0, 6, 298, 100);
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



