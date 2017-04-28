#ifndef LIMESDR_H
#define LIMESDR_H
#include <string>
#include <mutex>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <QWidget>
#include <QThread>
#include <QDebug>
#ifndef Q_DECL_OVERRIDE
    #define Q_DECL_OVERRIDE override
#endif

struct LimeSDRTask{
    std::vector<std::complex<int16_t> > TX[2];
    std::vector<std::complex<int16_t> > RX[2];
    long long timestamp;
};


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

public:

public slots:
    void setLNA(int gain);
    void setTIA(int gain);
    void setPGA(int gain);
    void setPAD(int gain);
    void setRxAntenna(QString ant){
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

signals:
    void resultReady(const QString &s);
    void taskDone();
private:
public:
    void addTask(LimeSDRTask* task);
    LimeSDRTask* takeTask();
    void finishTask(LimeSDRTask* task);

    LimeSDRTask* takeFinishedTask();


    LimeSDRConfig config;
    SoapySDR::Device  *sdrDevice;
    SoapySDR::Stream *rxStream;
    SoapySDR::Stream *txStream;

    //RX_buff[channel][sample]
    std::complex<int16_t> *RX_buffs[2];
    std::complex<int16_t> *TX_buffs[2];
    std::list<LimeSDRTask*> taskList;
    std::list<LimeSDRTask*> taskDoneList;
    std::mutex list_mutex;
    std::mutex doneList_mutex;
};


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
