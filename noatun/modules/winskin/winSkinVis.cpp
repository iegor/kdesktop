/*
  noatun visualisation interface for winskin
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "winSkinVis.h"

#define __BANDS     75
#define __SPAHEIGHT 15

WinSkinVis::WinSkinVis(QObject *parent, const char *name):
    QObject(parent,name),Visualization(50) {

    m_currentPeaks=new float[__BANDS];

    for(unsigned int i=0;i<__BANDS;i++) 
        m_currentPeaks[i]=0.0;

    // If we can create our server-side object, visualize away
    if (initServerObject())
    {
        start();
    }
}


WinSkinVis::~WinSkinVis()
{
    if (m_winSkinFFT != NULL) {
        if (connected())
        {
            visualizationStack().remove(m_id);
            m_winSkinFFT->stop();
            delete m_winSkinFFT;
        }
    }

    delete[] m_currentPeaks;
}


bool WinSkinVis::initServerObject()
{
    // Create FFT on server
    m_winSkinFFT = new Noatun::WinSkinFFT();
    *m_winSkinFFT = Arts::DynamicCast(server()->createObject("Noatun::WinSkinFFT"));
  
    if ( (*m_winSkinFFT).isNull() ) {
        delete m_winSkinFFT;
        m_winSkinFFT=NULL;
    } 
    else
    {
       m_winSkinFFT->bandResolution(__BANDS);
       m_winSkinFFT->start();
       m_id=visualizationStack().insertBottom(*m_winSkinFFT, "WinSkin FFT");
    }

    return (m_winSkinFFT != NULL);
}

void WinSkinVis::timeout()
{
    std::vector<float> *data(m_winSkinFFT->scope());
    
    float *f=&data->front();
    if (data->size())
        scopeEvent(f, data->size());

    delete data;
}

float* WinSkinVis::currentPeaks()
{
    return m_currentPeaks;
}

void WinSkinVis::scopeEvent(float* bandPtr, unsigned int bands)
{
    for (unsigned int i = 0;i < bands;i++) {
        float value=bandPtr[i];
        // if the peak is less we prefer the higher one
        if (m_currentPeaks[i] < value)
            m_currentPeaks[i] = value;
        else
            m_currentPeaks[i] = m_currentPeaks[i]-1.3;

        if (m_currentPeaks[i] < 0.0)
            m_currentPeaks[i] = 0.0;

        if (m_currentPeaks[i] > __SPAHEIGHT)
            m_currentPeaks[i]=__SPAHEIGHT;
    }
    emit(doRepaint());
}


#include "winSkinVis.moc"
