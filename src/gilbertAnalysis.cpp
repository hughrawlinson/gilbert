//
//  gilbertAnalysis.cpp
//  mySketch
//
//  Created by Nevo Segal on 19/03/2014.
//
//

#include "gilbertAnalysis.h"
#include <stdio.h>

gilbertAnalysis::gilbertAnalysis(){
    lastMags = new float[256];
}

//--------------------------------------------------------------
float gilbertAnalysis::calcRMS(float* b, int size){
    float count = 0;
    for(int i=0; i<size; i++){
        count += pow(b[i],2);
    }
    count = count/size;
    return sqrt(count);
}

//--------------------------------------------------------------
float gilbertAnalysis::calcRMS(std::vector<float>& buffer){
    float * shortBufferArray = &buffer[0];
    return calcRMS(shortBufferArray, buffer.size());
}

//--------------------------------------------------------------
float gilbertAnalysis::calcSC(float *b, int size){
    float centroid = 0;
    
    float samplerateDividedBySize = (44100.0f/(float)size);
    
    float sumMags = 0;
    float sumFreqByMags = 0;
    
    float * magnitude = new float[size];
	memset(magnitude, 0, size * sizeof(float));
    float * power = new float[size];
	memset(power, 0, size * sizeof(float));
    float * phase = new float[size];
	memset(phase, 0, size * sizeof(float));
    float avg_power;
    
    myfft.powerSpectrum(0, size/2, b, size, magnitude, phase, power, &avg_power);
    
    for(int i = 0; i < size; i++){
        sumMags += magnitude[i];
        sumFreqByMags += magnitude[i]*i*samplerateDividedBySize;
    }
    
    centroid = sumFreqByMags/sumMags;
    
    if(isnan(centroid) || isinf(centroid)){
        return 0.0f;
    }
    else{
        return centroid/22050.0f;
    }
}

//--------------------------------------------------------------
float gilbertAnalysis::calcSC(std::vector<float>& buffer){
    float * exactHitArray = &buffer[0];
    return calcSC(exactHitArray,buffer.size());
}

//--------------------------------------------------------------
float gilbertAnalysis::calcSF(float *magns, int size){
    float spectralFlux;
    float* mags = util::normalize(mags,size);
    
    for(int i = 0; i < size; i++){
        spectralFlux += pow(magns[i]-lastMags[i],2);
    }
    lastMags = mags;
    return pow(spectralFlux,1.0/size);
}

//--------------------------------------------------------------
float gilbertAnalysis::calcSF(std::vector<float>&buffer){
    float * shortBufferArray = &buffer[0];
    return calcSF(shortBufferArray,buffer.size());
}

std::vector<float> gilbertAnalysis::getExactHit(std::vector<float>&hitBuffer, float ambientRMS){
    std::vector<float> exactHit(2048);
    float* rmsInEachBin;
    rmsInEachBin = new float[822];
    int highestRMSBin = 0;
    float highestRMSValue = ambientRMS;
    for(int i = 0; i<hitBuffer.size()-100; i+=100){
        
        //calculate its rms and store it as an array element.
        std::vector<float>::const_iterator first = hitBuffer.begin() + i;
        std::vector<float>::const_iterator last = hitBuffer.begin() + i + 100;
        std::vector<float> hitBufferBin(first, last);
        rmsInEachBin[i/100] = calcRMS(hitBufferBin);
        
        if(rmsInEachBin[i/100]>highestRMSValue){
            highestRMSValue = rmsInEachBin[i/100];
            highestRMSBin = i;
        }
    }
    
    for(int j = 0 ; j < exactHit.size(); j++) {
        exactHit[j]=hitBuffer[j+highestRMSBin];
    }
    return exactHit;
}

//---------------------------------------------------------------
sfs gilbertAnalysis::analyseHitBuffer(std::vector<float>& hitBuffer, std::string drum, float ambientRMS){
    
    //array to store rms in each bin
    float hitSF = 0;
    
    std::vector<float> exactHit = getExactHit(hitBuffer, ambientRMS);
    //hitSF = calcSF(getExactHit(hitBuffer, ambientRMS));
    sfs hitInfo = {.id=drum, .centroid=calcSC(exactHit), .rms=calcRMS(exactHit), .flux = hitSF};
    
    return hitInfo;
}

sfs gilbertAnalysis::analyseHitBuffer(std::vector<float>& hitBuffer, std::string drum, float ambientRMS, bool writeWav){
    std::vector<float> exactHit = getExactHit(hitBuffer, ambientRMS);
    sfs hitInfo = analyseHitBuffer(hitBuffer, drum, ambientRMS);
    if(writeWav){
        writeWAV(exactHit, exactHit.size(), drum, hitInfo);
    }
    
    return hitInfo;
}

//--------------------------------------------------------------
void gilbertAnalysis::writeWAV(std::vector<float>& buffer, int bufferSize, std::string drum, sfs info){
    //    std::cout <<"Hello" <<std::endl;
    float* exactHitArray;
    exactHitArray = new float[buffer.size()];
    for(int j = 0; j<buffer.size(); j++){
        exactHitArray[j] = buffer[j];
    }
    // define the desired output format
    SF_INFO sfinfo ;
    sfinfo.channels = 1;
    sfinfo.samplerate = 44100;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    
    std::string path = "../../../";
    path+=drum;
    path+=".wav";
    std::cout << path.c_str() << std::endl;
    
    SNDFILE * outfile = sf_open(path.c_str(), SFM_WRITE, &sfinfo);
    std::cout << sf_strerror(outfile) << std::endl;
    char strbuf[50];
    sprintf(strbuf, "RMS: %f, \n SC: %f \n SF: %f", info.rms, info.centroid, info.flux);
    sf_set_string(outfile, SF_STR_COMMENT, strbuf);
    sf_count_t count = sf_write_float(outfile, &exactHitArray[0], bufferSize) ;
    sf_write_sync(outfile);
    sf_close(outfile);
}
