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
float gilbertAnalysis::calcRMS(std::vector<float>& shortBuffer){
    float * shortBufferArray = &shortBuffer[0];
    return calcRMS(shortBufferArray, shortBuffer.size());
}

//--------------------------------------------------------------
float gilbertAnalysis::calcSC(float *b, int size){
    float centroid = 0;
    
    float sumMags = 0.1;
    float sumFreqByMags = 0;
    
    for(int i = 0; i < size; i++){
        sumMags += b[i];
        sumFreqByMags += b[i]*((float)i*(44100.0f/(float)size));
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
float gilbertAnalysis::calcSC(std::vector<float>& exactHit){
    float * exactHitArray = &exactHit[0];
    return calcSC(exactHitArray,exactHit.size());
}

//---------------------------------------------------------------
sfs gilbertAnalysis::analyseHitBuffer(std::vector<float>& hitBuffer, std::string drum, float ambientRMS){
    
    //array to store rms in each bin
    float* rmsInEachBin;
    rmsInEachBin = new float[822];
    float hitSC =0,
    hitRMS = 0,
    hitSF = 0;
    std::vector<float> exactHit(2205);
//    sfs hitInfo;
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
    hitSC = calcSC(exactHit);
    hitRMS = calcRMS(exactHit);
//    hitSF = calcSF(&exactHit[0],exactHit.size());
    sfs hitInfo = {.id=drum, .centroid=hitSC, .rms=hitRMS, .flux = hitSF};
    writeWAV(exactHit, exactHit.size(), drum, hitInfo);
    
    return hitInfo;
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
