#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Types.hpp>
#include <stdio.h> //printf
#include <qwt5/qwt_plot.h>
#include <qwt5/qwt_plot_curve.h>

#include <qwt5/qwt_plot_grid.h>

#include "limesdr.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QwtPlot *myPlot;
    QVector<QPointF>* samples;
    QVector<QPointF>* samples2;
    QVector<QPointF>* samples3;
    QwtPlotCurve *curve1;
    QwtPlotCurve *curve2;
    QwtPlotCurve *curve3;
    QwtPlotGrid *grid;
public slots:
        void refreshDevicesList();
        void connectDevice();
        void updatefft();
        void resetAvg();
    /*    void setRx0IQ(int percent);
        void setRx1IQ(int percent);
        void setTx0IQ(int percent);
        void setTx1IQ(int percent);*/


private:
        LimeSDRConfig limeSDRconf;
        LimeSDR *lime;
        bool resetavg;
        size_t devicesCount;
        SoapySDR::KwargsList devicesKwargs;
        SoapySDR::Device  *sdrDevice;
        int RX_Count;
        int TX_Count;
        //enumerate devices
        QTimer *timer;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
