#include "gilbert.h"

//--------------------------------------------------------------
void gilbert::setup(){
    // 0 output channels,
	// 1 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 1 buffer
    initialBufferSize = 256;
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
    
    maxRoomRMS = 0;
    
    snare.loadSound("sounds/snare.wav");
    snare.setMultiPlay(true);
    kick.loadSound("sounds/kick.wav");
    kick.setMultiPlay(true);
    hat.loadSound("sounds/hat.wav");
    hat.setMultiPlay(true);
    crash.loadSound("sounds/crash.wav");
    crash.setMultiPlay(true);
    
    setGUI1();
    gui1->loadSettings("gui1Settings.xml");
    
    for (int i = 0; i < NUM_WINDOWS; i++)
	{
		for (int j = 0; j < BUFFER_SIZE/2; j++)
		{
			freq[i][j] = 0;
		}
	}
    audioFinished = true;
}

//--------------------------------------------------------------
void gilbert::update(){

}

//--------------------------------------------------------------
void gilbert::draw(){
    if(!audioFinished){
        return;
    }
    
    if(ofGetElapsedTimeMillis() < 1000){
        calcRoomRMS(analysis.calcRMS(buffer,BUFFER_SIZE));
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
    
    ofPushStyle();
	ofSetColor(255);
    ofDrawBitmapString("SF: " + ofToString(analysis.calcSF(power,256)/6500.0f),20, ofGetHeight()-80);
    ofDrawBitmapString("SC: " + ofToString(analysis.calcSC(magnitude,256)),20, ofGetHeight()-60);
    ofDrawBitmapString("AP: " + ofToString(avg_power),20, ofGetHeight()-40);
    ofDrawBitmapString("RMS: " + ofToString(analysis.calcRMS(buffer, BUFFER_SIZE)), 20, ofGetHeight()-20);
    ofPopStyle();
}

//--------------------------------------------------------------
void gilbert::exit(){
    gui1->saveSettings("gui1Settings.xml");
    delete gui1;
}

//--------------------------------------------------------------
void gilbert::audioIn(float *input, int bufferSize, int nChannels){
    int currArrPos = 0;
    int minBufferSize = MIN(initialBufferSize, bufferSize);
	for(int i=0; i<minBufferSize; i++) {
		buffer[i] = input[i];
	}
    
    if(aPressed){
        for(int i=0; i<minBufferSize; i++){
            aBuffer.push_back(input[i]);
            if(aBuffer.size() >= 88200){
                aPressed=false;
                std::vector<float> aExactHit = analysis.getExactHit(aBuffer,maxRoomRMS);
                sfs info = analysis.analyseHitBuffer(aExactHit, "a");
                inputSfsSet.push_back(info);
                analysis.writeWAV(aExactHit, aExactHit.size(), "a", info);
                break;
            }
        }
    }
    else if(bPressed){
        for(int i=0; i<minBufferSize; i++){
            bBuffer.push_back(input[i]);
            if(bBuffer.size() >= 88200){
                bPressed=false;
                std::vector<float> bExactHit = analysis.getExactHit(aBuffer,maxRoomRMS);
                sfs info = analysis.analyseHitBuffer(bExactHit, "b");
                inputSfsSet.push_back(info);
                analysis.writeWAV(bExactHit, bExactHit.size(), "b", info);
                break;
            }
        }
    }
    else if(cPressed){
        for(int i=0; i<minBufferSize; i++){
            cBuffer.push_back(input[i]);
            if(cBuffer.size() >= 88200){
                cPressed=false;
                std::vector<float> cExactHit = analysis.getExactHit(aBuffer,maxRoomRMS);
                sfs info = analysis.analyseHitBuffer(cExactHit, "c");
                inputSfsSet.push_back(info);
                analysis.writeWAV(cExactHit, cExactHit.size(), "c", info);
                break;
            }
        }
    }
    else if(dPressed){
        for(int i=0; i<minBufferSize; i++){
            dBuffer.push_back(input[i]);
            if(dBuffer.size() >= 88200){
                dPressed=false;
                std::vector<float> dExactHit = analysis.getExactHit(aBuffer,maxRoomRMS);
                sfs info = analysis.analyseHitBuffer(dExactHit, "d");
                inputSfsSet.push_back(info);
                analysis.writeWAV(dExactHit, dExactHit.size(), "d", info);
                break;
            }
        }
    }
    
    myfft.powerSpectrum(0, (int)BUFFER_SIZE/2, buffer, BUFFER_SIZE, magnitude, phase, power, &avg_power);
    
    if(analysis.calcRMS(buffer, BUFFER_SIZE) >= maxRoomRMS && inputSfsSet.size()>0){
        std::vector<float> liveHitBuffer = std::vector<float>(1,0);
        bool hitDetected = true;
        if(hitDetected){
            for (int i = 0 ; i<minBufferSize; i++) {
                liveHitBuffer.push_back(buffer[i]);
            }
            if (liveHitBuffer.size() >= 2048){
                analysis.analyseHitBuffer(liveHitBuffer, "static");
                hitDetected = false;
            }
        }
        
//        float * rmsInEachBin;
//        int highestBinIndex = 0;
//        float highestBin = 0;
//        rmsInEachBin = new float[BUFFER_SIZE/8];
//        
//        for(int i = 0; i<BUFFER_SIZE-8; i+=8){
//            rmsInEachBin[i/8] = analysis.calcRMS(buffer+i, 8);
//            if(rmsInEachBin[i/8] > highestBin){
//                highestBin = rmsInEachBin[i/8];
//                highestBinIndex = i;
//            }
//
//        }
        
        
//        ofLog(OF_LOG_NOTICE, "Highest Bin: %d", highestBinIndex);
//        ofLog(OF_LOG_NOTICE, "Highest Value: %f", highestBin);


        // get start time -> how
        // aside: how = run rms on like 16 samples
        // if start time > length - sample length
            // save zi = buffer[length-samplelength:length]
            // set flag
            // return
        // else
            // zi = start:start+samplelength
        //if flag
            // grab samples and append to z
            // calc sc on z
            // run distance
        sfs input1 = {.id="static",.centroid=analysis.calcSC(power,256),.rms=analysis.calcRMS(buffer, BUFFER_SIZE)};
        string soundid = lookupClosest(input1);
        if(soundid=="a" && !kick.getIsPlaying()){
            kick.play();
        }
        if(soundid=="b" && !snare.getIsPlaying()){
            snare.play();
        }
        if(soundid=="c" && !hat.getIsPlaying()){
            hat.play();
        }
        if(soundid=="d" && !crash.getIsPlaying()){
            crash.play();
        }
    }
    audioFinished = true;
}

//--------------------------------------------------------------
string gilbert::lookupClosest(sfs input){
    std::cout << input.id + ", " + ofToString(input.centroid) + ", " + ofToString(input.rms) << std::endl;
    sfs closest;
    float dist = 0;
    float closestDist=1000000000.0f;
    for(std::vector<int>::size_type i = 0; i != inputSfsSet.size(); i++) {
         dist= sqrt(pow(inputSfsSet[i].rms-input.rms,2)+pow(inputSfsSet[i].centroid-input.centroid,2));
        if(dist<closestDist){
            closestDist=dist;
            closest = inputSfsSet[i];
        }
    }
    std::cout << closest.id + ", " + ofToString(closest.centroid) + ", " + ofToString(closest.rms) << std::endl;
    if(closestDist<2){
        return closest.id;
    }
    else{
        return "";
    }
}

//--------------------------------------------------------------
void gilbert::setGUI1(){
    gui1 = new ofxUISuperCanvas("");
    gui1->setDrawBack(false);
    
    gui1->addSpectrum("SPECTRUM", power, 256, 0, 1, 298, 100);
    gui1->addLabel("SAMPLES", OFX_UI_FONT_SMALL);
    gui1->setGlobalButtonDimension(50);
    gui1->addToggleMatrix("1X4 MATRIX", 1, 4, 73, 73);
    gui1->autoSizeToFitWidgets();
    ofAddListener(gui1->newGUIEvent,this,&gilbert::guiEvent);
}

//--------------------------------------------------------------
void gilbert::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(name == "1X4 MATRIX(0,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        aPressed = toggle->getValue()==1;
    }
    else if (name == "1X4 MATRIX(1,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bPressed = toggle->getValue()==1;
        
    }
    else if (name == "1X4 MATRIX(2,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cPressed = toggle->getValue()==1;
    }
    else if (name == "1X4 MATRIX(3,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        dPressed = toggle->getValue()==1;
    }
}

//--------------------------------------------------------------
void gilbert::calcRoomRMS(float currRMS){
    if(currRMS > maxRoomRMS){
        maxRoomRMS = currRMS;
    }
}
