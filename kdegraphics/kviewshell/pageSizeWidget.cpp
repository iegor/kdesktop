// pageSizeWidget.cpp
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002-2003 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#include <config.h>

#include <kcombobox.h>
#include <kdebug.h>
#include <klocale.h>
#include <qlineedit.h>
#include <qvalidator.h>

#include "pageSize.h"
#include "pageSizeWidget.h"
#include "sizePreview.h"



// Constructs a pageSizeWidget_base which is a child of 'parent', with
// the name 'name' and widget flags set to 'f'.
pageSizeWidget::pageSizeWidget( QWidget* parent,  const char* name, WFlags fl )
    : pageSizeWidget_base( parent,  name, fl )
{
  connect(&chosenSize, SIGNAL(sizeChanged(const SimplePageSize&)), previewer, SLOT(setSize(const SimplePageSize&)));

  // Set up the formatChoice QComboBox
  formatChoice->insertItem(i18n("Custom Size"));
  formatChoice->insertStringList(chosenSize.pageSizeNames());

  // Activate the proper entry in the QComboBox
  if (chosenSize.formatName().isNull()) {
    orientationChoice->setEnabled(false);
    formatChoice->setCurrentItem(0);
  } else {
    orientationChoice->setEnabled(true);
    formatChoice->setCurrentText(chosenSize.formatName());
  }
  paperSize(formatChoice->currentItem());

  connect(formatChoice, SIGNAL(activated(int)), this, SLOT(paperSize(int)));
  connect(orientationChoice, SIGNAL(activated(int)), this, SLOT(orientationChanged(int)));

  // Update the text fields when the user switches to a new unit, and
  // when the "custom format" is NOT selected.
  connect(widthUnits, SIGNAL(activated(int)), this, SLOT(unitsChanged(int)));
  connect(heightUnits, SIGNAL(activated(int)), this, SLOT(unitsChanged(int)));

  // Upate the chosen size whenever the user edits the input field. 
  connect(widthInput, SIGNAL(textChanged(const QString &)), this, SLOT(input(const QString &)));
  connect(heightInput, SIGNAL(textChanged(const QString &)), this, SLOT(input(const QString &)));

  // Allow entries between 0 and 1200. More filtering is done by the
  // pageSize class, which silently ignores values which are out of
  // range.
  widthInput->setValidator(new QDoubleValidator(0.0, 1200.0, 1, this, "widthValidator"));
  heightInput->setValidator(new QDoubleValidator(0.0, 1200.0, 1, this, "heightValidator"));
}


// Receives the output from the formatChoice combobox. A value of
// "zero" means that the "custom paper size" has been chosen.
void pageSizeWidget::paperSize(int index)
{
  widthInput->setEnabled(index == 0);
  heightInput->setEnabled(index == 0);
  orientationChoice->setEnabled(index != 0);
  
  if (index != 0) {
    chosenSize.setPageSize(formatChoice->currentText());
    chosenSize.setOrientation(orientationChoice->currentItem());
  }
  widthUnits->setCurrentText(chosenSize.preferredUnit());
  heightUnits->setCurrentText(chosenSize.preferredUnit());
  
  fillTextFields();
}


void pageSizeWidget::setPageSize(const QString& sizeName)
{
  chosenSize.setPageSize(sizeName);

  int index = chosenSize.formatNumber();

  formatChoice->setCurrentItem(index+1);
  widthInput->setEnabled(index == -1);
  heightInput->setEnabled(index == -1);
  orientationChoice->setEnabled(index != -1);

  widthUnits->setCurrentText(chosenSize.preferredUnit());
  heightUnits->setCurrentText(chosenSize.preferredUnit());

  fillTextFields();
}


void pageSizeWidget::fillTextFields()
{				   
  // Warning! It is really necessary here to first generate both
  // strings, and then call setText() on the input widgets. Reason?
  // Calling setText() implicitly calls the input() method via the
  // textChanged()-Signal of the QLineEdit widgets.
  QString width  = chosenSize.widthString(widthUnits->currentText());
  QString height = chosenSize.heightString(heightUnits->currentText());

  widthInput->setText(width);
  heightInput->setText(height);
}


void pageSizeWidget::unitsChanged(int)
{
  // Update the text fields, i.e. show the current size in the proper
  // units, but only if the "custom size" is NOT selected.
  if (formatChoice->currentItem() != 0)
    fillTextFields();
  else
    input(QString::null);
}


void pageSizeWidget::setOrientation(int ori)
{
  orientationChoice->setCurrentItem(ori);
  orientationChanged();
}


void pageSizeWidget::orientationChanged(int orient)
{
  // Update the page preview
  chosenSize.setOrientation(orient);
}


void pageSizeWidget::input(const QString &)
{
  chosenSize.setPageSize(widthInput->text(), widthUnits->currentText(), heightInput->text(), heightUnits->currentText());
}



#include "pageSizeWidget.moc"
