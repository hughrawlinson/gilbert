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
    
	
    
    string __dir = ofDirectory().getAbsolutePath();
    
    SndfileHandle sn = SndfileHandle(__dir+"sounds/snare.wav");
    SndfileHandle ki = SndfileHandle(__dir+"sounds/kick.wav");
    SndfileHandle ha = SndfileHandle(__dir+"sounds/hat.wav");
    SndfileHandle cr = SndfileHandle(__dir+"sounds/crash.wav");
    
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
    
    hat.loadSound("sounds/hat.wav");
    float habu [ha.frames()];
    ha.read (habu, ha.frames());
    float pow3 [ha.frames()];
    myfft.powerSpectrum(0, 4096, habu, 8192, magn, phase, pow3, &avg_pow);
    normalizeComplement(pow3,ha.frames());
    haspec = pow3;
    
    crash.loadSound("sounds/crash.wav");
    float crbu [cr.frames()];
    cr.read (crbu, cr.frames());
    float pow4 [cr.frames()];
    myfft.powerSpectrum(0, 4096, crbu, 8192, magn, phase, pow4, &avg_pow);
    normalizeComplement(pow4,cr.frames());
    crspec = pow4;
    
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
    
    if(ofGetElapsedTimeMillis() < 1500){
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
    
    ofPushStyle();
	ofSetColor(255);
    ofDrawBitmapString("SC: " + ofToString(calcSC()),20, ofGetHeight()-60);
    ofDrawBitmapString("AP: " + ofToString(avg_power),20, ofGetHeight()-40);
    ofDrawBitmapString("RMS: " + ofToString(bufrms), 20, ofGetHeight()-20);
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
                analyseHitBuffer(aBuffer, "a");
                aPressed=false;
                break;
            }
        }
    }
    else if(bPressed){
        for(int i=0; i<minBufferSize; i++){
            bBuffer.push_back(buffer[i]);
            if(bBuffer.size() >= 88200){
                analyseHitBuffer(bBuffer, "b");
                bPressed=false;
                break;
            }
        }
    }
    else if(cPressed){
        for(int i=0; i<minBufferSize; i++){
            cBuffer.push_back(buffer[i]);
            if(cBuffer.size() >= 88200){
                analyseHitBuffer(cBuffer, "c");
                cPressed=false;
                break;
            }
        }
    }
    else if(dPressed){
        for(int i=0; i<minBufferSize; i++){
            dBuffer.push_back(buffer[i]);
            if(dBuffer.size() >= 88200){
                analyseHitBuffer(dBuffer, "d");
                dPressed=false;
                break;
            }
        }
    }
    myfft.powerSpectrum(0, (int)BUFFER_SIZE/2, buffer, BUFFER_SIZE, magnitude, phase, power, &avg_power);
//    power = normalize(power, BUFFER_SIZE);
    
    //filtering
    if(kick.getIsPlaying()){
        for(int x = 0; x < BUFFER_SIZE; x++){
            power[x] = power[x]*kispec[x];
        }
    }
    else if(snare.getIsPlaying()){
        for(int x = 0; x < BUFFER_SIZE; x++){
            power[x] = power[x]*snspec[x];
        }
    }
    else if(hat.getIsPlaying()){
        for(int x = 0; x < BUFFER_SIZE; x++){
            power[x] = power[x]*haspec[x];
        }
    }
    else if(crash.getIsPlaying()){
        for(int x = 0; x < BUFFER_SIZE; x++){
            power[x] = power[x]*crspec[x];
        }
    }
    
    float sum = 0;
    for(int i = 0; i < BUFFER_SIZE; i++){
        sum += pow(abs(power[i]/BUFFER_SIZE),2);
    }
    float brms = sqrt(sum);
    if(isnan(brms) || isinf(brms)){
        bufrms = 0;
    }
    else{
        bufrms = brms;
    }
    
    if(bufrms >= maxRoomRMS * 10 && inputSfsSet.size()>0){
        float * rmsInEachBin;
        int highestBinIndex = 0;
        float highestBin = 0;
        rmsInEachBin = new float[BUFFER_SIZE/8];
        
        for(int i = 0; i<BUFFER_SIZE-8; i+=8){
            rmsInEachBin[i/8] = calcRMS(buffer+i, 8);
            if(rmsInEachBin[i/8] > highestBin){
                highestBin = rmsInEachBin[i/8];
                highestBinIndex = i;
            }

        }
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
        sfs input1 = {.id="static",.centroid=calcSC()/22500.0f,.rms=bufrms};
        string soundid = lookupClosest(input1);
//        ofLog(OF_LOG_NOTICE,"triggered sound: "+soundid);
        if(soundid=="a"){
            kick.setVolume(calcRMS()*2);
            kick.play();
        }
        if(soundid=="b"){
            snare.setVolume(calcRMS()*2);
            snare.play();
        }
        if(soundid=="c"){
            hat.setVolume(calcRMS()*2);
            hat.play();
        }
        if(soundid=="d"){
            crash.setVolume(calcRMS()*2);
            crash.play();
        }
    }
    audioFinished = true;
}

//--------------------------------------------------------------
string gilbert::lookupClosest(sfs input){
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
//    ofLog(OF_LOG_NOTICE,"distance: "+ofToString(closestDist));
    if(closestDist<2){
        return closest.id;
    }
    else{
        return "";
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
float gilbert::calcRMS(float* b, int size){
    float count = 0;
    for(int i=0; i<size; i++){
        count += pow(b[i],2);
    }
    count = count/size;
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

void gilbert::analyseHitBuffer(vector<float>& hitBuffer, string drum){
    
    //array to store rms in each bin
    float* rmsInEachBin;
    Boolean flag = false;
    rmsInEachBin = new float[822];
    
    //creating new subvectors;
    std::vector<float> exactHitBuffer;
    std::vector<float> sub;
    float hitsc;
    int startpoint;

    for(int i = 0; i<hitBuffer.size()-100; i+=100){
        
        //calculate its rms and store it as an array element.
        rmsInEachBin[i/100] = calcVectorRMS(hitBuffer,i,i+100);
        
        //if there is a sound that is louder than 1.5 of the room average rms, it detects it as a hit.
        if(rmsInEachBin[i/100] > maxRoomRMS * 10 && !flag){
            hitsc = calcVectorSC(hitBuffer, i);
            startpoint = i;
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
    sfs thisssss = {.id=drum, .centroid=hitsc/22500.0f, .rms=calcRMS(&hitBuffer[startpoint],441)};
    inputSfsSet.push_back(thisssss);
    
}

//--------------------------------------------------------------

float gilbert::calcVectorRMS(const vector<float>& shortBuffer, int startPoint, int endPoint){
    float count = 0;
    endPoint = endPoint+startPoint > shortBuffer.size()?shortBuffer.size()-1:endPoint;
    for(int i=startPoint; i<endPoint; i++){
        count += pow(shortBuffer[i],2);
    }
    count = count/shortBuffer.size();
    return sqrt(count);
}

//--------------------------------------------------------------
float gilbert::calcVectorSC(vector<float>& exactHit, int startPoint){
    float * exactHitArray = &exactHit[0];
    float * magn = new float[BUFFER_SIZE/2];
    float * phas = new float[BUFFER_SIZE/2];
    float * pow = new float[BUFFER_SIZE/2];
    float avg_power;
    
    myfft.powerSpectrum(startPoint, 128, exactHitArray, 256, magn, phas, pow, &avg_power);
    for(int i = 0; i<256 ; i++){
//        ofLog(OF_LOG_NOTICE, "Freq: "+ofToString(i*44100/256)+", Mag: "+ofToString(pow[i]));
    }
    
    float centroid;
    
    float sumMags = 0;
    float sumFreqByMags = 0;
    
    for(int i = 0; i< 128; i++){
        sumMags += pow[i];
        sumFreqByMags += pow[i]*((float)i*44100.0/128.0);
    }
    
    centroid = sumFreqByMags/sumMags;
    
    return centroid;
}




