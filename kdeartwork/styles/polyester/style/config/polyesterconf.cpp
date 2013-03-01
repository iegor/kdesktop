/*
based on the Keramik and Plastik configuration dialog:

Plastik:
Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>

Keramik:
Copyright (c) 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qspinbox.h>
#include <qsettings.h>
#include <qcolor.h>
#include <qgroupbox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kdemacros.h>

#include "polyesterconf.h"
#include "configdialog.h"

extern "C"
{
	KDE_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
	{
		KGlobal::locale()->insertCatalogue("polyester");
		return new PolyesterStyleConfig(parent);
	}
}

PolyesterStyleConfig::PolyesterStyleConfig(QWidget* parent): QWidget(parent)
{

	//Should have no margins here, the dialog provides them
	QVBoxLayout* layout = new QVBoxLayout(this, 0, 0);
	dialog_ = new ConfigDialog(this);
	KGlobal::locale()->insertCatalogue("kstyle_polyester_config");


	layout->addWidget(dialog_);
	dialog_->show();

	QSettings s;
	origScrollBarLines = s.readBoolEntry("/polyesterstyle/Settings/scrollBarLines", false);
	dialog_->scrollBarLines->setChecked(origScrollBarLines);
	origColoredScrollBar = s.readBoolEntry("/polyesterstyle/Settings/coloredScrollBar", true);
	dialog_->coloredScrollBar->setChecked(origColoredScrollBar);
	origScrollBarStyle = s.readEntry("/polyesterstyle/Settings/scrollBarStyle", "ThreeButtonScrollBar");
	dialog_->scrollBarStyle->setCurrentItem(scrollBarItem(origScrollBarStyle));
	origScrollBarSize = s.readNumEntry("/polyesterstyle/Settings/scrollBarSize", 16);
	dialog_->scrollBarSize->setValue(origScrollBarSize);
	origCenteredTabBar = s.readBoolEntry("/polyesterstyle/Settings/centeredTabBar", false);
        dialog_->centeredTabBar->setChecked(origCenteredTabBar);
	origHighLightTab = s.readBoolEntry("/polyesterstyle/Settings/highLightTab", true);
	dialog_->highLightTab->setChecked(origHighLightTab);
	origStatusBarFrame = s.readBoolEntry("/polyesterstyle/Settings/statusBarFrame", true);
	dialog_->statusBarFrame->setChecked(origStatusBarFrame);

        //MENUS
	origMenuItemSpacing = s.readNumEntry("/polyesterstyle/Settings/menuItemSpacing", 8);
	dialog_->menuItemSpacing->setValue(origMenuItemSpacing);
	origButtonMenuItem = s.readBoolEntry("/polyesterstyle/Settings/buttonMenuItem", true);
	dialog_->buttonMenuItem->setChecked(origButtonMenuItem);
	origMenuBarEmphasis = s.readBoolEntry("/polyesterstyle/Settings/menuBarEmphasis", false);
	dialog_->menuBarEmphasis->setChecked(origMenuBarEmphasis);
	origMenuBarEmphasisBorder = s.readBoolEntry("/polyesterstyle/Settings/menuBarEmphasisBorder", true);
	dialog_->menuBarEmphasisBorder->setChecked(origMenuBarEmphasisBorder);
	origCustomMenuBarEmphasisColor = s.readBoolEntry("/polyesterstyle/Settings/customMenuBarEmphasisColor", false);
	dialog_->customMenuBarEmphasisColor->setChecked(origCustomMenuBarEmphasisColor);

	origMenuBarEmphasisColor = s.readEntry("/polyesterstyle/Settings/menuBarEmphasisColor", "black");
	dialog_->menuBarEmphasisColor->setColor(origMenuBarEmphasisColor);

	origMenuStripe = s.readBoolEntry("/polyesterstyle/Settings/menuStripe", true);
	dialog_->menuStripe->setChecked(origMenuStripe);

	origShadowedButtonsText = s.readBoolEntry("/polyesterstyle/Settings/shadowedButtonsText", true);
	dialog_->shadowedButtonsText->setChecked(origShadowedButtonsText);
        origShadowedMenuBarText = s.readBoolEntry("/polyesterstyle/Settings/shadowedMenuBarText", true);
        dialog_->shadowedMenuBarText->setChecked(origShadowedMenuBarText);

        //MISC
        origColorizeSortedHeader = s.readBoolEntry("/polyesterstyle/Settings/colorizeSortedHeader", true);
        dialog_->colorizeSortedHeader->setChecked(origColorizeSortedHeader);
	origUseLowerCaseText = s.readBoolEntry("/polyesterstyle/Settings/useLowerCaseText", false);
	dialog_->useLowerCaseText->setChecked(origUseLowerCaseText);
	origAnimProgressBar = s.readBoolEntry("/polyesterstyle/Settings/animateProgressBar", false);
	dialog_->animateProgressBar->setChecked(origAnimProgressBar);
	origLightBorder = s.readBoolEntry("/polyesterstyle/Settings/lightBorder", true);
	dialog_->lightBorder->setChecked(origLightBorder);
	origAnimButton = s.readBoolEntry("/polyesterstyle/Settings/animateButton", false);
	dialog_->animateButton->setChecked(origAnimButton);
	origAnimButtonToDark = s.readBoolEntry("/polyesterstyle/Settings/animateButtonToDark", false);
	dialog_->animateButtonToDark->setChecked(origAnimButtonToDark);
	origDrawToolBarSeparator = s.readBoolEntry("/polyesterstyle/Settings/drawToolBarSeparator", true);
	dialog_->drawToolBarSeparator->setChecked(origDrawToolBarSeparator);
	origDrawToolBarItemSeparator = s.readBoolEntry("/polyesterstyle/Settings/drawToolBarItemSeparator", true);
	dialog_->drawToolBarItemSeparator->setChecked(origDrawToolBarItemSeparator);
//	origDrawFocusRect = s.readBoolEntry("/polyesterstyle/Settings/drawFocusRect", true);
//	drawFocusRect->setChecked(origDrawFocusRect);
	origDrawTriangularExpander = s.readBoolEntry("/polyesterstyle/Settings/drawTriangularExpander", false);
	dialog_->drawTriangularExpander->setChecked(origDrawTriangularExpander);
	origInputFocusHighlight = s.readBoolEntry("/polyesterstyle/Settings/inputFocusHighlight", true);
	dialog_->inputFocusHighlight->setChecked(origInputFocusHighlight);
	origButtonStyle = s.readEntry("/polyesterstyle/Settings/buttonStyle", "glass");
	dialog_->buttonStyle->setCurrentItem(buttonItem(origButtonStyle));
	origCustomOverHighlightColor = s.readBoolEntry("/polyesterstyle/Settings/customOverHighlightColor", false);
	dialog_->customOverHighlightColor->setChecked(origCustomOverHighlightColor);
	origOverHighlightColor = s.readEntry("/polyesterstyle/Settings/overHighlightColor", "black");
	dialog_->overHighlightColor->setColor(origOverHighlightColor);
	origCustomFocusHighlightColor = s.readBoolEntry("/polyesterstyle/Settings/customFocusHighlightColor", false);
	dialog_->customFocusHighlightColor->setChecked(origCustomFocusHighlightColor);
	origFocusHighlightColor = s.readEntry("/polyesterstyle/Settings/focusHighlightColor", "black");
	dialog_->focusHighlightColor->setColor(origFocusHighlightColor);
	origCustomCheckMarkColor = s.readBoolEntry("/polyesterstyle/Settings/customCheckMarkColor", false);
	dialog_->customCheckMarkColor->setChecked(origCustomCheckMarkColor);
	origCheckMarkColor = s.readEntry("/polyesterstyle/Settings/checkMarkColor", "black");
	dialog_->checkMarkColor->setColor(origCheckMarkColor);

	connect(dialog_->useLowerCaseText, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->scrollBarLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->scrollBarStyle, SIGNAL( activated(int) ), SLOT( updateChanged() ) );
	connect(dialog_->scrollBarSize, SIGNAL( valueChanged( int ) ), SLOT( updateChanged() ) );
	connect(dialog_->coloredScrollBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->centeredTabBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->highLightTab, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->statusBarFrame, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->menuItemSpacing, SIGNAL( valueChanged(int) ), SLOT( updateChanged() ) );
	connect(dialog_->buttonMenuItem, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->menuBarEmphasis, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->menuBarEmphasisBorder, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->customMenuBarEmphasisColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->menuBarEmphasisColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );
	connect(dialog_->menuStripe, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->shadowedButtonsText, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect(dialog_->shadowedMenuBarText, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->animateProgressBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->lightBorder, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->animateButton, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->animateButtonToDark, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->drawToolBarSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->drawToolBarItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->drawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->inputFocusHighlight, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect(dialog_->colorizeSortedHeader, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->buttonStyle, SIGNAL( activated(int) ), SLOT( updateChanged() ) );
	connect(dialog_->customOverHighlightColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->overHighlightColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );
	connect(dialog_->customFocusHighlightColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->focusHighlightColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );
	connect(dialog_->customCheckMarkColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(dialog_->checkMarkColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );

	if ( dialog_->animateButton->isChecked() )
	 dialog_->animateButtonToDark->setEnabled(true);
	else
	 dialog_->animateButtonToDark->setEnabled(false);

        if ( dialog_->menuBarEmphasis->isChecked() )
        {
         dialog_->menuBarEmphasisBorder->setEnabled(true);
         dialog_->customMenuBarEmphasisColor->setEnabled(true);
         if ( dialog_->customMenuBarEmphasisColor->isChecked() )
           dialog_->menuBarEmphasisColor->setEnabled(true);
         else
           dialog_->menuBarEmphasisColor->setEnabled(false);
        }
        else
        {
         dialog_->menuBarEmphasisBorder->setEnabled(false);
         dialog_->customMenuBarEmphasisColor->setEnabled(false);
         dialog_->menuBarEmphasisColor->setEnabled(false);
        }

	if ( dialog_->customOverHighlightColor->isChecked() )
	 dialog_->overHighlightColor->setEnabled(true);
	else
	 dialog_->overHighlightColor->setEnabled(false);

	if ( dialog_->customFocusHighlightColor->isChecked() )
	 dialog_->focusHighlightColor->setEnabled(true);
	else
	 dialog_->focusHighlightColor->setEnabled(false);

	if ( dialog_->customCheckMarkColor->isChecked() )
	 dialog_->checkMarkColor->setEnabled(true);
	else
	 dialog_->checkMarkColor->setEnabled(false);
}

PolyesterStyleConfig::~PolyesterStyleConfig()
{
}


void PolyesterStyleConfig::save()
{
	QSettings s;
	s.writeEntry("/polyesterstyle/Settings/useLowerCaseText", dialog_->useLowerCaseText->isChecked());
	s.writeEntry("/polyesterstyle/Settings/scrollBarLines", dialog_->scrollBarLines->isChecked());
	s.writeEntry("/polyesterstyle/Settings/scrollBarStyle",
                     scrollBarType(dialog_->scrollBarStyle->currentItem()));
	s.writeEntry("/polyesterstyle/Settings/scrollBarSize", dialog_->scrollBarSize->value());
	s.writeEntry("/polyesterstyle/Settings/coloredScrollBar", dialog_->coloredScrollBar->isChecked());
	s.writeEntry("/polyesterstyle/Settings/centeredTabBar", dialog_->centeredTabBar->isChecked());
	s.writeEntry("/polyesterstyle/Settings/highLightTab", dialog_->highLightTab->isChecked());
	s.writeEntry("/polyesterstyle/Settings/statusBarFrame", dialog_->statusBarFrame->isChecked());
	s.writeEntry("/polyesterstyle/Settings/menuItemSpacing", dialog_->menuItemSpacing->value());
	s.writeEntry("/polyesterstyle/Settings/buttonMenuItem", dialog_->buttonMenuItem->isChecked());
	s.writeEntry("/polyesterstyle/Settings/menuBarEmphasis", dialog_->menuBarEmphasis->isChecked());
        s.writeEntry("/polyesterstyle/Settings/colorizeSortedHeader", dialog_->colorizeSortedHeader->isChecked());
	s.writeEntry("/polyesterstyle/Settings/menuBarEmphasisBorder", dialog_->menuBarEmphasisBorder->isChecked());
	s.writeEntry("/polyesterstyle/Settings/customMenuBarEmphasisColor", dialog_->customMenuBarEmphasisColor->isChecked());
	s.writeEntry("/polyesterstyle/Settings/menuBarEmphasisColor", QColor(dialog_->menuBarEmphasisColor->color()).name());

	s.writeEntry("/polyesterstyle/Settings/menuStripe", dialog_->menuStripe->isChecked());
	s.writeEntry("/polyesterstyle/Settings/shadowedButtonsText", dialog_->shadowedButtonsText->isChecked());
        s.writeEntry("/polyesterstyle/Settings/shadowedMenuBarText", dialog_->shadowedMenuBarText->isChecked());
	s.writeEntry("/polyesterstyle/Settings/animateProgressBar", dialog_->animateProgressBar->isChecked());
	s.writeEntry("/polyesterstyle/Settings/lightBorder", dialog_->lightBorder->isChecked());
	s.writeEntry("/polyesterstyle/Settings/animateButton", dialog_->animateButton->isChecked());
	s.writeEntry("/polyesterstyle/Settings/animateButtonToDark", dialog_->animateButtonToDark->isChecked());
	s.writeEntry("/polyesterstyle/Settings/drawToolBarSeparator", dialog_->drawToolBarSeparator->isChecked());
	s.writeEntry("/polyesterstyle/Settings/drawToolBarItemSeparator", dialog_->drawToolBarItemSeparator->isChecked());
	s.writeEntry("/polyesterstyle/Settings/drawTriangularExpander", dialog_->drawTriangularExpander->isChecked());
	s.writeEntry("/polyesterstyle/Settings/inputFocusHighlight", dialog_->inputFocusHighlight->isChecked());
	s.writeEntry("/polyesterstyle/Settings/buttonStyle", buttonType(dialog_->buttonStyle->currentItem()));
	s.writeEntry("/polyesterstyle/Settings/customOverHighlightColor", dialog_->customOverHighlightColor->isChecked());
	s.writeEntry("/polyesterstyle/Settings/overHighlightColor", QColor(dialog_->overHighlightColor->color()).name());
	s.writeEntry("/polyesterstyle/Settings/customFocusHighlightColor", dialog_->customFocusHighlightColor->isChecked());
	s.writeEntry("/polyesterstyle/Settings/focusHighlightColor", QColor(dialog_->focusHighlightColor->color()).name());
	s.writeEntry("/polyesterstyle/Settings/customCheckMarkColor", dialog_->customCheckMarkColor->isChecked());
	s.writeEntry("/polyesterstyle/Settings/checkMarkColor", QColor(dialog_->checkMarkColor->color()).name());
}

void PolyesterStyleConfig::defaults()
{
	dialog_->useLowerCaseText->setChecked(false);
	dialog_->scrollBarLines->setChecked(false);
	dialog_->scrollBarStyle->setCurrentItem(0);
	dialog_->scrollBarSize->setValue(16);
	dialog_->coloredScrollBar->setChecked(true);
	dialog_->centeredTabBar->setChecked(false);
	dialog_->highLightTab->setChecked(true);
	dialog_->statusBarFrame->setChecked(true);
	dialog_->menuBarEmphasis->setChecked(false);
	dialog_->menuBarEmphasisBorder->setChecked(true);
	dialog_->customMenuBarEmphasisColor->setChecked(false);
	dialog_->menuBarEmphasisColor->setColor("black");

	dialog_->menuStripe->setChecked(true);
	dialog_->animateProgressBar->setChecked(false);
        dialog_->colorizeSortedHeader->setChecked(true);
	dialog_->lightBorder->setChecked(true);
	dialog_->animateButton->setChecked(false);
	dialog_->animateButtonToDark->setChecked(false);
	dialog_->drawToolBarSeparator->setChecked(true);
	dialog_->drawToolBarItemSeparator->setChecked(true);
	dialog_->drawTriangularExpander->setChecked(false);
	dialog_->inputFocusHighlight->setChecked(true);
	dialog_->buttonStyle->setCurrentItem(0);
	dialog_->customOverHighlightColor->setChecked(false);
	dialog_->overHighlightColor->setColor("black");
	dialog_->customFocusHighlightColor->setChecked(false);
	dialog_->focusHighlightColor->setColor("black");
	dialog_->customCheckMarkColor->setChecked(false);
	dialog_->checkMarkColor->setColor("black");
	//updateChanged would be done by setChecked already
}

void PolyesterStyleConfig::updateChanged()
{

	if ((dialog_->scrollBarLines->isChecked() == origScrollBarLines) &&
	    (dialog_->scrollBarStyle->currentText() == origScrollBarStyle) &&
	    (dialog_->scrollBarSize->value() == origScrollBarSize) &&
	    (dialog_->coloredScrollBar->isChecked() == origColoredScrollBar) &&
	    (dialog_->centeredTabBar->isChecked() == origCenteredTabBar) &&
	    (dialog_->highLightTab->isChecked() == origHighLightTab) &&
	    (dialog_->statusBarFrame->isChecked() == origStatusBarFrame) &&
	    (dialog_->menuItemSpacing->value() == origMenuItemSpacing) &&
	    (dialog_->buttonMenuItem->isChecked() == origButtonMenuItem) &&
	    (dialog_->menuBarEmphasis->isChecked() == origMenuBarEmphasis) &&
	    (dialog_->menuBarEmphasisBorder->isChecked() == origMenuBarEmphasisBorder) &&
	     (dialog_->customMenuBarEmphasisColor->isChecked() == origCustomMenuBarEmphasisColor) &&
	      (dialog_->menuBarEmphasisColor->color() == origMenuBarEmphasisColor) &&
	    (dialog_->menuStripe->isChecked() == origMenuStripe) &&
	    (dialog_->shadowedButtonsText->isChecked() == origShadowedButtonsText) &&
            (dialog_->shadowedMenuBarText->isChecked() == origShadowedMenuBarText) &&
	    (dialog_->animateProgressBar->isChecked() == origAnimProgressBar) &&
	    (dialog_->lightBorder->isChecked() == origLightBorder) &&
            (dialog_->colorizeSortedHeader->isChecked() == origColorizeSortedHeader) &&
	    (dialog_->animateButton->isChecked() == origAnimButton) &&
	     (dialog_->animateButtonToDark->isChecked() == origAnimButtonToDark) &&
	    (dialog_->drawToolBarSeparator->isChecked() == origDrawToolBarSeparator) &&
	    (dialog_->drawToolBarItemSeparator->isChecked() == origDrawToolBarItemSeparator) &&
	    (dialog_->drawTriangularExpander->isChecked() == origDrawTriangularExpander) &&
	    (dialog_->inputFocusHighlight->isChecked() == origInputFocusHighlight) &&
	    (dialog_->customOverHighlightColor->isChecked() == origCustomOverHighlightColor) &&
	     (dialog_->overHighlightColor->color() == origOverHighlightColor) &&
	    (dialog_->buttonStyle->currentText() == origButtonStyle) &&
	    (dialog_->customFocusHighlightColor->isChecked() == origCustomFocusHighlightColor) &&
	     (dialog_->focusHighlightColor->color() == origFocusHighlightColor) &&
	    (dialog_->customCheckMarkColor->isChecked() == origCustomCheckMarkColor) &&
	     (dialog_->checkMarkColor->color() == origCheckMarkColor) &&
		(dialog_->useLowerCaseText->isChecked() == origUseLowerCaseText)
	   )
		emit changed(false);
	else
		emit changed(true);
}

//button types
QString PolyesterStyleConfig::buttonType( int listnr )
{
    switch ( listnr ) {
        case 1:
            return QString("gradients");
	case 2:
	    return QString("reverseGradients");
	case 3:
	    return QString("flat");
        default:
            return QString("glass");
    }
}

int PolyesterStyleConfig::buttonItem( QString kBType )
{
    if( !(kBType == "gradients") )
        return 1;
    else if( !(kBType == "reverseGradients") )
        return 2;
    else if( !(kBType == "flat") )
        return 3;
    else
        return 0;
}

//scrollBar types
QString PolyesterStyleConfig::scrollBarType( int listnr )
{
    switch ( listnr ) {
	case 1:
	    return QString("PlatinumStyleScrollBar");
	case 2:
	    return QString("NextStyleScrollBar");
        case 3:
            return QString("WindowsStyleScrollBar");
        default:
            return QString("ThreeButtonScrollBar");
    }
}

int PolyesterStyleConfig::scrollBarItem( QString kSType )
{
    if( !(kSType == "PlatinumStyleScrollBar") )
        return 1;
    else if( !(kSType == "NextStyleScrollBar") )
        return 2;
    else if( !(kSType == "WindowsStyleScrollBar") )
        return 3;
    else
        return 0;
}

#include "polyesterconf.moc"
