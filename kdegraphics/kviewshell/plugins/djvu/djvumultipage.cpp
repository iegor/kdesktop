/***************************************************************************
 *   Copyright (C) 2005 by Stefan Kebekus                                  *
 *   kebekus@kde.org                                                       *
 *                                                                         *
 *   Copyright (C) 2005 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <kaction.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <kparts/genericfactory.h>
#include <kprinter.h>
#include <ktempfile.h>
#include "kvsprefs.h"
#include <qapplication.h>
#include <qpaintdevicemetrics.h>
#include <qprinter.h>
#include <qtooltip.h>

#include "ByteStream.h"
#include "DjVuToPS.h"
#include "kprintDialogPage_DJVUpageoptions.h"
#include "kprintDialogPage_DJVUconversionoptions.h"
#include "djvumultipage.h"
#include "pageRangeWidget.h"
#include "prefs.h"

#include "kmessagebox.h"

typedef KParts::GenericFactory<DjVuMultiPage> DjVuMultiPageFactory;
K_EXPORT_COMPONENT_FACTORY(djvuviewpart, DjVuMultiPageFactory)


DjVuMultiPage::DjVuMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent,
                             const char *name, const QStringList&)
  : KMultiPage(parentWidget, widgetName, parent, name), djvuRenderer(parentWidget)
{
  /* This is kparts wizardry that cannot be understood by man. Simply
     change the names to match your implementation.  */
  setInstance(DjVuMultiPageFactory::instance());
  djvuRenderer.setName("DjVu renderer");

  // Render modes
  QStringList renderModes;
  renderModes.append(i18n("Color"));
  renderModes.append(i18n("Black and White"));
  renderModes.append(i18n("Show foreground only"));
  renderModes.append(i18n("Show background only"));
  renderModeAction = new KSelectAction (i18n("Render Mode"), 0, 0, 0, actionCollection(), "render_mode");
  renderModeAction->setItems(renderModes);

  renderModeAction->setCurrentItem(Prefs::renderMode());

  deletePagesAction = new KAction(i18n("Delete Pages..."), 0, this, SLOT(slotDeletePages()), actionCollection(), "delete_pages");

  // change the rendermode
  connect(renderModeAction, SIGNAL(activated(int)), this, SLOT(setRenderMode(int)));

  /* It is very important that this method is called in the
     constructor. Otherwise kmultipage does not know how to render
     files, and crashes may result. */
  setRenderer(&djvuRenderer);

  setXMLFile("djvumultipage.rc");

  enableActions(false);
}


DjVuMultiPage::~DjVuMultiPage()
{
  ;
}


KAboutData* DjVuMultiPage::createAboutData()
{
  /* You obviously want to change this to match your setup */
  KAboutData* about = new KAboutData("djvumultipage", I18N_NOOP("KDjView"), "0.1",
                          I18N_NOOP("KViewshell DjVu Plugin."),
                          KAboutData::License_GPL,
                          "Wilfried Huss",
                          I18N_NOOP("This program displays DjVu files."));

  about->addAuthor ("Stefan Kebekus",
                    I18N_NOOP("KViewShell plugin"),
                    "kebekus@kde.org",
                    "http://www.mi.uni-koeln.de/~kebekus");

  about->addAuthor ("Wilfried Huss",
                    I18N_NOOP("DjVu file loading"),
                    "Wilfried.Huss@gmx.at");

  return about;
}

void DjVuMultiPage::enableActions(bool b)
{
  KMultiPage::enableActions(b);

  deletePagesAction->setEnabled(b);
}

void DjVuMultiPage::setFile(bool r)
{
  enableActions(r);
}

QStringList DjVuMultiPage::fileFormats() const
{
  /* This list is used in the file selection dialog when the file is
     saved */
  QStringList r;
  r << i18n("*.djvu|DjVu file (*.djvu)");
  return r;
}


void DjVuMultiPage::setRenderMode(int mode)
{
  // Save renderMode for future uses
  switch (mode)
  {
    case Prefs::EnumRenderMode::BlackAndWhite:
      Prefs::setRenderMode(Prefs::EnumRenderMode::BlackAndWhite);

      break;
    case Prefs::EnumRenderMode::Foreground:
      Prefs::setRenderMode(Prefs::EnumRenderMode::Foreground);

      break;
    case Prefs::EnumRenderMode::Background:
      Prefs::setRenderMode(Prefs::EnumRenderMode::Background);

      break;
    default:  //Prefs::EnumRenderMode::Color
      Prefs::setRenderMode(Prefs::EnumRenderMode::Color);
  }
  Prefs::writeConfig();
  renderModeChanged();
}


void DjVuMultiPage::slotDeletePages()
{
  if (numberOfPages() == 0)
    return;
  
  KDialogBase dialog( parentWdg, "urldialog", true, i18n("Delete Pages"), KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true );
  PageRangeWidget range( 1, numberOfPages(), currentPageNumber(), &dialog, "range widget" );
  QToolTip::add( &range, i18n( "Select the pages you wish to delete." ) );
  dialog.setButtonOK(i18n("Delete Pages"));
  dialog.setMainWidget(&range);
  if (dialog.exec() != QDialog::Accepted)
    return;

  djvuRenderer.deletePages(range.getFrom(), range.getTo());


  // =========
  pageCache->deselectText();
  document_history.clear();
  pageCache->clear();
  
  generateDocumentWidgets();
  
  // Set number of widgets in the thumbnail sidebar
  markList()->clear();
  markList()->setNumberOfPages(numberOfPages(), KVSPrefs::showThumbnails());
  
  // Set Table of Contents
  //@@@@@@@@@@  tableOfContents->setContents(renderer->getBookmarks());
  
  // Clear Statusbar
  emit setStatusBarText(QString::null);
}


void DjVuMultiPage::print()
{
  // Paranoid safety checks
  if (djvuRenderer.isEmpty())
    return;

  // Allocate the printer structure
  KPrinter *printer = getPrinter(false);
  if (printer == 0)
    return;

  KPrintDialogPage_DJVUPageOptions *pageOptions = new KPrintDialogPage_DJVUPageOptions();
  if (pageOptions == 0) {
    kdError(1223) << "DjVuMultiPage::print(): Cannot allocate new KPrintDialogPage_PageOptions structure" << endl;
    delete printer;
    return;
  }
  printer->addDialogPage( pageOptions );

  KPrintDialogPage_DJVUConversionOptions *conversionOptions = new KPrintDialogPage_DJVUConversionOptions();
  if (pageOptions == 0) {
    kdError(1223) << "DjVuMultiPage::print(): Cannot allocate new KPrintDialogPage_ConversionOptions structure" << endl;
    delete printer;
    return;
  }
  printer->addDialogPage( conversionOptions );
  
  // initialize the printer using the print dialog
  if ( printer->setup(parentWdg, i18n("Print %1").arg(m_file.section('/', -1))) ) {    
    // Now do the printing. 
    QValueList<int> pageList = printer->pageList();
    if (pageList.isEmpty()) 
      printer->abort();
    else {
      // Printing usually takes a while. This is to keep the GUI
      // updated.
      qApp->processEvents();
      
      // Printing...
      DjVuToPS converter;
      DjVuToPS::Options &options = converter.options;
      
      // Set PostScript Language Level, taking 3 as the default
      options.set_format(DjVuToPS::Options::PS);
      QString op = printer->option( "kde-kdjvu-pslevel" );
      if (op == "1")
	options.set_level(1);
      else
	if (op == "3")
	  options.set_level(3);
	else
	  options.set_level(2);
      
      // Set page size orientation
      if (printer->option( "kde-kviewshell-rotatepage" ) == "true")
	options.set_orientation (DjVuToPS::Options::AUTO);
      else
	if ( printer->orientation() == KPrinter::Landscape )
	  options.set_orientation (DjVuToPS::Options::LANDSCAPE);
	else
	  options.set_orientation (DjVuToPS::Options::PORTRAIT);
      
      // Set render mode, taking "color" as default
      op = printer->option("kde-kdjvu-rendermode");
      if (op == "black-and-white")
	options.set_mode(DjVuToPS::Options::BW);
      else
	if (op == "foreground")
	  options.set_mode(DjVuToPS::Options::FORE);
	else
	  if (op == "background")
	    options.set_mode(DjVuToPS::Options::BACK);
	  else
	    options.set_mode(DjVuToPS::Options::COLOR);
      
      // Set Color or Grayscale mode
      if (printer->colorMode() == KPrinter::Color)
	options.set_color(true);
      else
	options.set_color(false);
      
      // Set Zoom
      if (printer->option( "kde-kdjvu-fitpage" ) == "true")
	options.set_zoom(0);
      else
	options.set_zoom(100);
      
      KTempFile tmpPSFile(QString::null, "ps");
      tmpPSFile.close();
      tmpPSFile.setAutoDelete(true);
      
      if (djvuRenderer.convertToPSFile(converter, tmpPSFile.name(), pageList ) == true)
	printer->printFiles( QStringList(tmpPSFile.name()), true );
      else
	printer->abort();
    }
    delete printer;
  }
}


bool DjVuMultiPage::isReadWrite() const
{
  return true;
}


bool DjVuMultiPage::isModified() const
{
  return djvuRenderer.isModified();
}


void DjVuMultiPage::slotSave()
{
  // Paranoid safety checks
  if (djvuRenderer.isEmpty())
    return;

  // Try to guess the proper ending...
  QString formats;
  QString ending;
  int rindex = m_file.findRev(".");
  if (rindex == -1) {
    ending = QString::null;
    formats = QString::null;
  } else {
    ending = m_file.mid(rindex); // e.g. ".dvi"
    formats = fileFormats().grep(ending).join("\n");
  }

  QString fileName = KFileDialog::getSaveFileName(QString::null, formats, 0, i18n("Save File As"));

  if (fileName.isEmpty())
    return;

  // Add the ending to the filename. I hope the user likes it that
  // way.
  if (!ending.isEmpty() && fileName.find(ending) == -1)
    fileName = fileName+ending;

  if (QFile(fileName).exists()) {
    int r = KMessageBox::warningContinueCancel(parentWdg, i18n("The file %1\nalready exists. Do you want to overwrite it?").arg(fileName),
					       i18n("Overwrite File"), i18n("Overwrite"));
    if (r == KMessageBox::Cancel)
      return;
  }
  
  djvuRenderer.save(fileName);

  /*
  if (!djvuRenderer.save(fileName) == false)
    KMessageBox::error( parentWdg,
			i18n("<qt><strong>File error.</strong> Unable to write to the specified file '%1'. The document is <strong>not</strong> saved.</qt>").arg(fileName),
			i18n("File Error"));
  */
  
  return;
}



#include "djvumultipage.moc"
