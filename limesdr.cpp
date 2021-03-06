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

    setTXf(config.TXFreq/1e6);
    setRXf(config.RXFreq/1e6);
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


    qDebug()<<"Start Cal";

    calibMode=3;
    calibrateAll();
   /* qDebug()<<"Apply Cal";
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
    }*/

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
                if(doCalibrate.tryAcquire()){
                    sdrDevice->deactivateStream(rxStream,0,0);
                    sdrDevice->deactivateStream(txStream,0,0);
                    calibrateAll();
                    sdrDevice->activateStream(rxStream,0,0,0);
                    sdrDevice->activateStream(txStream,0,0,0);
                    txBufferTime=txBufferAhead;
                    txTime0 = sdrDevice->getHardwareTime() + (0.001e9) ;//100ms

                }
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

void LimeSDRWorker::setLNA(int gain){
    config.RXGainLNA=gain;
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
}

void LimeSDRWorker::setTIA(int gain){
    config.RXGainTIA=gain;
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
}

void LimeSDRWorker::setPGA(int gain){
    config.RXGainPGA=gain;
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
}

void LimeSDRWorker::setPAD(int gain){
    config.TXGainPAD=gain;
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
}

void LimeSDRWorker::setRxAntenna(QString ant){
    config.RXAntenna=ant.toStdString();
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
}

void LimeSDRWorker::calibrateAll(){

    double RXGainLNA=config.RXGainLNA;
    double RXGainTIA=config.RXGainTIA;
    double RXGainPGA=config.RXGainPGA;
    double TXGainPAD=config.TXGainPAD;
    std::complex<double> q;
    setLNA(30);
    setTIA(0);
    setPGA(19);
    setPAD(0);
    QThread::msleep(500);
    if(calibMode&1){
        try{
            sdrDevice->writeSetting(SOAPY_SDR_TX,0,"CALIBRATE_TX",QString::number((int)config.bw).toStdString());
        } catch (const std::exception& e) {
            qDebug()<<e.what();
        }
        q=sdrDevice->getIQBalance(SOAPY_SDR_TX,0);
        qDebug()<<q.real()<<q.imag();

        try{
            sdrDevice->writeSetting(SOAPY_SDR_TX,1,"CALIBRATE_TX",QString::number((int)config.bw).toStdString());
        } catch (const std::exception& e) {
            qDebug()<<e.what();
        }
        q=sdrDevice->getIQBalance(SOAPY_SDR_TX,1);
        qDebug()<<q.real()<<q.imag();

    }
    if(calibMode&2){
        try{
            sdrDevice->writeSetting(SOAPY_SDR_RX,0,"CALIBRATE_RX",QString::number((int)config.bw).toStdString());
        } catch (const std::exception& e) {
            qDebug()<<e.what();
        }
        q=sdrDevice->getIQBalance(SOAPY_SDR_RX,0);
        qDebug()<<q.real()<<q.imag();

        try{
            sdrDevice->writeSetting(SOAPY_SDR_RX,1,"CALIBRATE_RX",QString::number((int)config.bw).toStdString());
        } catch (const std::exception& e) {
            qDebug()<<e.what();
        }
        q=sdrDevice->getIQBalance(SOAPY_SDR_RX,1);
        qDebug()<<q.real()<<q.imag();


        unsigned sto=sdrDevice->readRegister(0x0100);
        unsigned LMS7_CFB_TIA_RFE=sdrDevice->readRegister(0x0112);
        /*qDebug()<<"LMS7_CFB_TIA_RFE:"<<LMS7_CFB_TIA_RFE<<sto;
        try{
            sdrDevice->writeSetting("SAVE_CONFIG","CFG.txt");
        } catch (const std::exception& e) {
            qDebug()<<e.what();
        }*/


    }
    setLNA(RXGainLNA);
    setTIA(RXGainTIA);
    setPGA(RXGainPGA);
    setPAD(TXGainPAD);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_RX,0,config.RXDCOffsetMode);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_RX,1,config.RXDCOffsetMode);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_TX,0,config.TXDCOffsetMode);
    sdrDevice->setDCOffsetMode(SOAPY_SDR_TX,1,config.TXDCOffsetMode);

}

void LimeSDRWorker::calibrateRequestTX(){
    calibMode=1;
    doCalibrate.release();
}

void LimeSDRWorker::restartRequest(){
    calibMode=0;
    doCalibrate.release();
}

void LimeSDRWorker::saveConfig(){
    try{
        sdrDevice->writeSetting("SAVE_CONFIG","CFG.txt");
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
}

void LimeSDRWorker::loadConfig(){
    try{
        sdrDevice->writeSetting("LOAD_CONFIG","CFG.txt");
    } catch (const std::exception& e) {
        qDebug()<<e.what();
    }
}

void LimeSDRWorker::setRXf(double freq){
    config.RXFreq=freq*1e6;
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
}

void LimeSDRWorker::setTXf(double freq){
    config.TXFreq=freq*1e6;
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
}
void LimeSDRWorker::calibrateRequestRX(){
    calibMode=2;
    doCalibrate.release();
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
