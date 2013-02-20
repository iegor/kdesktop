/* This file is part of the KDE project

Copyright (C) 2001, 2003 Lukas Tinkl <lukas@kde.org>
Andreas Schlapbach <schlpbch@iam.unibe.ch>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#include <qlabel.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>


#include <klocale.h>
#include <kurl.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kurlrequester.h>
#include <kglobalsettings.h>
#include <kconfig.h>

#include "imgallerydialog.h"
#include "imgallerydialog.moc"

KIGPDialog::KIGPDialog(QWidget *parent, const QString& path, const char *name )
    : KDialogBase( IconList, i18n("Configure"), Default|Ok|Cancel,
                   Ok, parent, name, true, true ),
      m_dialogOk( false )
{
    m_path = path;
    setCaption(i18n("Create Image Gallery"));
    setButtonOK( KGuiItem(i18n("Create"),"imagegallery") );
    m_config = new KConfig("kimgallerypluginrc", false, false);
    setupLookPage(path);
    setupDirectoryPage(path);
    setupThumbnailPage(path);
}

void KIGPDialog::slotDefault()
{
    m_title->setText(i18n("Image Gallery for %1").arg(m_path));
    m_imagesPerRow->setValue(4);
    m_imageName->setChecked(true);
    m_imageSize->setChecked(false);
    m_imageProperty->setChecked(false);
    m_fontName->setCurrentText( KGlobalSettings::generalFont().family() );
    m_fontSize->setValue(14);
    m_foregroundColor->setColor( QColor( "#d0ffd0") );
    m_backgroundColor->setColor( QColor("#333333") );

    m_imageNameReq->setURL(m_path + "images.html");
    m_recurseSubDir->setChecked( false );
    m_recursionLevel->setEnabled( false );
    m_copyOriginalFiles->setChecked( false );
    m_useCommentFile->setChecked( false );
    m_commentFileReq->setURL(m_path + "comments");
    m_commentFileReq->setEnabled( false );

    m_imageFormat->setCurrentText( "JPEG");
    m_thumbnailSize->setValue(140);
    m_colorDepthSet->setChecked(false);
    m_colorDepth->setCurrentText("8");
}

void KIGPDialog::setupLookPage(const QString& path) {
    QFrame *page = addPage( i18n("Look"), i18n("Page Look"),
                            BarIcon("colorize", KIcon::SizeMedium ) );

    m_config->setGroup("Look");
    QVBoxLayout *vlay = new QVBoxLayout( page, 0, spacingHint() );

    QLabel *label;

    label = new QLabel( i18n("&Page title:"), page);
    vlay->addWidget(label);

    m_title = new QLineEdit(i18n("Image Gallery for %1").arg(path), page);
    vlay->addWidget( m_title );
    label->setBuddy(m_title);

    m_imagesPerRow = new KIntNumInput(m_config->readNumEntry("ImagesPerRow", 4), page);
    m_imagesPerRow->setRange(1, 8, 1, true );
    m_imagesPerRow->setLabel( i18n("I&mages per row:") );
    vlay->addWidget( m_imagesPerRow );

    QGridLayout *grid = new QGridLayout( 2, 2 );
    vlay->addLayout( grid );

    m_imageName = new QCheckBox( i18n("Show image file &name"), page);
    m_imageName->setChecked( m_config->readBoolEntry("ImageName", true) );
    grid->addWidget( m_imageName, 0, 0 );

    m_imageSize = new QCheckBox( i18n("Show image file &size"), page);
    m_imageSize->setChecked( m_config->readBoolEntry("ImageSize", false) );
    grid->addWidget( m_imageSize, 0, 1 );

    m_imageProperty = new QCheckBox( i18n("Show image &dimensions"), page);
    m_imageProperty->setChecked( m_config->readBoolEntry("ImageProperty", false) );
    grid->addWidget( m_imageProperty, 1, 0 );

    QHBoxLayout *hlay11  = new QHBoxLayout( );
    vlay->addLayout( hlay11 );

    m_fontName = new QComboBox( false,page );
    QStringList standardFonts;
    KFontChooser::getFontList(standardFonts, 0);
    m_fontName->insertStringList( standardFonts );
    m_fontName->setCurrentText( m_config->readEntry("FontName", KGlobalSettings::generalFont().family() ) );

    label = new QLabel( i18n("Fon&t name:"), page );
    label->setBuddy( m_fontName );
    hlay11->addWidget( label );
    hlay11->addStretch( 1 );
    hlay11->addWidget( m_fontName );

    QHBoxLayout *hlay12  = new QHBoxLayout( );
    vlay->addLayout( hlay12 );

    m_fontSize = new QSpinBox( 6, 15, 1, page );
    m_fontSize->setValue( m_config->readNumEntry("FontSize", 14) );

    label = new QLabel( i18n("Font si&ze:"), page );
    label->setBuddy( m_fontSize );
    hlay12->addWidget( label );
    hlay12->addStretch( 1 );
    hlay12->addWidget( m_fontSize );

    QHBoxLayout *hlay1  = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay1 );

    m_foregroundColor = new KColorButton(page);
    m_foregroundColor->setColor( QColor( m_config->readEntry("ForegroundColor", "#d0ffd0") ) );

    label = new QLabel( i18n("&Foreground color:"), page);
    label->setBuddy( m_foregroundColor );
    hlay1->addWidget( label );
    hlay1->addStretch( 1 );
    hlay1->addWidget(m_foregroundColor);

    QHBoxLayout *hlay2 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay2 );

    m_backgroundColor = new KColorButton(page);
    m_backgroundColor->setColor( QColor(m_config->readEntry("BackgroundColor", "#333333") ) );

    label = new QLabel( i18n("&Background color:"), page);
    hlay2->addWidget( label );
    label->setBuddy( m_backgroundColor );
    hlay2->addStretch( 1 );
    hlay2->addWidget(m_backgroundColor);

    vlay->addStretch(1);
}

void KIGPDialog::setupDirectoryPage(const QString& path) {
    QFrame *page = addPage( i18n("Folders"), i18n("Folders"),
                            BarIcon("folder", KIcon::SizeMedium ) );

    m_config->setGroup("Directory");
    QVBoxLayout *dvlay = new QVBoxLayout( page, 0, spacingHint() );

    QLabel *label;
    label = new QLabel(i18n("&Save to HTML file:"), page);
    dvlay->addWidget( label );
    QString whatsThis;
    whatsThis = i18n("<p>The name of the HTML file this gallery will be saved to.");
    QWhatsThis::add( label, whatsThis );

    m_imageNameReq = new KURLRequester(path + "images.html", page);
    label->setBuddy( m_imageNameReq );
    dvlay->addWidget(m_imageNameReq);
    connect( m_imageNameReq, SIGNAL(textChanged(const QString&)),
             this, SLOT(imageUrlChanged(const QString&)) );
    QWhatsThis::add( m_imageNameReq, whatsThis );

    const bool recurseSubDir = m_config->readBoolEntry("RecurseSubDirectories", false);
    m_recurseSubDir = new QCheckBox(i18n("&Recurse subfolders"), page);
    m_recurseSubDir->setChecked( recurseSubDir );
    whatsThis = i18n("<p>Whether subfolders should be included for the "
                     "image gallery creation or not.");
    QWhatsThis::add( m_recurseSubDir, whatsThis );

    const int recursionLevel = m_config->readNumEntry("RecursionLevel", 0);
    m_recursionLevel = new KIntNumInput( recursionLevel, page );
    m_recursionLevel->setRange( 0, 99, 1, true );
    m_recursionLevel->setLabel( i18n("Rec&ursion depth:") );
    if ( recursionLevel == 0 )
      m_recursionLevel->setSpecialValueText( i18n("Endless"));
    m_recursionLevel->setEnabled(recurseSubDir);
    whatsThis = i18n("<p>You can limit the number of folders the "
                     "image gallery creator will traverse to by setting an "
                     "upper bound for the recursion depth.");
    QWhatsThis::add( m_recursionLevel, whatsThis );


    connect(m_recurseSubDir, SIGNAL( toggled(bool) ),
            m_recursionLevel, SLOT( setEnabled(bool) ) );

    dvlay->addWidget(m_recurseSubDir);
    dvlay->addWidget(m_recursionLevel);

    m_copyOriginalFiles = new QCheckBox(i18n("Copy or&iginal files"), page);
    m_copyOriginalFiles->setChecked(m_config->readBoolEntry("CopyOriginalFiles", false) );
    dvlay->addWidget(m_copyOriginalFiles);
    whatsThis = i18n("<p>This makes a copy of all images and the gallery will refer "
                     "to these copies instead of the original images.");
    QWhatsThis::add( m_copyOriginalFiles, whatsThis );


    const bool useCommentFile = m_config->readBoolEntry("UseCommentFile", false);
    m_useCommentFile = new QCheckBox(i18n("Use &comment file"), page);
    m_useCommentFile->setChecked(useCommentFile);
    dvlay->addWidget(m_useCommentFile);

    whatsThis = i18n("<p>If you enable this option you can specify "
                     "a comment file which will be used for generating "
                     "subtitles for the images."
                     "<p>For details about the file format please see "
                     "the \"What's This?\" help below.");
    QWhatsThis::add( m_useCommentFile, whatsThis );

    label = new QLabel(i18n("Comments &file:"), page);
    label->setEnabled( useCommentFile );
    dvlay->addWidget( label );
    whatsThis = i18n("<p>You can specify the name of the comment file here. "
                     "The comment file contains the subtitles for the images. "
                     "The format of this file is:"
                     "<p>FILENAME1:"
                     "<br>Description"
                     "<br>"
                     "<br>FILENAME2:"
                     "<br>Description"
                     "<br>"
                     "<br>and so on");
    QWhatsThis::add( label, whatsThis );

    m_commentFileReq = new KURLRequester(path + "comments", page);
    m_commentFileReq->setEnabled(useCommentFile);
    label->setBuddy( m_commentFileReq );
    dvlay->addWidget(m_commentFileReq);
    QWhatsThis::add( m_commentFileReq, whatsThis );

    connect(m_useCommentFile, SIGNAL(toggled(bool)),
            label, SLOT(setEnabled(bool)));
    connect(m_useCommentFile, SIGNAL(toggled(bool)),
            m_commentFileReq, SLOT(setEnabled(bool)));

    dvlay->addStretch(1);
}

void KIGPDialog::setupThumbnailPage(const QString& path) {
    QFrame *page = addPage( i18n("Thumbnails"), i18n("Thumbnails"),
                            BarIcon("thumbnail", KIcon::SizeMedium ) );

    m_config->setGroup("Thumbnails");
    QLabel *label;

    QVBoxLayout *vlay = new QVBoxLayout( page, 0, spacingHint() );

    QHBoxLayout *hlay3 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay3 );

    m_imageFormat = new QComboBox(false, page);
    m_imageFormat->insertItem("JPEG");
    m_imageFormat->insertItem("PNG");
    m_imageFormat->setCurrentText( m_config->readEntry("ImageFormat", "JPEG") );

    label = new QLabel( i18n("Image format f&or the thumbnails:"), page);
    hlay3->addWidget( label );
    label->setBuddy( m_imageFormat );
    hlay3->addStretch( 1 );
    hlay3->addWidget(m_imageFormat);

    m_thumbnailSize = new KIntNumInput(m_config->readNumEntry("ThumbnailSize", 140), page);
    m_thumbnailSize->setRange(10, 1000, 1, true );
    m_thumbnailSize->setLabel( i18n("Thumbnail size:") );
    vlay->addWidget( m_thumbnailSize );

    QGridLayout *grid = new QGridLayout( 2, 2 );
    vlay->addLayout( grid );

    QHBoxLayout *hlay4 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay4 );
    const bool colorDepthSet = m_config->readBoolEntry("ColorDepthSet", false);
    m_colorDepthSet = new QCheckBox(i18n("&Set different color depth:"), page);
    m_colorDepthSet->setChecked(colorDepthSet);
    hlay4->addWidget( m_colorDepthSet );

    m_colorDepth = new QComboBox(false, page);
    m_colorDepth->insertItem("1");
    m_colorDepth->insertItem("8");
    m_colorDepth->insertItem("16");
    m_colorDepth->insertItem("32");
    m_colorDepth->setCurrentText(m_config->readEntry("ColorDepth", "8"));
    m_colorDepth->setEnabled(colorDepthSet);
    hlay4->addWidget( m_colorDepth );

    connect(m_colorDepthSet, SIGNAL( toggled(bool) ),
            m_colorDepth, SLOT( setEnabled(bool) ) );

    vlay->addStretch(1);

}

void KIGPDialog::writeConfig()
{
 m_config->setGroup("Look");
 m_config->writeEntry("ImagesPerRow", getImagesPerRow());
 m_config->writeEntry("ImageName", printImageName());
 m_config->writeEntry("ImageSize", printImageSize());
 m_config->writeEntry("ImageProperty", printImageProperty());
 m_config->writeEntry("FontName", getFontName());
 m_config->writeEntry("FontSize", getFontSize());
 m_config->writeEntry("ForegroundColor", getForegroundColor().name() );
 m_config->writeEntry("BackgroundColor", getBackgroundColor().name());

 m_config->setGroup("Directory");
 m_config->writeEntry("RecurseSubDirectories", recurseSubDirectories());
 m_config->writeEntry("RecursionLevel", recursionLevel());
 m_config->writeEntry("CopyOriginalFiles", copyOriginalFiles());
 m_config->writeEntry("UseCommentFile", useCommentFile());

 m_config->setGroup("Thumbnails");
 m_config->writeEntry("ThumbnailSize", getThumbnailSize());
 m_config->writeEntry("ColorDepth", getColorDepth());
 m_config->writeEntry("ColorDepthSet", colorDepthSet());
 m_config->writeEntry("ImageFormat", getImageFormat());
 m_config->sync();
}

KIGPDialog::~KIGPDialog()
{
}

void KIGPDialog::imageUrlChanged(const QString &url )
{
    enableButtonOK( !url.isEmpty());
}

bool  KIGPDialog::printImageName()  const
{
    return m_imageName->isChecked();
}

bool  KIGPDialog::printImageSize() const
{
    return m_imageSize->isChecked();
}

bool  KIGPDialog::printImageProperty() const
{
    return m_imageProperty->isChecked();
}

bool KIGPDialog::recurseSubDirectories() const
{
    return m_recurseSubDir->isChecked();
}

int KIGPDialog::recursionLevel() const
{
    return m_recursionLevel->value();
}

bool KIGPDialog::copyOriginalFiles() const
{
    return m_copyOriginalFiles->isChecked();
}

bool KIGPDialog::useCommentFile() const
{
    return m_useCommentFile->isChecked();
}

int KIGPDialog::getImagesPerRow() const
{
    return m_imagesPerRow->value();
}

int KIGPDialog::getThumbnailSize() const
{
    return m_thumbnailSize->value();
}

int KIGPDialog::getColorDepth() const
{
    return m_colorDepth->currentText().toInt();
}

bool KIGPDialog::colorDepthSet() const
{
    return m_colorDepthSet->isChecked();
}

const QString KIGPDialog::getTitle() const
{
    return m_title->text();
}

const QString KIGPDialog::getImageName() const
{
    return m_imageNameReq->url();
}

const QString KIGPDialog::getCommentFile() const
{
    return m_commentFileReq->url();
}

const QString KIGPDialog::getFontName() const
{
    return m_fontName->currentText();
}

const QString KIGPDialog::getFontSize() const
{
    return m_fontSize->text();
}

const QColor KIGPDialog::getBackgroundColor() const
{
    return m_backgroundColor->color();
}

const QColor KIGPDialog::getForegroundColor() const
{
    return m_foregroundColor->color();
}

const QString KIGPDialog::getImageFormat() const
{
    return m_imageFormat->currentText();
}
