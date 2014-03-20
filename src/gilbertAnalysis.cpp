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

float gilbertAnalysis::calcVectorRMS(const std::vector<float>& shortBuffer, int startPoint, int endPoint){
    float count = 0;
    endPoint = endPoint+startPoint > shortBuffer.size()?shortBuffer.size()-1:endPoint;
    for(int i=startPoint; i<endPoint; i++){
        count += pow(shortBuffer[i],2);
    }
    count = count/shortBuffer.size();
    return sqrt(count)/6500.0f;
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
float gilbertAnalysis::calcVectorSC(std::vector<float>& exactHit){
    float * exactHitArray = &exactHit[0];
    float * magn = new float[exactHit.size()/2];
    float * phas = new float[exactHit.size()/2];
    float * pow = new float[exactHit.size()/2];
    float avg_power;
    
    myfft.powerSpectrum(0, 4410, exactHitArray, 8820, magn, phas, pow, &avg_power);
//    for(int i = 0; i<256 ; i++){
//        //        ofLog(OF_LOG_NOTICE, "Freq: "+ofToString(i*44100/256)+", Mag: "+ofToString(pow[i]));
//    }
    
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


//---------------------------------------------------------------

sfs gilbertAnalysis::analyseHitBuffer(std::vector<float>& hitBuffer, std::string drum, float ambientRMS){
    //array to store rms in each bin
    float* rmsInEachBin;
    bool flag = false;
    rmsInEachBin = new float[822];
    
    //creating new subvectors;
    float hitsc;
    int startpoint;
    
    for(int i = 0; i<hitBuffer.size()-100; i+=100){
        
        //calculate its rms and store it as an array element.
        rmsInEachBin[i/100] = calcVectorRMS(hitBuffer,i,i+100);
        
        //if there is a sound that is louder than 10 of the room average rms, it detects it as a hit.
        if(rmsInEachBin[i/100] > ambientRMS * 5 && !flag){
            
            std::vector<float>::const_iterator first = hitBuffer.begin() + i;
            std::vector<float>::const_iterator last = hitBuffer.begin() + i + 2205;
            std::vector<float> exactHit(first,last);

            writeWAV(exactHit, exactHit.size(), drum);
            
            //calcualte SC for the exact hit.
            hitsc = calcVectorSC(exactHit);
            startpoint = i;
            flag = true;
        }
    }
//    for(int i = 0; i< 822; i++){
//        if(rmsInEachBin[i]>ambientRMS){
//            ofLog(OF_LOG_NOTICE, "RMS index: %d", i);
//            ofLog(OF_LOG_NOTICE, "RMS: %f", rmsInEachBin[i]);
//        }
//    }
    sfs thisssss = {.id=drum, .centroid=hitsc, .rms=calcRMS(&hitBuffer[startpoint], 2205)};
    return thisssss;
}

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

void gilbertAnalysis::writeWAV(std::vector<float>& buffer, int bufferSize, std::string drum){
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
    sf_count_t count = sf_write_float(outfile, &exactHitArray[0], bufferSize) ;
    sf_write_sync(outfile);
    sf_close(outfile);
}
