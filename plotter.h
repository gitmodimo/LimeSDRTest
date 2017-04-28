#ifndef PLOTTER_H
#define PLOTTER_H
#include <QMap>
#include <qwt5/qwt_plot.h>
#include <qwt5/qwt_plot_curve.h>
#include <qwt5/qwt_plot_grid.h>
#include <complex>
#include <fasttransforms.h>
enum PlotterMode{
    REAL,
    IMAG,
    ABS,
    LOG,
    FFT,
    REAL_SHIFT,
    IMAG_SHIFT,
};

struct PlotterCurve{
    QVector<QPointF> samples;
    QwtPlotCurve curve;
};


int getNumel(alglib::complex_1d_array samples);
double getReal(alglib::complex_1d_array samples,int i);
double getImag(alglib::complex_1d_array samples,int i);

int getNumel(std::vector<std::complex<int16_t> > samples);
double getReal(std::vector<std::complex<int16_t> > samples,int i);
double getImag(std::vector<std::complex<int16_t> > samples,int i);





class Plotter : public QWidget
{
    Q_OBJECT
public:
    explicit Plotter(QWidget *parent = 0);
    QwtPlot plot;
    QwtPlotGrid grid;
    QMap<QString,PlotterCurve*> curves;
    QVector<QPen> pens;

    void setAxisScale(int axisId, double min, double max, double step = 0);
    void setAxisAutoScale(int axisId);
    PlotterCurve *PlotterCurveByName(QString name);



    template<typename T>
    void draw(QString name,T samples,int from, int to,PlotterMode mode,double scale=1,double offset=0){
        PlotterCurve *curve=PlotterCurveByName(name);
        curve->samples.clear();
        int numel=getNumel(samples);
        int ctr,ii;
        switch(mode){
        case REAL:
            for(int i=from;i<to;++i)
                curve->samples.push_back(QPointF(i,scale*(offset+getReal(samples,i))));
            break;
        case IMAG:
            for(int i=from;i<to;++i)
                curve->samples.push_back(QPointF(i,scale*(offset+getImag(samples,i))));
            break;
        case ABS:
            for(int i=from;i<to;++i)
                curve->samples.push_back(QPointF(i,scale*(offset+std::sqrt(std::pow(getReal(samples,i),2.0)+std::pow(getImag(samples,i),2.0)))));
            break;
        case LOG:
            for(int i=from;i<to;++i){
                double val=scale*10.0*std::log(offset+std::pow(getReal(samples,i),2.0)+std::pow(getImag(samples,i),2.0));
                if(val<0)val=scale;
                curve->samples.push_back(QPointF(i,val));
            }
            break;
        case FFT:
            ctr=(from+to)/2;
            ii=-ctr;
            for(int i=ctr;i<to;++i){
                double val=scale*10.0*std::log(std::sqrt(offset+std::pow(getReal(samples,i),2.0)+std::pow(getImag(samples,i),2.0)));
                if(val<0)val=scale;
                curve->samples.push_back(QPointF(ii++,val));
            }

            for(int i=from;i<ctr;++i){
                double val=scale*10.0*std::log(std::sqrt(offset+std::pow(getReal(samples,i),2.0)+std::pow(getImag(samples,i),2.0)));
                if(val<0)val=scale;
                curve->samples.push_back(QPointF(ii++,val));
            }
            break;
        case REAL_SHIFT:
            ctr=(from+to)/2;
            ii=-ctr;
            for(int i=ctr;i<to;++i)
                curve->samples.push_back(QPointF(ii++,scale*(offset+getReal(samples,i))));
            for(int i=from;i<ctr;++i)
                curve->samples.push_back(QPointF(ii++,scale*(offset+getReal(samples,i))));
            break;
        case IMAG_SHIFT:
            ctr=(from+to)/2;
            ii=-ctr;
            for(int i=ctr;i<to;++i)
                curve->samples.push_back(QPointF(ii++,scale*(offset+getImag(samples,i))));
            for(int i=from;i<ctr;++i)
                curve->samples.push_back(QPointF(ii++,scale*(offset+getImag(samples,i))));
            break;

        }

        curve->curve.setData(curve->samples);
        updatePlot();
    }
    template<typename T>
    void draw(QString name,T samples,PlotterMode mode,double scale=1,double offset=0){
        draw(name,samples,0,getNumel(samples),mode,scale,offset);
    }

    void addPlot(QString name, PlotterCurve *curve);

    void updatePlot();

signals:

public slots:
};

#endif // PLOTTER_H
