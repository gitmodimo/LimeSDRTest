#include "plotter.h"


int getNumel(alglib::complex_1d_array samples){
    return samples.length();
}

double getReal(alglib::complex_1d_array samples,int i){
    return samples[i].x;
}
double getImag(alglib::complex_1d_array samples,int i){
    return samples[i].y;
}

int getNumel(std::vector<std::complex<int16_t> > samples){
    return samples.size();
}

double getReal(std::vector<std::complex<int16_t> > samples,int i){
    return samples[i].real();
}
double getImag(std::vector<std::complex<int16_t> > samples,int i){
    return samples[i].imag();
}



Plotter::Plotter(QWidget *parent) : QWidget(parent)
{
    pens.append(QPen(QColor(255,0,0)));
    pens.append(QPen(QColor(0,255,0)));
    pens.append(QPen(QColor(0,0,255)));
    pens.append(QPen(QColor(255,255,0)));
    pens.append(QPen(QColor(0,255,255)));
    pens.append(QPen(QColor(255,0,255)));
    plot.show();
}

void Plotter::setAxisScale(int axisId, double min, double max, double step){
    plot.setAxisScale(axisId, min, max, step );
}

void Plotter::setAxisAutoScale(int axisId){
    plot.setAxisAutoScale(axisId);
}

PlotterCurve *Plotter::PlotterCurveByName(QString name){
    PlotterCurve *curve;
    if(!curves.contains(name)){
        curve=new PlotterCurve();
        curve->curve.setPen(pens.first());
        pens.push_back(pens.front());
        pens.pop_front();
        curve->curve.setTitle(name);
        curves[name]=curve;
        curve->curve.attach(&plot);
    }
    else{
        curve=curves[name];
    }
    return curve;
}

void Plotter::addPlot(QString name, PlotterCurve *curve){
    //
    curves[name]=curve;
}

void Plotter::updatePlot(){
    plot.replot();
}
