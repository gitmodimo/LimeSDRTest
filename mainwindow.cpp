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
                connect(timer, SIGNAL(timeout()),SLOT(updatefft()));

    myPlot = new QwtPlot();
    myPlot->setAxisScale(QwtPlot::yLeft,-180,180);
    curve1 = new QwtPlotCurve("Curve 1");
    curve1->setPen(QPen(QColor(255,0,0)));

    //QwtPointSeriesData* myData = new QwtPointSeriesData;

    samples = new QVector<QPointF>;
    samples->push_back(QPointF(1.0,1.0));
    samples->push_back(QPointF(2.0,2.0));
    samples->push_back(QPointF(3.0,3.0));
    samples->push_back(QPointF(4.0,5.0));

    curve1->setData(*samples);

    curve1->attach(myPlot);
    samples2 = new QVector<QPointF>;
    samples2->push_back(QPointF(5.0,3.0));
    samples2->push_back(QPointF(6.0,5.0));
    curve2 = new QwtPlotCurve("Curve 2");
    curve2->setPen(QPen(QColor(0,255,0)));
    curve2->setData(*samples2);
    curve2->attach(myPlot);


    curve3 = new QwtPlotCurve("Curve 3");
    curve3->setPen(QPen(QColor(0,0,255)));
    samples3 = new QVector<QPointF>;
    samples3->push_back(QPointF(7.0,3.0));
    samples3->push_back(QPointF(8.0,5.0));
    curve3->setData(*samples3);

    grid=new QwtPlotGrid();
    grid->attach(myPlot);
    curve3->attach(myPlot);
    myPlot->replot();
    myPlot->show();
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

    ///////////////////////////
    limeSDRconf.samplerate=SAMPLING;
    limeSDRconf.bw=40e6;
    limeSDRconf.RXFreq=1880.0e6;//10.0e6+2462.0e6;//1880.0e6;//90e6;//2457.0e6;10.0e6+2462.0e6;//
    limeSDRconf.TXFreq=10e6+1880.0e6;//10.0e6+2462.0e6;//1880.0e6;//90e6;//2457.0e6;10.0e6+2462.0e6;//
    limeSDRconf.RXAntenna="LNAW";
    limeSDRconf.TXAntenna="BAND1";
    limeSDRconf.RXGainLNA=10;
    limeSDRconf.RXGainTIA=0;
    limeSDRconf.RXGainPGA=19-10;
    limeSDRconf.TXGainPAD=0;
    limeSDRconf.RXDCOffsetMode=true;
    limeSDRconf.TXDCOffsetMode=true;
    limeSDRconf.RXApplyCorrections="1";
    limeSDRconf.TXApplyCorrections="1";
    limeSDRconf.buffSize=NUMFFT;


    lime=new LimeSDR(limeSDRconf);
    lime->worker.start();
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





    //updatefft();
    //timer->start(2);



}

void MainWindow::updatefft(){
    float maxpower;
    static int init=0;
    //setup a stream (complex floats){
    static SoapySDR::Stream *rxStream;
    static SoapySDR::Stream *txStream;
    if(!init){
        init=1;
        try{
            rxStream=sdrDevice->setupStream(SOAPY_SDR_RX,SOAPY_SDR_CS16,{0,1},{});
            qDebug()<<"RX mtu:"<<sdrDevice->getStreamMTU(rxStream);
            txStream=sdrDevice->setupStream(SOAPY_SDR_TX,SOAPY_SDR_CS16,{0,1},{});
            qDebug()<<"TX mtu:"<<sdrDevice->getStreamMTU(txStream);
        } catch (const std::exception& e) {
            qDebug()<<e.what();
        }
        //SoapySDRStream *rxStream;

        /*if (SoapySDRDevice_setupStream(sdrDevice, &rxStream, SOAPY_SDR_RX, SOAPY_SDR_CF32, NULL, 0, NULL) != 0)
        {
            //printf("setupStream fail: %s\n", SoapySDRDevice_lastError());
        }*/

        sdrDevice->activateStream(rxStream,0,0,0);
        sdrDevice->activateStream(txStream,0,0,0);
    }
    //SoapySDRDevice_activateStream(sdrDevice, rxStream, 0, 0, 0); //start streaming

    //create a re-usable buffer for rx samples
    int N=NUMFFT;
    std::complex<int16_t> buff[NUMFFT];
    std::complex<int16_t> buff2[NUMFFT];
    std::complex<int16_t> tx_buff[NUMFFT];
    std::complex<int16_t> tx_buff2[NUMFFT];

    const double pi = std::acos(-1);
    const std::complex<double> j(0, 1);
    double tonef=-3e6;
    double tonef2=1e6;
    for(int i=0;i<NUMFFT;i++){
        tx_buff[i]=C_FULL_SCALE*std::exp(j *2.0* pi*tonef*((double)i)/SAMPLING);
        tx_buff2[i]=C_FULL_SCALE*std::exp(j *2.0* pi*tonef2*((double)i)/SAMPLING);
        //tx_buff2[i]=std::complex<int16_t>(0,0);
    }

    static QVector<double> anglediff(NUMFFT);
    static QVector<double> weight(NUMFFT);
    //receive some samples
    samples->clear();
    samples2->clear();
    samples3->clear();
    int ts=0;
    fftw_complex *in, *out;
    fftw_plan p;
    fftw_complex *in2, *out2;
    fftw_plan p2;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    in2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p2 = fftw_plan_dft_1d(N, in2, out2, FFTW_FORWARD, FFTW_ESTIMATE);
    for (size_t i = 0; i < 1; i++)
    {
        samples->clear();
        samples2->clear();
        samples3->clear();
 ts=0;
        void *tx_buffs[] = {tx_buff,tx_buff2}; //array of buffers
        void *buffs[] = {buff,buff2}; //array of buffers
        int flags; //flags set by receive operation
        long long timeNs,ptimeNs; //timestamp for receive buffer
        long long  txTime0 = sdrDevice->getHardwareTime() + (0.001e9) ;//100ms
        int ret;


        for(int j=0;j<10;j++){
            ret=sdrDevice->writeStream(txStream,tx_buffs,N,flags,txTime0+j*N*20,1000000);
            qDebug()<<"TX ret="<<ret<<", flags="<<flags<<"timeNs="<<txTime0+j*N*20;
        }
        do{
         ret=sdrDevice->readStream(rxStream,buffs,N,flags,timeNs,1000000);

         ptimeNs=timeNs;

        }while(timeNs<txTime0);
        qDebug()<<"RX ret="<<ret<<", flags="<<flags<<"timeNs="<<timeNs<<"dtimeNs="<<timeNs-txTime0;
        //int ret = SoapySDRDevice_readStream(sdrDevice, rxStream, buffs, N, &flags, &timeNs, 100000);



            for(int j=0;j<N;j++){
                in[j][0]=buff[j].real()/C_FULL_SCALE*tukeyWindow(j,N,0.05);
                in[j][1]=buff[j].imag()/C_FULL_SCALE*tukeyWindow(j,N,0.05);
            }

            fftw_execute(p); /* repeat as needed */

ts=0;
        for(int j =0;j<N;j++){
        samples->push_back(QPointF(ts++,20*log(sqrt(pow(out[(j+N/2)%N][0],2)+pow(out[(j+N/2)%N][1],2)))));
        //samples->push_back(QPointF(ts++,10*(sqrt(pow(in[j][0],2)+pow(in[j][1],2)))));
        }


        for(int j=0;j<N;j++){
            in2[j][0]=buff2[j].real()/C_FULL_SCALE*tukeyWindow(j,N,0.05);
            in2[j][1]=buff2[j].imag()/C_FULL_SCALE*tukeyWindow(j,N,0.05);
        }

        fftw_execute(p2); /* repeat as needed */
ts=0;

        for(int j =0;j<N;j++){
        samples2->push_back(QPointF(ts++,20*log(sqrt(pow(out2[(j+N/2)%N][0],2)+pow(out2[(j+N/2)%N][1],2)))));
        }

        ///////////////////
        /// \brief parg
        ///

        if(resetavg){
            resetavg=false;
            for(int j =0;j<N;j++){
                anglediff[j]=0;
                weight[j]=0;
            }
        }


        ts=0;
        float parg[100]={0};
         maxpower=0;
        for(int j =0;j<N;j++){
            std::complex<float> ch1,ch2,dif;
            float arg,diffarg;
            ch1=std::complex<float>(out[(j+N/2)%N][0],out[(j+N/2)%N][1]);
            ch2=std::complex<float>(out2[(j+N/2)%N][0],out2[(j+N/2)%N][1]);
            dif=ch1/ch2;
            arg=std::arg(dif);
            diffarg=(arg-parg[99-NUMFFT/100]);
            if(diffarg>M_PI)
                diffarg-=2*M_PI;
            if(diffarg<-M_PI)
                diffarg+=2*M_PI;
            float power=std::max(20*log(sqrt(pow(out[(j+N/2)%N][0],2)+pow(out[(j+N/2)%N][1],2))),
                    20*log(sqrt(pow(out2[(j+N/2)%N][0],2)+pow(out2[(j+N/2)%N][1],2))));
            if(power<-30){
                diffarg=0;
               // anglediff[j]=0;
               // weight[j]=0;
            }else{
                 anglediff[j]=0;
                 weight[j]=0;
                anglediff[j]+=diffarg*pow(10,power/20.0);
                weight[j]+=pow(10,power/20.0);
                maxpower=std::max((double)maxpower,weight[j]);
                if(weight[j]>1000){
                    weight[j]/=2;
                    anglediff[j]/=2;
                }
            }
            samples3->push_back(QPointF(ts++,anglediff[j]/weight[j]/*diffarg*/*10.0*1.0*1.0-100));

            for(int k=0;k<100-1;k++){
                parg[k]=parg[k+1];
            }
            parg[99]=arg;
        }



    }
    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
    qDebug()<<maxpower;
    //shutdown the stream
   // sdrDevice->deactivateStream(rxStream,0,0);
   // sdrDevice->closeStream(rxStream);
    //sdrDevice->deactivateStream(txStream,0,0);
  //  sdrDevice->closeStream(txStream);
//
   // SoapySDRDevice_deactivateStream(sdrDevice, rxStream, 0, 0); //stop streaming
   // SoapySDRDevice_closeStream(sdrDevice, rxStream);

    //cleanup device handle
   // SoapySDRDevice_unmake(sdrDevice);



    curve1->setData(*samples);
    curve2->setData(*samples2);
    curve3->setData(*samples3);
    myPlot->replot();

}

void MainWindow::resetAvg(){
    resetavg=true;
}
/*
void MainWindow::setRx0IQ(int percent){
    //sdrDevice->setIQBalance(SOAPY_SDR_RX,0,(double));
}

void MainWindow::setRx1IQ(int percent){

}

void MainWindow::setTx0IQ(int percent){

}

void MainWindow::setTx1IQ(int percent){

}
*/
