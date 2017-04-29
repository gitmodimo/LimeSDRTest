#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fftw3.h>
#include <complex.h>
#include <QDebug>
#include <QTimer>
#define C_FULL_SCALE ((double)2048)
#define SAMPLING 50e6
#define NUMFFT 2048

double tukeyWindow(int x_inp,int L,double r){
    double x=(double)x_inp/(double)L;
    double win=1;
    if(x<(r/2)){
        win=0.5*(1+std::cos(2*M_PI/r*(x-r/2)));
    }
    if(x>=(1-r/2)){

        win=0.5*(1+std::cos(2*M_PI/r*(x-1+r/2)));
    }
    //qDebug()<<x_inp<<x<<L<<r<<win;
    return win;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    devicesCount=0;
    refreshDevicesList();
    connect(ui->refreshDevice,SIGNAL(clicked(bool)),SLOT(refreshDevicesList()));
    connect(ui->connectButton,SIGNAL(clicked(bool)),SLOT(connectDevice()));
    connect(ui->resetButton,SIGNAL(clicked(bool)),SLOT(resetAvg()));


    timer = new QTimer(this);


      resetavg=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshDevicesList(){
    int id=0;
    devicesKwargs.clear();
    //SoapySDR::KwargsList_clear(devicesKwargs, devicesCount);
    ui->deviceComboBox->clear();
    QString devicename;

    devicesKwargs = SoapySDR::Device::enumerate(SoapySDR::Kwargs());//, &devicesCount

    for(SoapySDR::Kwargs& dev : devicesKwargs)
    {
        for(auto const &e : dev) {

             devicename=devicename+e.first.c_str()+"="+e.second.c_str()+
                     ", ";
        }
        ui->deviceComboBox->addItem(devicename,QVariant(id++));
    }
}

void MainWindow::connectDevice(){
    int id=0;
    int deviceId=ui->deviceComboBox->itemData(ui->deviceComboBox->currentIndex()).toInt();
   // SoapySDR::Kwargs args;
    for (auto const &e : devicesKwargs[deviceId])
    {
        qDebug()<<e.first.c_str()<<"="<< e.second.c_str();
        if(strcmp(e.first.c_str(),"driver")==0){
            limeSDRconf.devicesKwargs[e.first]=e.second;
        }
    }

    limeSDRconf.devicesKwargs["cacheCalibrations"]="0";


    ///////////////////////////
    limeSDRconf.samplerate=20e6;
    limeSDRconf.bw=20e6;
    limeSDRconf.RXFreq=325e6;//1880.0e6;//10.0e6+2462.0e6;//1880.0e6;//90e6;//2457.0e6;10.0e6+2462.0e6;//
    limeSDRconf.TXFreq=325e6;//1880.0e6+0e6+0*.0001e6;//10e6+//10.0e6+2462.0e6;//1880.0e6;//90e6;//2457.0e6;10.0e6+2462.0e6;//
    limeSDRconf.RXAntenna="LNAW";
    limeSDRconf.TXAntenna="BAND1";
    limeSDRconf.RXGainLNA=30-10;
    limeSDRconf.RXGainTIA=0;
    limeSDRconf.RXGainPGA=-12;
    limeSDRconf.TXGainPAD=0-30;//-10-10;
    limeSDRconf.RXDCOffsetMode=true;
    limeSDRconf.TXDCOffsetMode=true;
    limeSDRconf.RXApplyCorrections="1";
    limeSDRconf.TXApplyCorrections="1";
    limeSDRconf.buffSize=NUMFFT;

    ui->LNAGain->setValue(limeSDRconf.RXGainLNA);
    ui->TIAGain->setValue(limeSDRconf.RXGainTIA);
    ui->PGAGain->setValue(limeSDRconf.RXGainPGA);
    ui->PADGain->setValue(limeSDRconf.TXGainPAD);
    ui->RXfSpinBox->setValue(limeSDRconf.RXFreq/1e6);
    ui->TXfSpinBox->setValue(limeSDRconf.TXFreq/1e6);

    lime=new LimeSDR(limeSDRconf);
    connect(&lime->worker,SIGNAL(taskDone()),SLOT(updatePlot()));
    connect(timer, SIGNAL(timeout()),SLOT(addTask()));


    connect(ui->LNAGain,SIGNAL(valueChanged(int)),&lime->worker,SLOT(setLNA(int)));
    connect(ui->TIAGain,SIGNAL(valueChanged(int)),&lime->worker,SLOT(setTIA(int)));
    connect(ui->PGAGain,SIGNAL(valueChanged(int)),&lime->worker,SLOT(setPGA(int)));
    connect(ui->PADGain,SIGNAL(valueChanged(int)),&lime->worker,SLOT(setPAD(int)));
    connect(ui->antennaComboBox,SIGNAL(activated(QString)),&lime->worker,SLOT(setRxAntenna(QString)));
    connect(ui->calRXButton,SIGNAL(clicked(bool)),&lime->worker,SLOT(calibrateRequestRX()));
    connect(ui->calTXButton,SIGNAL(clicked(bool)),&lime->worker,SLOT(calibrateRequestTX()));
    connect(ui->restartButton,SIGNAL(clicked(bool)),&lime->worker,SLOT(restartRequest()));
    connect(ui->saveButton,SIGNAL(clicked(bool)),&lime->worker,SLOT(saveConfig()));
    connect(ui->loadButton,SIGNAL(clicked(bool)),&lime->worker,SLOT(loadConfig()));
    connect(ui->RXfSpinBox,SIGNAL(valueChanged(double)),&lime->worker,SLOT(setRXf(double)));
    connect(ui->TXfSpinBox,SIGNAL(valueChanged(double)),&lime->worker,SLOT(setTXf(double)));
   // connect(ui->calButton,SIGNAL(clicked(bool)),SLOT(addTask()));


    lime->worker.start();
    timer->start(50);
    //addTask();
    sdrDevice=lime->worker.sdrDevice;


    RX_Count= sdrDevice->getNumChannels(SOAPY_SDR_RX);
    TX_Count= sdrDevice->getNumChannels(SOAPY_SDR_TX);
    QString desc;
    desc=QString("Key:")+QString(sdrDevice->getHardwareKey().c_str());
    desc+=QString(" RX:")+QString::number(RX_Count)+QString(" TX:")+QString::number(TX_Count);

    desc+=QString(" Duplex: ")
            +QString(sdrDevice->getFullDuplex(SOAPY_SDR_RX,0)?"F":"H")
            +QString(sdrDevice->getFullDuplex(SOAPY_SDR_RX,1)?"F":"H")
            +QString(sdrDevice->getFullDuplex(SOAPY_SDR_TX,0)?"F":"H")
            +QString(sdrDevice->getFullDuplex(SOAPY_SDR_TX,1)?"F":"H");


    ui->deviceParams->setText(desc);



    auto antennas=sdrDevice->listAntennas(SOAPY_SDR_RX, 0);
    qDebug()<<"Rx antennas: ";
    id=0;
    for (const auto a:antennas){

        ui->antennaComboBox->addItem(a.c_str(),QVariant((int)id++));
        if(std::strcmp(limeSDRconf.RXAntenna.c_str(),a.c_str())==0)
            ui->antennaComboBox->setCurrentIndex(id-1);
        qDebug()<<a.c_str();
    }
    antennas.clear();

    auto gains = sdrDevice->listGains(SOAPY_SDR_RX, 0);
    qDebug()<<"Rx gains: ";
    id=0;
    for (const auto gain:gains){
        ui->gainComboBox->addItem(gain.c_str(),QVariant((int)id++));
        qDebug()<<gain.c_str();
    }
    gains.clear();

    auto freqs=sdrDevice->listFrequencies(SOAPY_SDR_RX, 0);
    qDebug()<<"Rx frequencies: ";
    id=0;
    for (const auto freq:freqs){
        ui->frequencyComboBox->addItem(freq.c_str(),QVariant((int)id++));
        qDebug()<<freq.c_str();
    }
    freqs.clear();

    auto bandwidth_range=sdrDevice->getBandwidthRange( SOAPY_SDR_RX, 0);
    qDebug()<<"Rx bandwidths: "<<bandwidth_range[0].minimum()<<":"<<bandwidth_range[0].step()<<":"<<bandwidth_range[0].maximum();
    ui->bandwidthSpinBox->setMinimum(bandwidth_range[0].minimum()/1e6);
    ui->bandwidthSpinBox->setMinimum(bandwidth_range[0].maximum()/1e6);

    auto freq_ranges=sdrDevice->getFrequencyRange(SOAPY_SDR_RX, 0);
    qDebug()<<"Rx freq ranges: ";
    for (const auto freq_range:freq_ranges){
        qDebug()<<freq_range.minimum()<< "Hz ->"<<freq_range.maximum()<<"Hz";
    }
    freq_ranges.clear();

    auto sample_ranges=sdrDevice->getSampleRateRange(SOAPY_SDR_RX, 0);
            //ranges = SoapySDRDevice_getSampleRateRange(sdrDevice, SOAPY_SDR_RX, 0, &length);
    qDebug()<<"Rx sample rate ranges: ";
    for (const auto sample_range:sample_ranges){
        qDebug()<<sample_range.minimum()<< "Hz ->"<<sample_range.maximum()<<"Hz";
    }
    sample_ranges.clear();

    auto sample_rates=sdrDevice->listSampleRates(SOAPY_SDR_RX, 0);
    qDebug()<<"Rx fss: ";
    for (const auto sample_rate:sample_rates){
        qDebug()<<sample_rate<< "Hz";
    }
    sample_rates.clear();



    std::vector<std::string> registers=sdrDevice->listRegisterInterfaces();
    for (const auto reg:registers){
        qDebug()<<"Reg: "<<reg.c_str();
    }
    SoapySDR::ArgInfoList settings=sdrDevice->getSettingInfo(SOAPY_SDR_RX,0);
    for (const  SoapySDR::ArgInfo setting:settings){
        qDebug()<<"Rx sett: "<<setting.key.c_str()<<setting.value.c_str()<<setting.name.c_str()
               <<setting.description.c_str()<<setting.units.c_str()<<setting.type;
        qDebug()<<"Rx sett range: "<<setting.range.minimum()<<setting.range.step()<<setting.range.maximum();
        for(const auto option:setting.options){
            qDebug()<<"Set Rx option: "<<option.c_str();
        }
        for(const auto optionName:setting.optionNames){
            qDebug()<<"Set Rx optionName: "<<optionName.c_str();
        }
    }
    settings=sdrDevice->getSettingInfo(SOAPY_SDR_TX,0);
    for (const  SoapySDR::ArgInfo setting:settings){
        qDebug()<<"Tx sett: "<<setting.key.c_str()<<setting.value.c_str()<<setting.name.c_str()
               <<setting.description.c_str()<<setting.units.c_str()<<setting.type;
        qDebug()<<"Tx sett range: "<<setting.range.minimum()<<setting.range.step()<<setting.range.maximum();
        for(const auto option:setting.options){
            qDebug()<<"Set Tx option: "<<option.c_str();
        }
        for(const auto optionName:setting.optionNames){
            qDebug()<<"Set Tx optionName: "<<optionName.c_str();
        }
    }


    /*std::vector<std::string> registers=sdrDevice->listRegisterInterfaces();
    for(const std::string register:registers){
        qDebug()<<"Reg: "<<register;
    }*/


    //updatefft();
    //



}

void MainWindow::updatefft(){


}
#define TX_BURST 2048
#define RX_BURST 4096
#include <fasttransforms.h>
void MainWindow::updatePlot(){
    alglib::complex_1d_array txsig,txsig2;
    alglib::complex_1d_array rxsig,rxsig2,diff,ddiff;
    alglib::complex_1d_array trxcorr;
    static alglib::complex_1d_array diff_avg,time_avg;
    LimeSDRTask *task=lime->worker.takeFinishedTask();
    static int init=0;
    static double phshift_avg=0;
    if(!init){
        init=1;
        time_avg.setlength(RX_BURST);
        diff_avg.setlength(RX_BURST);
        for(int j =0;j<task->RX[0].size();j++){
            diff_avg[j]=0;
            time_avg[j]=0;
        }
    }

    txsig.setlength(task->TX[0].size());
    rxsig.setlength(task->RX[0].size());
    txsig2.setlength(task->TX[0].size());
    rxsig2.setlength(task->RX[0].size());
    for(int j =0;j<task->TX[0].size();j++){
        // samples->push_back(QPointF(ts++,10*20*log(std::sqrt(std::pow(out2[j][0],2.0)+std::pow(out2[j][1],2.0)))));
        txsig[j]=alglib::complex(task->TX[0][j].real(),task->TX[0][j].imag());
        txsig2[j]=alglib::complex(task->TX[1][j].real(),task->TX[1][j].imag());
    }
    for(int j =0;j<task->RX[0].size();j++){
        rxsig[j]=alglib::complex(task->RX[0][j].real(),task->RX[0][j].imag());
        rxsig2[j]=alglib::complex(task->RX[1][j].real(),task->RX[1][j].imag());
    }


    plot2.setAxisScale(QwtPlot::yLeft,-2048,2048);
    plot2.draw("DIFF_ARG",rxsig,512,2048,REAL);
    plot2.draw("DIFF_ARG2",rxsig,512,2048,IMAG);


    //spectrogram

    plot3.setAxisScale(QwtPlot::yLeft,0,150);
    alglib::fftc1d(rxsig,rxsig.length());
    plot3.draw("RX_LOG",rxsig,FFT);
    alglib::fftc1d(rxsig2,rxsig2.length());
    plot3.draw("RX2_LOG",rxsig2,FFT);



    diff.setlength(task->RX[0].size());
    ddiff.setlength(task->RX[0].size());
    for(int j =0;j<task->RX[0].size();j++){
        diff[j]=rxsig[j]/rxsig2[j];
        double arg=atan2(diff[j].y,diff[j].x);
        double mod=std::sqrt(std::pow(diff[j].y,2.0)+std::pow(diff[j].x,2.0));
        diff[j].x=arg;
        diff[j].y=mod;


    }


    for(int j =0;j<task->RX[0].size();j++){
        if(j)
            ddiff[j].x=diff[j].x-diff[j-1].x;
        else
            ddiff[j].x=diff[j].x-diff[task->RX[0].size()-1].x;
        if(ddiff[j].x>M_PI)ddiff[j].x-=2.0*M_PI;
        if(ddiff[j].x<-M_PI)ddiff[j].x+=2.0*M_PI;
        diff_avg[j].x+=-diff_avg[j].x/16.0+ddiff[j].x/16.0;

    }


    double phshift=0;
    int integspan=RX_BURST/2*10.0e6/(double)limeSDRconf.samplerate;
    for(int j=0;j<integspan;++j){
        phshift+=ddiff[j].x;
        phshift+=ddiff[RX_BURST-j-1].x;
    }

    phshift_avg+=-phshift_avg/16.0+phshift/16.0;

    for(int j =1;j<task->RX[0].size();j++){
        time_avg[j-1].x=time_avg[j].x;
        time_avg[j-1].y=time_avg[j].y;
    }
    time_avg[task->RX[0].size()-1].x=phshift;
    time_avg[task->RX[0].size()-1].y=phshift_avg;
    //phshift/=10000.0;
    /*AOA  plot2.setAxisScale(QwtPlot::yLeft,-4,4);
    plot2.draw("DIFF_ARG",diff,REAL_SHIFT);

    plot4.setAxisScale(QwtPlot::yLeft,-5,5);
    plot4.draw("DIFF_ARG1",time_avg,task->RX[0].size()-1024,task->RX[0].size(),REAL);
    plot4.draw("DIFF_ARG2",time_avg,task->RX[0].size()-1024,task->RX[0].size(),IMAG);


    plot1.setAxisScale(QwtPlot::yLeft,-5,5);
    plot1.draw("DIFF_ARG_AVG",diff_avg,REAL_SHIFT);*/
  //  plot1.draw("DIFF_MOD",diff,IMAG_SHIFT,10);
   /*  plot2.setAxisScale(QwtPlot::yLeft,-3000,3000);
     plot2.draw("TX_REAL",task->TX[0],REAL);
     plot2.draw("TX_ABS",task->TX[0],ABS);
     plot2.draw("TX2_REAL",task->TX[1],REAL);
     plot2.draw("TX2_ABS",task->TX[1],ABS);
     plot2.draw("RX_REAL",task->RX[0],REAL,0.1);
     plot2.draw("RX_ABS",task->RX[0],ABS,0.1);

     //cross correlation

     {
         txsig.setlength(task->TX[0].size());
         rxsig.setlength(task->RX[0].size());
         for(int j =0;j<task->TX[0].size();j++){
             // samples->push_back(QPointF(ts++,10*20*log(std::sqrt(std::pow(out2[j][0],2.0)+std::pow(out2[j][1],2.0)))));
             txsig[j]=alglib::complex(task->TX[0][j].real(),task->TX[0][j].imag());
         }
         for(int j =0;j<task->RX[0].size();j++){
             rxsig[j]=alglib::complex(task->RX[0][j].real(),task->RX[0][j].imag());
         }


         alglib::corrc1d(rxsig,task->RX[0].size(),
                 txsig,task->TX[0].size(),
                 trxcorr);
     }




    plot1.setAxisScale(QwtPlot::yLeft,-1000,1000);
    plot1.draw("CORR_REAL",trxcorr,0,task->RX[0].size(),REAL,1.0/1.0/2048.0/2048.0);
    plot1.draw("CORR_IMAG",trxcorr,0,task->RX[0].size(),IMAG,1.0/1.0/2048.0/2048.0);
    plot1.draw("CORR_ABS",trxcorr,0,task->RX[0].size(),ABS,1.0/1.0/2048.0/2048.0);
    plot1.draw("CORR_LOG",trxcorr,0,task->RX[0].size(),LOG);




    plot4.setAxisScale(QwtPlot::yLeft,0,500);
    plot4.draw("RX_LOG",rxsig,LOG);


    //spectrogram

    plot3.setAxisScale(QwtPlot::yLeft,0,500);
    alglib::fftc1d(rxsig,rxsig.length());
    plot3.draw("RX_LOG",rxsig,LOG);



*/





}

void MainWindow::addTask(){
    static LimeSDRTask testtask;

    const double pi = std::acos(-1);
    const std::complex<double> j(0, 1);
    const std::complex<double> q(0, 0);
    const double u=1;
    double tonef=-.1e6;
    double tonef2=+0.2e6;
    testtask.TX[0].resize(TX_BURST);
    testtask.TX[1].resize(TX_BURST);
   /* for(int i=0;i<TX_BURST/4;i++){
        testtask.TX[0][i]=0;
        testtask.TX[1][i]=0;
    }*/
    double ph=((double)ui->phaseSlider->value()/1000.0);
    double ampl=ui->ampSlider->value()/100.0;
    double absphase=ui->absPhase->value()/1000.0*M_PI;
    double phase=0,freq=0,phase2=0,freq2=0;
    for(int i=0;i<TX_BURST;i++){
        //testtask.TX[0][i]=1.0*C_FULL_SCALE*std::exp(j *2.0* pi*tonef*(((double)i)/limeSDRconf.samplerate));
        //testtask.TX[0][i]=C_FULL_SCALE*std::exp(-j* pi*u*(double)i*((double)i+1.0+2.0*q+TX_BURST/8.0)/((double)TX_BURST*2));
        freq=5.0e6-10.0e6*(double)i/(double)TX_BURST;
        phase+=freq/(double)limeSDRconf.samplerate;
        phase+=(5.0e6-10.0e6*(double)i/(double)TX_BURST)/(double)limeSDRconf.samplerate;


        phase=(double)i*(5.0e6-1.0/2.0*10.0e6*(double)i/(double)TX_BURST)/(double)limeSDRconf.samplerate;

        testtask.TX[0][i]=C_FULL_SCALE*std::exp(-j*2.0*pi*(phase))/2.0;


        freq2=5.0e6-10.0e6*(double)i/(double)TX_BURST;

        phase2=ph+(double)i*(5.0e6-1.0/2.0*10.0e6*(double)i/(double)TX_BURST)/(double)limeSDRconf.samplerate;

        testtask.TX[1][i]=-ampl*C_FULL_SCALE*std::exp(j*absphase-j*2.0*pi*(phase2))/2.0;


        //testtask.TX[1][i]=ampl*1.0*C_FULL_SCALE*std::exp(j *2.0* pi*(ph+tonef*((double)i)/limeSDRconf.samplerate));//C_FULL_SCALE*std::exp(j *2.0* pi*tonef2*((double)i)/SAMPLING);
        //testtask.TX[1][i]=-ampl*C_FULL_SCALE*std::exp(j*absphase-j* pi*u*((double)i*(ph+(double)i+1.0+2.0*q+TX_BURST/8.0)/((double)TX_BURST*2)));
        //testtask.TX[1][i]=0;
        //tx_buff2[i]=std::complex<int16_t>(0,0);
    }
    testtask.TX[0][0]=0;
    testtask.TX[1][0]=0;
    testtask.TX[0][TX_BURST-1]=0;
    testtask.TX[1][TX_BURST-1]=0;

    /*for(int i=TX_BURST/2;i<TX_BURST;i++){
        testtask.TX[0][i]=0;
        testtask.TX[1][i]=0;
    }*/



    testtask.RX[0].resize(RX_BURST);
    testtask.RX[1].resize(RX_BURST);
    //for(int i=0;i<1000;i++)
    lime->worker.calibrating.lock();
        lime->worker.addTask(&testtask);
        lime->worker.calibrating.unlock();
}

void MainWindow::stateMachine(){

    
}


enum state_t{
    init_state,
    
    
};

void MainWindow::resetAvg(){
    resetavg=true;
}

