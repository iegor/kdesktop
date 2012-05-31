/* This file is in the public domain */

// $Id: kvieweffects.h 213095 2003-03-11 15:21:44Z mkretz $

#ifndef KVIEWEFFECTS_H
#define KVIEWEFFECTS_H

#include <kparts/plugin.h>
#include <qcolor.h>

namespace KImageViewer { class Viewer; }

class KViewEffects : public KParts::Plugin
{
	Q_OBJECT
public:
	KViewEffects( QObject* parent, const char* name, const QStringList & );
	virtual ~KViewEffects();

private slots:
	void intensity();
	void setIntensity( int );
	void applyIntensity();

	void blend();
	void setOpacity( int );
	void setColor( const QColor & );
	void applyBlend();

	void gamma();
	void setGammaValue( double );
	void applyGammaCorrection();

private:
	QImage * workImage();

	KImageViewer::Viewer * m_pViewer;
	double m_gamma, m_lastgamma;
	int m_opacity, m_lastopacity;
	int m_intensity, m_lastintensity;
	QColor m_color;
	QImage * m_image;
};

// vim:sw=4:ts=4:cindent
#endif // KVIEWEFFECTS_H
