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
    
    
    buffer = new float[initialBufferSize];
	memset(buffer, 0, initialBufferSize * sizeof(float));
    
    setGUI1();
    gui1->loadSettings("gui1Settings.xml");
	   
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
    int currArrPos = 0;
    int minBufferSize = MIN(initialBufferSize, bufferSize);
	for(int i=0; i<minBufferSize; i++) {
		buffer[i] = input[i];
	}
    
    if(aPressed){
        for(int i=0; i<minBufferSize; i++){
            aBuffer.push_back(buffer[i]);
            if(aBuffer.size() >= 88200){
                analyseHitBuffer(aBuffer);
                aPressed=false;
                break;
            }
        }
    }
    else if(bPressed){
        for(int i=0; i<minBufferSize; i++){
            bBuffer.push_back(buffer[i]);
            if(bBuffer.size() >= 88200){
                bPressed=false;
                break;
            }
        }
    }
    else if(cPressed){
        for(int i=0; i<minBufferSize; i++){
            cBuffer.push_back(buffer[i]);
            if(cBuffer.size() >= 88200){
                cPressed=false;
                break;
            }
        }
    }
    else if(dPressed){
        for(int i=0; i<minBufferSize; i++){
            dBuffer.push_back(buffer[i]);
            if(dBuffer.size() >= 88200){
                dPressed=false;
                break;
            }
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
    
    gui1->addSpectrum("SPECTRUM", power, 256, 0, 6, 298, 100);
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

    
    if(name == "1X4 MATRIX(0,0)")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue() == 1){
            aPressed = true;
            
        }
        else{
            aPressed = false;
        }
    }
    else if (name == "1X4 MATRIX(1,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue() == 1){
            bPressed = true;
        }
        else{
            bPressed = false;
        }
    }
    else if (name == "1X4 MATRIX(2,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue() == 1){
            cPressed = true;
        }
        else{
            cPressed = false;
        }
    }
    else if (name == "1X4 MATRIX(3,0)"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue() == 1){
            dPressed = true;
        }
        else{
            dPressed = false;
        }
    }
}

//--------------------------------------------------------------
void gilbert::calcRoomRMS(float currRMS){
    if(currRMS > maxRoomRMS){
        maxRoomRMS = currRMS;
    }
}

//--------------------------------------------------------------

void gilbert::analyseHitBuffer(vector<float>& hitBuffer){
    
    //array to store rms in each bin
    float* rmsInEachBin;
    Boolean flag = false;
    rmsInEachBin = new float[822];
    
    //creating new subvectors;
    std::vector<float> exactHitBuffer;
    std::vector<float> sub;

    for(int i = 0; i<hitBuffer.size()-100; i+=100){
        
        //calculate its rms and store it as an array element.
        rmsInEachBin[i/100] = calcVectorRMS(hitBuffer,i,i+100);
        
        //if there is a sound that is louder than 1.5 of the room average rms, it detects it as a hit.
        if(rmsInEachBin[i/100] > maxRoomRMS && !flag){
            float hitsc = calcVectorSC(hitBuffer, i);
//            ofLog(OF_LOG_NOTICE, "SC: %f", hitsc);
            flag = true;
        }
    }
//    ofLog(OF_LOG_NOTICE, "Max Room RMS: %f", maxRoomRMS);
//    for(int i = 0; i< 822; i++){
//        if(rmsInEachBin[i]>maxRoomRMS){
//            ofLog(OF_LOG_NOTICE, "RMS index: %d", i);
//            ofLog(OF_LOG_NOTICE, "RMS: %f", rmsInEachBin[i]);
//        }
//    }
    


}

//--------------------------------------------------------------

float gilbert::calcVectorRMS(const vector<float>& shortBuffer, int startPoint, int endPoint){
    float count = 0;
    for(int i=startPoint; i<endPoint; i++){
        count += pow(shortBuffer[i],2);
    }
    count = count/initialBufferSize;
    return sqrt(count);
}

//--------------------------------------------------------------
float gilbert::calcVectorSC(vector<float>& exactHit, int startPoint){
    float * exactHitArray = &exactHit[0];
    float * magn = new float[256];
    float * phas = new float[256];
    float * pow = new float[256];
    float avg_power;
    
    
    
    myfft.powerSpectrum(startPoint, 128, exactHitArray, 256, magn, phas, pow, &avg_power);
    for(int i = 0; i<256 ; i++){
        ofLog(OF_LOG_NOTICE, "Freq: "+ofToString(i*44100/256)+", Mag: "+ofToString(pow[i]));
    }
    
    float centroid;
    
    float sumMags = 0;
    float sumFreqByMags = 0;
    
    for(int i = 0; i< 128; i++){
        sumMags += pow[i];
        sumFreqByMags += pow[i]*((float)i*44100.0/128.0);
    }
    
    ofLog(OF_LOG_NOTICE, "MagTot: "+ofToString(sumMags));
    ofLog(OF_LOG_NOTICE, "MagTot*freq: "+ofToString(sumFreqByMags));
    
    centroid = sumFreqByMags/sumMags;
    ofLog(OF_LOG_NOTICE, "Cent: "+ofToString(centroid));
    
    return centroid;
}










