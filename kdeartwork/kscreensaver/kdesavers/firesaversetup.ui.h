/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


#ifndef KFIRESAVER_SETUP_H
#define KFIRESAVER_SETUP_H

#include <qcolor.h>
#include <qtimer.h>
#include <kconfig.h>


void KFireSaverSetup::init()
{
    readConfig();
    fireSaverWidget = NULL;
    updatePreview();
}

void KFireSaverSetup::destroy()
{
    if ( fireSaverWidget )
	delete fireSaverWidget;
}


void KFireSaverSetup::applyButton_clicked()
{
    writeConfig();
    updatePreview();
}

void KFireSaverSetup::updatePreview()
{
    if ( fireSaverWidget )
	delete fireSaverWidget;
    if ( detachBox->isChecked() ) {
	previewLabel->show();
	fireSaverWidget = new KFireSaver;
	fireSaverWidget->resize(640,480);
    } else {
	previewLabel->hide();
	fireSaverWidget = new KFireSaver( previewFrame );
	fireSaverWidget->move(1,1);
	fireSaverWidget->resize(
		previewFrame->width() - 2,
		previewFrame->height() - 2 );
    }
    fireSaverWidget->show();
}


void KFireSaverSetup::writeConfig()
{
    KConfig config("kfiresaverrc",false,false);

    // show
    config.setGroup( "Show" );
    config.writeEntry( "ShowType", showCombo->currentItem() );
    config.writeEntry( "FireworksFrequency", fireworksSlider->value() );
    config.writeEntry( "ParticlesSize", particlesSlider->value() );
    config.writeEntry( "enable-BottomFire", bottomfireBox->isChecked() );
      config.writeEntry( "BottomFireColor", bottomfireColorButton->color() );
    config.writeEntry( "enable-Sounds", soundsBox->isChecked() );
    config.writeEntry( "enable-NoOverhead", overheadBox->isChecked() );
    config.writeEntry( "enable-FrameSkip", frameskipBox->isChecked() );

    // fireworks
    config.setGroup( "Fireworks" );
    if (!fire1Box->isChecked() && !fire2Box->isChecked() &&
	!fire3Box->isChecked() && !fire4Box->isChecked() &&
	!fire5Box->isChecked() && !fire6Box->isChecked() &&
	!fire7Box->isChecked() && !fire8Box->isChecked())
	fire1Box->setChecked(true);
    config.writeEntry( "use-Classic", fire1Box->isChecked() );
    config.writeEntry( "use-Explosion", fire2Box->isChecked() );
    config.writeEntry( "use-FlameRing", fire3Box->isChecked() );
    config.writeEntry( "use-FlameWorld", fire4Box->isChecked() );
    config.writeEntry( "use-Fall", fire5Box->isChecked() );
    config.writeEntry( "use-Splitter", fire6Box->isChecked() );
    config.writeEntry( "use-Spirals", fire7Box->isChecked() );
    config.writeEntry( "use-SuperNova", fire8Box->isChecked() );
    if (!redBox->isChecked() && !orangeBox->isChecked() &&
        !greenBox->isChecked() && !blueBox->isChecked() &&
	!whiteBox->isChecked() && !purpleBox->isChecked() &&
	!seagreenBox->isChecked())
	whiteBox->setChecked(true);
    config.writeEntry( "use-White", whiteBox->isChecked() );
    config.writeEntry( "use-Blue", blueBox->isChecked() );
    config.writeEntry( "use-DeepGreen", seagreenBox->isChecked() );
    config.writeEntry( "use-Purple", purpleBox->isChecked() );
    config.writeEntry( "use-Orange", orangeBox->isChecked() );
    config.writeEntry( "use-Red", redBox->isChecked() );
    config.writeEntry( "use-Green", greenBox->isChecked() );
    config.writeEntry( "use-Multicolor", combosBox->isChecked() );

    // specials
    config.setGroup( "Specials" );
    config.writeEntry( "enable-Logos", logosBox->isChecked() );
      config.writeEntry( "LogosKonqui", logosKonquiBox->isChecked() );
      config.writeEntry( "LogosTux", logosTuxBox->isChecked() );
      config.writeEntry( "LogosKDEIcons", logosIconsBox->isChecked() );
      config.writeEntry( "LogosReduceDetail", logosDetailBox->isChecked() );
      config.writeEntry( "LogosFrequency", logosSlider->value() );
    config.writeEntry( "enable-Stars", starsBox->isChecked() );
      config.writeEntry( "StarsFlicker", starsFlickerBox->isChecked() );
      config.writeEntry( "StarsGradient", starsGradientBox->isChecked() );
      config.writeEntry( "StarsNumber", starsSlider->value() );
    config.writeEntry( "enable-Writings", writingsBox->isChecked() );

    // effects
    config.setGroup( "Effects" );
    config.writeEntry( "enable-SphericalLight", lightBox->isChecked() );
    config.writeEntry( "enable-Flash", flashBox->isChecked() );
      config.writeEntry( "FlashOpacity", flashSlider->value() );
    config.writeEntry( "enable-Fade", fadeBox->isChecked() );
      config.writeEntry( "FadeIntensity", fadeSlider->value() );
    config.writeEntry( "enable-Flares", flaresBox->isChecked() );
      config.writeEntry( "FlaresDimension", flaresSlider->value() );
    config.writeEntry( "enable-Trail", trailBox->isChecked() );
}


void KFireSaverSetup::readConfig()
{
    KConfig config("kfiresaverrc",true,false);

    // show
    config.setGroup( "Show" );
    showCombo->setCurrentItem(config.readNumEntry( "ShowType", 1 ));
    fireworksSlider->setValue(config.readNumEntry( "FireworksFrequency", 7 ));
    particlesSlider->setValue(config.readNumEntry( "ParticlesSize", 0 ));
    bottomfireBox->setChecked(config.readBoolEntry( "enable-BottomFire", true ));
    QColor blue = Qt::darkBlue;
    bottomfireColorButton->setColor(config.readColorEntry( "BottomFireColor", &blue ));
    soundsBox->setChecked(config.readBoolEntry( "enable-Sounds", false ));
    overheadBox->setChecked(config.readBoolEntry( "enable-NoOverhead", true ));
    frameskipBox->setChecked(config.readBoolEntry( "enable-FrameSkip", true ));

    // fireworks
    config.setGroup( "Fireworks" );
    fire1Box->setChecked(config.readBoolEntry( "use-Classic", true ));
    fire2Box->setChecked(config.readBoolEntry( "use-Explosion", false ));
    fire3Box->setChecked(config.readBoolEntry( "use-FlameRing", false ));
    fire4Box->setChecked(config.readBoolEntry( "use-FlameWorld", false ));
    fire5Box->setChecked(config.readBoolEntry( "use-Fall", false ));
    fire6Box->setChecked(config.readBoolEntry( "use-Splitter", false ));
    fire7Box->setChecked(config.readBoolEntry( "use-Spirals", false ));
    fire8Box->setChecked(config.readBoolEntry( "use-SuperNova", false ));
    whiteBox->setChecked(config.readBoolEntry( "use-White", true ));
    blueBox->setChecked(config.readBoolEntry( "use-Blue", false ));
    seagreenBox->setChecked(config.readBoolEntry( "use-DeepGreen", true ));
    purpleBox->setChecked(config.readBoolEntry( "use-Purple", false ));
    orangeBox->setChecked(config.readBoolEntry( "use-Orange", true ));
    redBox->setChecked(config.readBoolEntry( "use-Red", false ));
    greenBox->setChecked(config.readBoolEntry( "use-Green", false ));
    combosBox->setChecked(config.readBoolEntry( "use-Multicolor", true ));

    // specials
    config.setGroup( "Specials" );
    logosBox->setChecked(config.readBoolEntry( "enable-Logos", true ));
    logosKonquiBox->setChecked(config.readBoolEntry( "LogosKonqui", true ));
    logosTuxBox->setChecked(config.readBoolEntry( "LogosTux", true ));
    logosIconsBox->setChecked(config.readBoolEntry( "LogosKDEIcons", true ));
    logosDetailBox->setChecked(config.readBoolEntry( "LogosReduceDetail", true ));
    logosSlider->setValue(config.readNumEntry( "LogosFrequency", 4 ));
    starsBox->setChecked(config.readBoolEntry( "enable-Stars", true ));
    starsFlickerBox->setChecked(config.readBoolEntry( "StarsFlicker", false ));
    starsGradientBox->setChecked(config.readBoolEntry( "StarsGradient", true ));
    starsSlider->setValue(config.readNumEntry( "StarsNumber", 4 ));
    writingsBox->setChecked(config.readBoolEntry( "enable-Writings", true ));

    // effects
    config.setGroup( "Effects" );
    lightBox->setChecked(config.readBoolEntry( "enable-SphericalLight", true ));
    flashBox->setChecked(config.readBoolEntry( "enable-Flash", false ));
    flashSlider->setValue(config.readNumEntry( "FlashOpacity", 5 ));
    fadeBox->setChecked(config.readBoolEntry( "enable-Fade", false ));
    fadeSlider->setValue(config.readNumEntry( "FadeIntensity", 3 ));
    flaresBox->setChecked(config.readBoolEntry( "enable-Flares", true ));
    flaresSlider->setValue(config.readNumEntry( "FlaresDimension", 5 ));
    trailBox->setChecked(config.readBoolEntry( "enable-Trail", false ));
}

void KFireSaverSetup::useButton_clicked()
{
    int ci = presetCombo->currentItem();

    showCombo->setCurrentItem( 0 );
    particlesSlider->setValue( 0 );
    overheadBox->setChecked( true );
    flashBox->setChecked( false );
    //flashSlider->setValue( );
    //soundsBox->setChecked( );
    //logosDetailBox->setChecked( );

    switch (ci){
    case 0: // KDE default
	showCombo->setCurrentItem( 1 );
	fireworksSlider->setValue( 7 );
	bottomfireBox->setChecked( true );
	bottomfireColorButton->setColor( Qt::darkBlue );
	frameskipBox->setChecked( true );
	fire1Box->setChecked( true );
	fire2Box->setChecked( false );
	fire3Box->setChecked( false );
	fire4Box->setChecked( false );
	fire5Box->setChecked( false );
	fire6Box->setChecked( false );
	fire7Box->setChecked( false );
	fire8Box->setChecked( false );
	whiteBox->setChecked( true );
	blueBox->setChecked( false );
	seagreenBox->setChecked( true );
	purpleBox->setChecked( false );
	orangeBox->setChecked( true );
	redBox->setChecked( false );
	greenBox->setChecked( false );
	combosBox->setChecked( true );
	logosBox->setChecked( true );
	logosKonquiBox->setChecked( true );
	logosTuxBox->setChecked( true );
	logosIconsBox->setChecked( true );
	logosSlider->setValue( 4 );
	starsBox->setChecked( true );
	starsFlickerBox->setChecked( false );
	starsGradientBox->setChecked( true );
	starsSlider->setValue( 4 );
	writingsBox->setChecked( true );
	lightBox->setChecked( true );
	fadeBox->setChecked( false );
	//fadeSlider->setValue( );
	flaresBox->setChecked( true );
	flaresSlider->setValue( 5 );
	trailBox->setChecked( false );
	break;
    case 1:  // Elegant white
	fireworksSlider->setValue( 7 );
	bottomfireBox->setChecked( true );
	bottomfireColorButton->setColor( Qt::blue );
	frameskipBox->setChecked( true );
	fire1Box->setChecked( true );
	fire2Box->setChecked( false );
	fire3Box->setChecked( false );
	fire4Box->setChecked( false );
	fire5Box->setChecked( true );
	fire6Box->setChecked( true );
	fire7Box->setChecked( false );
	fire8Box->setChecked( false );
	whiteBox->setChecked( true );
	blueBox->setChecked( true );
	seagreenBox->setChecked( false );
	purpleBox->setChecked( false );
	orangeBox->setChecked( false );
	redBox->setChecked( false );
	greenBox->setChecked( false );
	combosBox->setChecked( true );
	logosBox->setChecked( true );
	logosKonquiBox->setChecked( true );
	logosTuxBox->setChecked( false );
	logosIconsBox->setChecked( false );
	logosSlider->setValue( 3 );
	starsBox->setChecked( true );
	starsFlickerBox->setChecked( true );
	starsGradientBox->setChecked( false );
	starsSlider->setValue( 4 );
	writingsBox->setChecked( true );
	lightBox->setChecked( true );
	fadeBox->setChecked( true );
	fadeSlider->setValue( 3 );
	flaresBox->setChecked( true );
	flaresSlider->setValue( 6 );
	trailBox->setChecked( true );
	break;
    case 2:  // Quick simple
	showCombo->setCurrentItem(1);
	fireworksSlider->setValue( 4 );
	bottomfireBox->setChecked( false );
	//bottomfireColorButton->setColor( );
	frameskipBox->setChecked( false );
	fire1Box->setChecked( true );
	fire2Box->setChecked( true );
	fire3Box->setChecked( false );
	fire4Box->setChecked( false );
	fire5Box->setChecked( false );
	fire6Box->setChecked( false );
	fire7Box->setChecked( false );
	fire8Box->setChecked( false );
	whiteBox->setChecked( true );
	blueBox->setChecked( true );
	seagreenBox->setChecked( false );
	purpleBox->setChecked( false );
	orangeBox->setChecked( false );
	redBox->setChecked( true );
	greenBox->setChecked( true );
	combosBox->setChecked( false );
	logosBox->setChecked( true );
	logosKonquiBox->setChecked( true );
	logosTuxBox->setChecked( true );
	logosIconsBox->setChecked( true );
	logosSlider->setValue( 2 );
	starsBox->setChecked( false );
	//starsFlickerBox->setChecked( );
	//starsGradientBox->setChecked( );
	//starsSlider->setValue( );
	writingsBox->setChecked( false );
	lightBox->setChecked( false );
	fadeBox->setChecked( false );
	//fadeSlider->setValue( );
	flaresBox->setChecked( false );
	//flaresSlider->setValue( );
	trailBox->setChecked( false );
	break;
    case 3: //Enhanced reality
	fireworksSlider->setValue( 8 );
	bottomfireBox->setChecked( true );
	bottomfireColorButton->setColor( Qt::magenta );
	frameskipBox->setChecked( true );
	fire1Box->setChecked( true );
	fire2Box->setChecked( true );
	fire3Box->setChecked( true );
	fire4Box->setChecked( true );
	fire5Box->setChecked( true );
	fire6Box->setChecked( true );
	fire7Box->setChecked( false );
	fire8Box->setChecked( false );
	whiteBox->setChecked( true );
	blueBox->setChecked( true );
	seagreenBox->setChecked( true );
	purpleBox->setChecked( true );
	orangeBox->setChecked( true );
	redBox->setChecked( true );
	greenBox->setChecked( true );
	combosBox->setChecked( true );
	logosBox->setChecked( true );
	logosKonquiBox->setChecked( false );
	logosTuxBox->setChecked( true );
	logosIconsBox->setChecked( false );
	logosSlider->setValue( 2 );
	starsBox->setChecked( true );
	starsFlickerBox->setChecked( true );
	starsGradientBox->setChecked( true );
	starsSlider->setValue( 5 );
	writingsBox->setChecked( true );
	lightBox->setChecked( false );
	fadeBox->setChecked( true );
	fadeSlider->setValue( 3 );
	flaresBox->setChecked( false );
	//flaresSlider->setValue( );
	trailBox->setChecked( true );
	break;
    case 4: //Hypnotic illusions
	fireworksSlider->setValue( 9 );
	bottomfireBox->setChecked( true );
	bottomfireColorButton->setColor( Qt::red );
	frameskipBox->setChecked( true );
	fire1Box->setChecked( true );
	fire2Box->setChecked( true );
	fire3Box->setChecked( true );
	fire4Box->setChecked( true );
	fire5Box->setChecked( true );
	fire6Box->setChecked( true );
	fire7Box->setChecked( false );
	fire8Box->setChecked( false );
	whiteBox->setChecked( true );
	blueBox->setChecked( true );
	seagreenBox->setChecked( true );
	purpleBox->setChecked( true );
	orangeBox->setChecked( true );
	redBox->setChecked( true );
	greenBox->setChecked( true );
	combosBox->setChecked( true );
	logosBox->setChecked( false );
	//logosKonquiBox->setChecked( true );
	//logosTuxBox->setChecked( true );
	//logosIconsBox->setChecked( true );
	//logosSlider->setValue( 5 );
	starsBox->setChecked( true );
	starsFlickerBox->setChecked( true );
	starsGradientBox->setChecked( true );
	starsSlider->setValue( 8 );
	writingsBox->setChecked( false );
	lightBox->setChecked( false );
	fadeBox->setChecked( true );
	fadeSlider->setValue( 7 );
	flaresBox->setChecked( true );
	flaresSlider->setValue( 8 );
	trailBox->setChecked( false );
	break;
    }
    //applyButton_clicked();
}

#endif
