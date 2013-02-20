#ifndef __FLUXSS_H__
#define __FLUXSS_H__
//============================================================================
//
// Terence Welsh Screensaver - Flux
// http://www.reallyslick.com/
//
// Ported to KDE by Karl Robillard
//
//============================================================================


#include <qgl.h>


#define LIGHTSIZE   64


class flux;
class particle;
class QTimer;

class FluxWidget : public QGLWidget
{
    Q_OBJECT

public:

    enum eDefault
    {
        Regular,
        Hypnotic,
        Insane,
        Sparklers,
        Paradigm,
        Galactic,
        DefaultModes
    };

    FluxWidget( QWidget* parent=0, const char* name=0 );
    ~FluxWidget();

    void setDefaults( int which );
    void updateParameters();

protected:

    void paintGL();
    void resizeGL( int w, int h );
    void initializeGL();
#ifdef UNIT_TEST
    void keyPressEvent( QKeyEvent* );
#endif

private slots:

    void nextFrame();

private:

    float lumdiff;
    float cameraAngle;
    float cosCameraAngle, sinCameraAngle;
    unsigned char lightTexture[LIGHTSIZE][LIGHTSIZE];

    int dFluxes;
    int dParticles;
    int dTrail;
    int dGeometry;
    float dSize;
    int dComplexity;
    int dRandomize;
    int dExpansion;
    int dRotation;
    int dWind;
    float dInstability;
    int dBlur;
    int dSmart;
    int dPriority;

    flux* _fluxes;

    // Using QTimer rather than timerEvent() to avoid getting locked out of
    // the QEvent loop on lower-end systems.  Ian Geiser <geiseri@kde.org>
    // says this is the way to go.
    QTimer* _timer;
    int _frameTime;

    friend class flux;
    friend class particle;
};


#ifndef UNIT_TEST
#include <kdialogbase.h>
#include <kscreensaver.h>


class KFluxScreenSaver : public KScreenSaver
{
    Q_OBJECT

public:

    KFluxScreenSaver( WId id );
    virtual ~KFluxScreenSaver();

    int mode() const { return _mode; }

public slots:

    void setMode( int );

private:

    void readSettings();

    FluxWidget* _flux;
    int _mode;
};


class QComboBox;

class KFluxSetup : public KDialogBase
{
    Q_OBJECT

public:

    KFluxSetup( QWidget* parent = 0, const char* name = 0 );
    ~KFluxSetup();

private slots:

    void slotHelp();
    void slotOk();

private:

    QComboBox* modeW;
    KFluxScreenSaver* _saver;
};
#endif


#endif //__FLUXSS_H__
