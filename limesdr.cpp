#include "limesdr.h"
#include "ui_limesdr.h"



LimeSDR::LimeSDR(LimeSDRConfig _config,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LimeSDR),
    worker(_config)
{
    ui->setupUi(this);


  //  int

}

LimeSDR::~LimeSDR()
{

    delete ui;
}


LimeSDRWorker::LimeSDRWorker(LimeSDRConfig _config):config(_config){


    try {
        sdrDevice = SoapySDR::Device::make(config.devicesKwargs);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
\
        SoapySDR::Device::unmake(sdrDevice);
    }

//
    try{
        sdrDevice->setSampleRate(SOAPY_SDR_RX, 0, config.samplerate);

    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
        sdrDevice->setSampleRate(SOAPY_SDR_RX, 1,  config.samplerate);

    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }    //apply settings
    try{
        sdrDevice->setSampleRate(SOAPY_SDR_TX, 0,  config.samplerate);

    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
        sdrDevice->setSampleRate(SOAPY_SDR_TX, 1,  config.samplerate);

    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }


    //
    try{
    sdrDevice->setBandwidth(SOAPY_SDR_RX, 0, config.bw);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setBandwidth(SOAPY_SDR_RX, 1, config.bw);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setBandwidth(SOAPY_SDR_TX, 0, config.bw);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setBandwidth(SOAPY_SDR_TX, 1, config.bw);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    //

    try{
    sdrDevice->setFrequency(SOAPY_SDR_RX, 0, config.RXFreq, {});
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setFrequency(SOAPY_SDR_RX, 1, config.RXFreq, {});
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
  //
    try{
    sdrDevice->setFrequency(SOAPY_SDR_TX, 0, config.TXFreq, {});
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setFrequency(SOAPY_SDR_TX, 1, config.TXFreq, {});
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
//
    try{
    sdrDevice->setAntenna(SOAPY_SDR_RX, 0, config.RXAntenna);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setAntenna(SOAPY_SDR_RX, 1,  config.RXAntenna);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    //
    try{
    sdrDevice->setAntenna(SOAPY_SDR_TX, 0, config.TXAntenna);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setAntenna(SOAPY_SDR_TX, 1, config.TXAntenna);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
//

    try{
    sdrDevice->setGain(SOAPY_SDR_RX, 0, "LNA",config.RXGainLNA);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setGain(SOAPY_SDR_RX, 1, "LNA",config.RXGainLNA);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }

    try{
    sdrDevice->setGain(SOAPY_SDR_RX, 0, "TIA",config.RXGainTIA);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setGain(SOAPY_SDR_RX, 1, "TIA",config.RXGainTIA);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }

    try{
    sdrDevice->setGain(SOAPY_SDR_RX, 0, "PGA",config.RXGainPGA);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setGain(SOAPY_SDR_RX, 1, "PGA",config.RXGainPGA);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }

    try{
    sdrDevice->setGain(SOAPY_SDR_TX, 0, "PAD",config.TXGainPAD);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
    sdrDevice->setGain(SOAPY_SDR_TX, 1, "PAD",config.TXGainPAD);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    //
    sdrDevice->setDCOffsetMode(SOAPY_SDR_RX,0,config.RXDCOffsetMode);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_RX,1,config.RXDCOffsetMode);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_TX,0,config.TXDCOffsetMode);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_TX,1,config.TXDCOffsetMode);


    try{
        sdrDevice->writeSetting(SOAPY_SDR_RX,0,"APPLY_CORRECTIONS",config.RXApplyCorrections);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
        sdrDevice->writeSetting(SOAPY_SDR_RX,1,"APPLY_CORRECTIONS",config.RXApplyCorrections);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
        sdrDevice->writeSetting(SOAPY_SDR_TX,0,"APPLY_CORRECTIONS",config.TXApplyCorrections);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
    try{
        sdrDevice->writeSetting(SOAPY_SDR_TX,1,"APPLY_CORRECTIONS",config.TXApplyCorrections);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }

   // sdrDevice->getStreamMTU()
    //
    RX_buffs[0]=new std::complex<int16_t>[config.buffSize];
    RX_buffs[1]=new std::complex<int16_t>[config.buffSize];

    TX_buffs[0]=new std::complex<int16_t>[config.buffSize];
    TX_buffs[1]=new std::complex<int16_t>[config.buffSize];

}
LimeSDRWorker::~LimeSDRWorker(){

    SoapySDR::Device::unmake(sdrDevice);
    delete []RX_buffs[0];
    delete []RX_buffs[1];
    //delete []RX_buffs[1][0];
    //delete []RX_buffs[1][1];
    delete []TX_buffs[0];
    delete []TX_buffs[1];
    //delete []TX_buffs[1][0];
    //delete []TX_buffs[1][1];
}


void LimeSDRWorker::run(){
    QString result("done");

    try{
        rxStream=sdrDevice->setupStream(SOAPY_SDR_RX,SOAPY_SDR_CS16,{0,1},{});
        qDebug()<<"RX mtu:"<<sdrDevice->getStreamMTU(rxStream);
        txStream=sdrDevice->setupStream(SOAPY_SDR_TX,SOAPY_SDR_CS16,{0,1},{});
        qDebug()<<"TX mtu:"<<sdrDevice->getStreamMTU(txStream);
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }

    sdrDevice->activateStream(rxStream,0,0,0);
    sdrDevice->activateStream(txStream,0,0,0);


    void *rxb[] = {RX_buffs[0],RX_buffs[1]}; //array of buffers
    void *txb[2];
    void *dummytxb[2];
    std::complex<int16_t> dummy[1024];
    dummytxb[0]=dummy;
    dummytxb[1]=dummy;

    /* ... here is the expensive or blocking operation ... */
    int flags;
    int ret; //flags set by receive operation
    long long timeNs,ptimeNs;
    const long long txBufferAhead=0.1e9;
    long long txBufferTime=txBufferAhead;
    long long readTick[2];
    long long  txTime0 = sdrDevice->getHardwareTime() + (0.001e9) ;//100ms
    int readCounter[2];
    LimeSDRTask* task;
    bool taskDone=true;
    bool taskFailed=false;
    bool bufferused=false;



    while(1){
        // for(int i=0;i<100;++i){
        while(txBufferTime-txBufferAhead<sdrDevice->getHardwareTime()){
            if(taskDone){
                task=takeTask();
                if(task!=nullptr){
                    readCounter[0]=0;
                    readCounter[1]=0;
                    taskDone=false;
                    taskFailed=false;
                    //task->timestamp = sdrDevice->getHardwareTime() + (0.001e9) ;//100ms
                    task->timestamp=txBufferTime;
                    //flags=SOAPY_SDR_HAS_TIME;
                    txb[0]=task->TX[0].data();
                    txb[1]=task->TX[1].data();
                    size_t buffsize=std::min(task->TX[0].size(),task->TX[1].size());
                    flags=SOAPY_SDR_HAS_TIME;
                    ret=sdrDevice->writeStream(txStream,txb,buffsize,flags,task->timestamp ,1000000);
                    if(ret)
                        txBufferTime+=SoapySDR::ticksToTimeNs(ret,config.samplerate);
                    //qDebug()<<"TX ret="<<ret<<", flags="<<flags<<"timeNs="<<task->timestamp<<SoapySDR::timeNsToTicks(task->timestamp,config.samplerate);
                }else{
                    flags=SOAPY_SDR_HAS_TIME;
                    ret=sdrDevice->writeStream(txStream,dummytxb,1024,flags,txBufferTime ,1000000);
                    if(ret)
                        txBufferTime+=SoapySDR::ticksToTimeNs(ret,config.samplerate);
                    //qDebug()<<"DTX ret="<<ret<<", flags="<<flags<<"timeNs="<<txBufferTime<<SoapySDR::timeNsToTicks(txBufferTime,config.samplerate);

                }
            }else{
                flags=SOAPY_SDR_HAS_TIME;
                ret=sdrDevice->writeStream(txStream,dummytxb,1024,flags,txBufferTime ,1000000);
                if(ret)
                    txBufferTime+=SoapySDR::ticksToTimeNs(ret,config.samplerate);
       //         qDebug()<<"DDTX ret="<<ret<<", flags="<<flags<<"timeNs="<<txBufferTime<<SoapySDR::timeNsToTicks(txBufferTime,config.samplerate);

            }

        }

        flags=0;
        ret=sdrDevice->readStream(rxStream,rxb,config.buffSize,flags,timeNs,1000000);
       /* qDebug()<<SoapySDR::timeNsToTicks(sdrDevice->getHardwareTime(),config.samplerate)
               <<SoapySDR::timeNsToTicks(txBufferTime,config.samplerate)
              <<SoapySDR::timeNsToTicks(timeNs,config.samplerate)
             <<SoapySDR::timeNsToTicks(txBufferTime,config.samplerate)-SoapySDR::timeNsToTicks(sdrDevice->getHardwareTime(),config.samplerate)
            <<SoapySDR::timeNsToTicks(sdrDevice->getHardwareTime(),config.samplerate)-SoapySDR::timeNsToTicks(timeNs,config.samplerate)
                ;*/
        bufferused=false;
        if(ret&&taskDone==false){

            long long taskTick=SoapySDR::timeNsToTicks(task->timestamp,config.samplerate);
            long long bufferTick=SoapySDR::timeNsToTicks(timeNs,config.samplerate);

            for(unsigned int i=0;i<2;++i){

                 if(bufferTick+ret>taskTick){//triggered
                     bufferused=true;
                     if(bufferTick<=taskTick){//beginning
                         readTick[i]=taskTick;
                         for(unsigned int j=(taskTick-bufferTick);
                             (j<ret && readCounter[i]<task->RX[i].size());
                             j++){
                             task->RX[i][readCounter[i]++]=RX_buffs[i][j];
                             readTick[i]++;
                         }
                     }
                     else if(readCounter[i]<task->RX[i].size()){
                         if(readTick[i]!=bufferTick){
                             taskFailed=true;
                             qDebug()<<"Read lost: "<<readTick[i]<<"!="<<bufferTick;
                         }
                         for(unsigned int j=0;
                             (j<ret && readCounter[i]<task->RX[i].size());
                             j++){
                             task->RX[i][readCounter[i]++]=RX_buffs[i][j];
                             readTick[i]++;
                         }
                     }
                 }

                if(readCounter[0]==task->RX[0].size() && readCounter[1]==task->RX[1].size()||
                        taskTick+std::max(task->RX[0].size(),task->RX[1].size())<bufferTick){
                    if(readCounter[0]!=task->RX[0].size() || readCounter[1]!=task->RX[1].size())
                        taskFailed=true;
                    taskDone=true;
                    if(taskFailed==false){
                        finishTask(task);

                        task=nullptr;
                    }

                }
            }
        }
      //  if(bufferused)            qDebug()<<"RX ret="<<ret<<", flags="<<flags<<"timeNs="<<timeNs<<"dtimeNs="<<timeNs-ptimeNs<<readTick[0]<<readTick[1];

       ptimeNs=timeNs;


       //  QThread::usleep(10000);
    }
    emit resultReady(result);
}

void LimeSDRWorker::addTask(LimeSDRTask *task){
    list_mutex.lock();
    taskList.push_back(task);
    list_mutex.unlock();
}

LimeSDRTask *LimeSDRWorker::takeTask(){
    LimeSDRTask* tsk;
    list_mutex.lock();
    if(taskList.empty()){
        tsk=nullptr;
    }
    else
    {
        tsk=taskList.front();
        taskList.pop_front();
    }
    list_mutex.unlock();
    return tsk;
}

void LimeSDRWorker::finishTask(LimeSDRTask *task){
    doneList_mutex.lock();
    taskDoneList.push_back(task);
    doneList_mutex.unlock();
    emit taskDone();

}

LimeSDRTask *LimeSDRWorker::takeFinishedTask(){
    LimeSDRTask* tsk;
    doneList_mutex.lock();
    if(taskDoneList.empty()){
        tsk=nullptr;
    }
    else
    {
        tsk=taskDoneList.front();
        taskDoneList.pop_front();
    }
    doneList_mutex.unlock();
    return tsk;
}
