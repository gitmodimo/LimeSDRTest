#ifndef LIMESDR_H
#define LIMESDR_H
#include <string>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Types.hpp>
#include <QWidget>
#include <QThread>
#include <QDebug>
#ifndef Q_DECL_OVERRIDE
    #define Q_DECL_OVERRIDE override
#endif


struct LimeSDRConfig{
    SoapySDR::Kwargs devicesKwargs;
    double samplerate;
    double bw;
    double RXFreq;
    double TXFreq;
    std::string RXAntenna;
    std::string TXAntenna;
    double RXGainLNA;
    double RXGainTIA;
    double RXGainPGA;
    double TXGainPAD;
    bool RXDCOffsetMode;
    bool TXDCOffsetMode;
    std::string RXApplyCorrections;
    std::string TXApplyCorrections;
    size_t buffSize;
};

class LimeSDRWorker : public QThread
{
    Q_OBJECT
public:

    LimeSDRWorker(LimeSDRConfig _config);

    ~LimeSDRWorker();
private:
    void run() Q_DECL_OVERRIDE;
    /*int initialize(const double samplerate,const double bw,const double freq){

    }*/

signals:
    void resultReady(const QString &s);
private:
public:
    LimeSDRConfig config;
    SoapySDR::Device  *sdrDevice;
    SoapySDR::Stream *rxStream;
    SoapySDR::Stream *txStream;

    //RX_buff[channel][sample]
    std::complex<int16_t> *RX_buffs[2];
    std::complex<int16_t> *TX_buffs[2];
};

/*
void LimeSDRWorker::startWorkInAThread()
{
    WorkerThread *workerThread = new WorkerThread(this);
    connect(workerThread, &WorkerThread::resultReady, this, &MyObject::handleResults);
    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}
*/

namespace Ui {
class LimeSDR;
}

class LimeSDR : public QWidget
{
    Q_OBJECT

public:
    explicit LimeSDR(LimeSDRConfig _config, QWidget *parent = 0);
    ~LimeSDR();

private:
    Ui::LimeSDR *ui;
public:
    LimeSDRWorker worker;
};

#endif // LIMESDR_H
