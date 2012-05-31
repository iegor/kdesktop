#include "artseffects.h"
#include "stdsynthmodule.h"
#include <qwidget.h>

class QSlider;

namespace Arts {

class ExtraStereoGUI_impl : public QWidget, virtual public ExtraStereoGUI_skel
{
public:
	ExtraStereo effect;

	ExtraStereoGUI_impl();

	void setEffect(StereoEffect newEffect);

public slots:
	void changeSlider(int);

private:
	QSlider *mSlider;
};


};
