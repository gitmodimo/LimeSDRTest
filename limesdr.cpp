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

    /* ... here is the expensive or blocking operation ... */
    int flags,ret; //flags set by receive operation
    long long timeNs,ptimeNs;
    while(1){
        ret=sdrDevice->readStream(rxStream,rxb,config.buffSize,flags,timeNs,1000000);
        //if(timeNs-ptimeNs>40960)
        qDebug()<<"RX ret="<<ret<<", flags="<<flags<<"timeNs="<<timeNs<<"dtimeNs="<<timeNs-ptimeNs;
        ptimeNs=timeNs;
        //QThread::msleep(1000);
    }
    emit resultReady(result);
}
