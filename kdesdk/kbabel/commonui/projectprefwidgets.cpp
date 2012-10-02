/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2005 by Stanislav Visnovsky
			    <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */
#include "klisteditor.h"
#include "toolselectionwidget.h"
#include "projectprefwidgets.h"
#include "resources.h"
#include "kbabeldictbox.h"
#include "toolaction.h"
#include "cmdedit.h"
#include "kbprojectsettings.h"

#include <kcombobox.h>
#include <kdatatool.h>
#include <klocale.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <knuminput.h>
#include <kmessagebox.h>
#include <klineedit.h> 
#include <kurlcompletion.h>
#include <kfontdialog.h>
#include <kcolorbutton.h>
#include <kparts/componentfactory.h>
#include <kregexpeditorinterface.h>
#include <ksconfig.h>
#include <kurldrag.h>
#include <kurlrequester.h>

#include <qlayout.h>
#include <qobjectlist.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtextcodec.h>
#include <qwhatsthis.h>

using namespace KBabel;

static QSize sizeHintForWidget(const QWidget* widget)
{
  //
  // The size is computed by adding the sizeHint().height() of all
  // widget children and taking the width of the widest child and adding
  // layout()->margin() and layout()->spacing()
  //

  QSize size;

  int numChild = 0;
  QObjectList *l = (QObjectList*)(widget->children());

  for( uint i=0; i < l->count(); i++ )
  {
    QObject *o = l->at(i);
    if( o->isWidgetType() )
    {
      numChild += 1;
      QWidget *w=((QWidget*)o);

      QSize s = w->sizeHint();
      if( s.isEmpty() == true )
      {
          s = QSize( 50, 100 ); // Default size
      }
      size.setHeight( size.height() + s.height() );
      if( s.width() > size.width() ) { size.setWidth( s.width() ); }
    }
  }

  if( numChild > 0 )
  {
    size.setHeight( size.height() + widget->layout()->spacing()*(numChild-1) );
    size += QSize( widget->layout()->margin()*2, widget->layout()->margin()*2 + 1 );
  }
  else
  {
    size = QSize( 1, 1 );
  }

  return( size );
}




SavePreferences::SavePreferences(QWidget *parent)
    :  KTabCtl(parent)
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* box=new QGroupBox(1,Qt::Horizontal,page);
    layout->addWidget(box);

    box->setMargin(KDialog::marginHint());
    _updateButton = new QCheckBox(i18n("&Update header when saving"),box, "kcfg_AutoUpdate");
    _descriptionButton = new QCheckBox(i18n("Update &description comment when saving"),box, "kcfg_UpdateDescription");
    _autoCheckButton = new QCheckBox(i18n("Chec&k syntax of file when saving"),box, "kcfg_AutoSyntaxCheck");
    _saveObsoleteButton = new QCheckBox(i18n("Save &obsolete entries"),box, "kcfg_SaveObsolete");
    
    QGroupBox* descBox=new QGroupBox(1,Qt::Horizontal,i18n("De&scription"),page);
    layout->addWidget(descBox);
    
    descBox->setMargin(KDialog::marginHint());
    _descriptionEdit = new QLineEdit(descBox, "kcfg_DescriptionString");

    QGroupBox* encodingBox = new QGroupBox(1,Qt::Horizontal,i18n("Encoding")
                                  ,page);
    encodingBox->setMargin(KDialog::marginHint());
    layout->addWidget(encodingBox);
    QHBox *b = new QHBox(encodingBox);

    QLabel* tempLabel=new QLabel(i18n("Default:"),b);
    _encodingBox = new QComboBox(b, "kcfg_Encoding");
    b->setStretchFactor(_encodingBox,2);
    b->setSpacing(KDialog::spacingHint());

    QString defaultName=charsetString(ProjectSettingsBase::Locale);
    defaultName+=" "+i18n("(default)");
    QString utf8Name=charsetString(ProjectSettingsBase::UTF8);
    QString utf16Name=charsetString(ProjectSettingsBase::UTF16);

    _encodingBox->insertItem(defaultName,(int)ProjectSettingsBase::Locale);
    _encodingBox->insertItem(utf8Name,(int)ProjectSettingsBase::UTF8);

    // KBabel seems to crash somehow, when saving in utf16, so
    // it's better to disable this, since it is useless anyway
    // at the moment
    //_encodingBox->insertItem(utf16Name,(int)UTF16);

    tempLabel->setBuddy(_encodingBox);

    _oldEncodingButton = new QCheckBox(i18n("Kee&p the encoding of the file")
                     ,encodingBox, "kcfg_UseOldEncoding");

    _autoSaveBox = new QGroupBox( 1, Qt::Horizontal, i18n( "Automatic Saving" ), page );
    _autoSaveBox->setMargin( KDialog::marginHint( ) );
    layout->addWidget( _autoSaveBox );
    _autoSaveDelay = new KIntNumInput( _autoSaveBox, "kcfg_AutoSaveDelay" );
    _autoSaveDelay->setRange( 0, 60 );
    _autoSaveDelay->setSuffix( i18n( "Short for minutes", " min" ) );
    _autoSaveDelay->setSpecialValueText( i18n( "No autosave" ) );

    layout->addStretch(1);
    page->setMinimumSize(sizeHintForWidget(page));
    addTab(page, i18n("&General"));
    
    page = new QWidget(this);
    layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* gridBox = new QGroupBox(2,Qt::Horizontal,i18n("Fields to Update"),page);
    layout->addWidget(gridBox);
    gridBox->setMargin(KDialog::marginHint());

    _revisionButton = new QCheckBox(i18n("Re&vision-Date"),gridBox, "kcfg_UpdateRevisionDate");
    _lastButton = new QCheckBox(i18n("Last-&Translator"),gridBox, "kcfg_UpdateLastTranslator");
    _languageButton = new QCheckBox(i18n("&Language"),gridBox, "kcfg_UpdateLanguageTeam");
    _charsetButton = new QCheckBox(i18n("Char&set"),gridBox, "kcfg_UpdateCharset");
    _encodingButton = new QCheckBox(i18n("&Encoding"),gridBox, "kcfg_UpdateEncoding");
    _projectButton = new QCheckBox(i18n("Pro&ject"),gridBox, "kcfg_UpdateProject");

    QButtonGroup* dateBox = new QButtonGroup(2,Qt::Horizontal,i18n("Format of Revision-Date"),page, "kcfg_DateFormat");
    layout->addWidget(dateBox);
    box->setMargin(KDialog::marginHint());

    // we remove/insert default date button to correctly map Qt::DateFormat to our Ids
    _defaultDateButton = new QRadioButton( i18n("De&fault date format"),dateBox );
    dateBox->remove (_defaultDateButton);
    _localDateButton = new QRadioButton( i18n("Local date fo&rmat"),dateBox );
    dateBox->remove (_localDateButton);
    _customDateButton = new QRadioButton( i18n("Custo&m date format:"),dateBox );

    dateBox->insert (_defaultDateButton);
    dateBox->insert (_localDateButton);

    _dateFormatEdit = new QLineEdit(dateBox, "kcfg_CustomDateFormat");
    _dateFormatEdit->setEnabled(false);

    connect( _customDateButton, SIGNAL(toggled(bool)), this, SLOT( customDateActivated(bool) ) );

    QGroupBox* projectBox = new QGroupBox(1,Qt::Horizontal,i18n("Project String")
                                  ,page);
    projectBox->setMargin(KDialog::marginHint());
    layout->addWidget(projectBox);
    b = new QHBox(projectBox);

    tempLabel=new QLabel(i18n("Project-Id:"),b);
    _projectEdit = new QLineEdit(b, "kcfg_ProjectString");
    b->setStretchFactor(_projectEdit,2);
    b->setSpacing(KDialog::spacingHint());
    tempLabel->setBuddy(_projectEdit);
    
    layout->addStretch(1);
    page->setMinimumSize(sizeHintForWidget(page));
    addTab(page, i18n("&Header"));
    
    page = new QWidget(this);
    layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* translatorCopyrightBox = new QGroupBox(1,Qt::Horizontal, page);
    translatorCopyrightBox->setMargin(KDialog::marginHint());
    _translatorCopyrightButton = 
	new QCheckBox(i18n("Update &translator copyright")
	    ,translatorCopyrightBox, "kcfg_UpdateTranslatorCopyright");
    layout->addWidget(translatorCopyrightBox);

    QGroupBox* fsfBox=new QButtonGroup(1,Qt::Horizontal,i18n("Free Software Foundation Copyright"),page, "kcfg_FSFCopyright");
    layout->addWidget(fsfBox);

    fsfBox->setMargin(KDialog::marginHint());
    _removeFSFButton = new QRadioButton(i18n("&Remove copyright if empty"),fsfBox);
    _updateFSFButton = new QRadioButton(i18n("&Update copyright"),fsfBox);
    _nochangeFSFButton = new QRadioButton(i18n("Do &not change"),fsfBox);

    layout->addStretch(1);
    page->setMinimumSize(sizeHintForWidget(page));
    addTab(page, i18n("Cop&yright"));

    QWhatsThis::add(_updateButton,
        i18n("<qt><p><b>Update Header</b></p>\n"
             "<p>Check this button to update the header "
             "information of the file "
             "every time it is saved.</p>\n"
             "<p>The header normally keeps information about "
             "the date and time the file was last\n"
             "updated, the last translator etc.</p>\n"
             "<p>You can choose which information you want to update from the checkboxes below.\n"
             "Fields that do not exist are added to the header.\n"
             "If you want to add additional fields to the header, you can edit the header manually by choosing\n"
             "<b>Edit->Edit Header</b> in the editor window.</p></qt>"));

    QWhatsThis::add(gridBox,i18n("<qt><p><b>Fields to update</b></p>\n"
 "<p>Choose which fields in the header you want to have updated when saving.\n"
 "If a field does not exist, it is appended to the header.</p>\n"
 "<p>If you want to add other information to the header, you have to edit the header manually\n"
 "by choosing <b>Edit->Edit Header</b> in the editor window.</p>\n"
 "<p>Deactivate <b>Update Header</b> above if you do not want to have the header\n"
 "updated when saving.</p></qt>"));

    QWhatsThis::add(encodingBox,i18n("<qt><p><b>Encoding</b></p>"
"<p>Choose how to encode characters when saving to a file. If you are unsure "
"what encoding to use, please ask your translation coordinator.</p>"
"<ul><li><b>%1</b>: this is the encoding that fits the character "
"set of your system language.</li>"
"<li><b>%2</b>: uses Unicode (UTF-8) encoding.</li>"
"</ul></qt>").arg(defaultName).arg(utf8Name) );


    QWhatsThis::add(_oldEncodingButton
        ,i18n("<qt><p><b>Keep the encoding of the file</b></p>"
        "<p>If this option is activated, files are always saved in the "
        "same encoding as they were read in. Files without charset "
        "information in the header (e.g. POT files) are saved in the "
        "encoding set above.</p></qt>"));

    QWhatsThis::add(_autoCheckButton,i18n("<qt><p><b>Check syntax of file when saving</b></p>\n"
"<p>Check this to automatically check syntax of file with \"msgfmt --statistics\"\n"
"when saving a file. You will only get a message, if an error occurred.</p></qt>"));

    QWhatsThis::add(_saveObsoleteButton,i18n("<qt><p><b>Save obsolete entries</b></p>\n"
"<p>If this option is activated, obsolete entries found when the file was open\n"
"will be saved back to the file. Obsolete entries are marked by #~ and are\n"
"created when the msgmerge does not need the translation anymore.\n"
"If the text will appear again, the obsolete entries will be activated again.\n"
"The main drawback is the size of the saved file.</p></qt>"));


    QWhatsThis::add(dateBox, i18n("<qt><p><b>Format of Revision-Date</b></p>"
"<p>Choose in which format the date and time of the header field\n"
"<i>PO-Revision-Date</i> is saved: <ul>\n"
"<li><b>Default</b> is the format normally used in PO files.</li>\n"
"<li><b>Local</b> is the format specific to your country.\n"
"It can be configured in KDE's Control Center.</li>\n"
"<li><b>Custom</b> lets you define your own format.</li></ul></p> "
"<p>It is recommended that you use the default format to avoid creating non-standard PO files.</p>"
"<p>For more information, see section <b>The Preferences Dialog</b> "
"in the online help.</p>"
"</qt>") );

    setMinimumSize(sizeHint());
}


void SavePreferences::defaults(const KBabel::SaveSettings& _settings)
{
   _updateButton->setChecked(_settings.autoUpdate);

   _lastButton->setChecked(_settings.updateLastTranslator);
   _revisionButton->setChecked(_settings.updateRevisionDate);
   _languageButton->setChecked(_settings.updateLanguageTeam);
   _charsetButton->setChecked(_settings.updateCharset);
   _encodingButton->setChecked(_settings.updateEncoding);
   _projectButton->setChecked(_settings.updateProject);

   _encodingBox->setCurrentItem(_settings.encoding);
   _oldEncodingButton->setChecked(_settings.useOldEncoding);

   _projectEdit->setText(_settings.projectString);
   
   _descriptionButton->setChecked(_settings.updateDescription);
   _descriptionEdit->setText(_settings.descriptionString);
   _translatorCopyrightButton->setChecked(_settings.updateTranslatorCopyright);
   
   switch(_settings.FSFCopyright)
   {
      case ProjectSettingsBase::Update:
         _updateFSFButton->setChecked(true);
         break;
      case ProjectSettingsBase::Remove:
         _removeFSFButton->setChecked(true);
         break;
      case ProjectSettingsBase::NoChange:
         _nochangeFSFButton->setChecked(true);
         break;
      case ProjectSettingsBase::RemoveLine:
         break;
   }
   
   _autoCheckButton->setChecked(_settings.autoSyntaxCheck);
   _saveObsoleteButton->setChecked(_settings.saveObsolete);

   _dateFormatEdit->setText(_settings.customDateFormat);

   switch(_settings.dateFormat)
   {
      case Qt::ISODate:
         _defaultDateButton->setChecked(true);
         break;
      case Qt::LocalDate:
         _localDateButton->setChecked(true);
         break;
      case Qt::TextDate:
         _customDateButton->setChecked(true);
         break;
   }

   _autoSaveDelay->setValue( _settings.autoSaveDelay );
}


void SavePreferences::customDateActivated(bool on)
{
   _dateFormatEdit->setEnabled(on);
   _dateFormatEdit->setFocus();
}

void SavePreferences::setAutoSaveVisible( const bool on )
{
    if( on ) _autoSaveBox->show();
    else _autoSaveBox->hide();
}



IdentityPreferences::IdentityPreferences(QWidget* parent, const QString& project)
         : QWidget(parent)
{
    QWidget* page = this;
    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    if( !project.isEmpty() )
    {
	// show the project name in the widget at the top
	layout->addWidget(new QLabel(i18n("<font size=\"+1\">Project: %1</font>").arg(project),page));
    }

    QGroupBox* group = new QGroupBox(2,Qt::Horizontal,page);
    layout->addWidget(group);
    group->setMargin(KDialog::marginHint());
    
    QLabel* tempLabel=new QLabel(i18n("&Name:"),group);
    _nameEdit = new QLineEdit(group, "kcfg_AuthorName");
    tempLabel->setBuddy(_nameEdit);

    tempLabel=new QLabel(i18n("Localized na&me:"),group);
    _localNameEdit = new QLineEdit(group, "kcfg_LocalAuthorName");
    tempLabel->setBuddy(_localNameEdit);

    tempLabel=new QLabel(i18n("E&mail:"),group);
    _mailEdit = new QLineEdit(group, "kcfg_AuthorEmail");
    tempLabel->setBuddy(_mailEdit);


    tempLabel=new QLabel(i18n("&Full language name:"),group);

    QHBox *hbox = new QHBox(group);
    hbox->setSpacing(KDialog::spacingHint());
    _langEdit = new QLineEdit(hbox, "kcfg_Language");
    tempLabel->setBuddy(_langEdit);
    tempLabel=new QLabel(i18n("Lan&guage code:"),hbox);
    _langCodeEdit = new QLineEdit(hbox, "kcfg_LanguageCode");
    tempLabel->setBuddy(_langCodeEdit);
    connect(_langCodeEdit,SIGNAL(textChanged(const QString&)), this
            , SLOT(checkTestPluralButton()));

    tempLabel=new QLabel(i18n("&Language mailing list:"),group);
    _listEdit = new QLineEdit(group, "kcfg_Mailinglist");
    _listEdit->setMinimumSize(100,_listEdit->sizeHint().height());
    tempLabel->setBuddy(_listEdit);
 
    tempLabel=new QLabel(i18n("&Timezone:"), group);
    _timezoneEdit = new QLineEdit(group, "kcfg_Timezone");
    _timezoneEdit->setMinimumSize(100,_timezoneEdit->sizeHint().height());
    tempLabel->setBuddy(_timezoneEdit);


    QString whatsThisMsg=i18n("<qt><p><b>Identity</b></p>\n"
"<p>Fill in information about you and your translation team.\n"
"This information is used when updating the header of a file.</p>\n"
"<p>You can find the options if and what fields in the header should be updated\n"
"on page <b>Save</b> in this dialog.</p></qt>");

    QWhatsThis::add(group,whatsThisMsg);


    group = new QGroupBox(1,Qt::Horizontal,page);
    layout->addWidget(group);
    group->setMargin(KDialog::marginHint());

    hbox = new QHBox(group);
    hbox->setSpacing(KDialog::spacingHint());

    QLabel *label = new QLabel(i18n("&Number of singular/plural forms:"), hbox);
    _pluralFormsBox = new QSpinBox(0,100,1,hbox, "kcfg_PluralForms");
    _pluralFormsBox->setSpecialValueText(
            i18n("automatic choose number of plural forms","Automatic"));
    label->setBuddy(_pluralFormsBox);
    connect(_pluralFormsBox,SIGNAL(valueChanged(int)), this
            , SLOT(checkTestPluralButton()));
    
    hbox->setStretchFactor(_pluralFormsBox,1);

    _testPluralButton = new QPushButton(i18n("Te&st"),hbox);
    _testPluralButton->setEnabled(false);
    connect(_testPluralButton, SIGNAL(clicked()), this
            , SLOT(testPluralForm()));

    const QString msg=i18n("<qt><p><b>Number of singular/plural forms</b></p>"
            "<p><b>Note</b>: This option is KDE specific. "
            "If you are not translating a KDE application, you can safely "
            "ignore this option.</p>"
            "<p>Choose here how many singular and plural forms are used in "
            "your language. "
            "This number must correspond to the settings of your language "
            "team.</p>"
            "<p>Alternatively, you can set this option to "
            "<i>Automatic</i> and KBabel will try to get this information "
            "automatically from KDE. Use the <i>Test</i> button "
            "to test if it can find it out.</p></qt>");
    QWhatsThis::add(_pluralFormsBox,msg);
    QWhatsThis::add(_testPluralButton,msg);
    
    QVBox* vbox = new QVBox(group);
    vbox->setSpacing(KDialog::spacingHint());

    label = new QLabel(i18n("&GNU plural form header:"), vbox);
    
    hbox = new QHBox(vbox);
    hbox->setSpacing(KDialog::spacingHint());
    
    _gnuPluralFormHeaderEdit = new QLineEdit(hbox, "kcfg_PluralFormsHeader");
    label->setBuddy(_gnuPluralFormHeaderEdit);
    
    hbox->setStretchFactor(_gnuPluralFormHeaderEdit,1);

    _testGnuPluralFormButton = new QPushButton(i18n("&Lookup"),hbox);
    connect(_testGnuPluralFormButton, SIGNAL(clicked()), this
            , SLOT(lookupGnuPluralFormHeader()));

    _checkPluralArgumentBox = new QCheckBox( i18n("Re&quire plural form arguments in translation")
	, group, "kcfg_CheckPluralArgument" );
    QWhatsThis::add(_checkPluralArgumentBox, 
	i18n("<qt><p><b>Require plural form arguments in translation</b></p>\n"
            "<p><b>Note</b>: This option is KDE specific at the moment. "
            "If you are not translating a KDE application, you can safely "
            "ignore this option.</p>\n"
	    "<p>If is this option enabled, the validation check will "
	    "require the %n argument to be present in the message.</p></qt>"));

    QWhatsThis::add(_gnuPluralFormHeaderEdit, 
	i18n("<qt><p><b>GNU plural form header</b></p>\n"
            "<p>Here you can fill a header entry for GNU plural form handling; "
            "if you leave the entry empty, the entry in the PO file will not be "
	    "changed or added.</p>\n"
	    "<p>KBabel can automatically try to determine value suggested by the "
	    "GNU gettext tools for currently set language; just press the <b>Lookup</b> "
	    "button.</p></qt>"));

    layout->addStretch(1);

    page->setMinimumSize(sizeHintForWidget(page));

    setMinimumSize(sizeHint());

    _mailEdit->installEventFilter(this);
    _listEdit->installEventFilter(this);
}

void IdentityPreferences::defaults(const IdentitySettings& settings)
{
    _nameEdit->setText(settings.authorName);
    _localNameEdit->setText(settings.authorLocalizedName);
    _langEdit->setText(settings.languageName);
    _langCodeEdit->setText(settings.languageCode);
    _listEdit->setText(settings.mailingList);
    _timezoneEdit->setText(settings.timeZone);
    _pluralFormsBox->setValue(settings.numberOfPluralForms);
    _gnuPluralFormHeaderEdit->setText(settings.gnuPluralFormHeader);
    _checkPluralArgumentBox->setChecked(settings.checkPluralArgument);
}

bool IdentityPreferences::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Drop)
    {
        QDropEvent *de = static_cast<QDropEvent*>(e);
        KURL::List urlList;
        if(de && KURLDrag::decode(de,urlList))
        {
            KURL url(urlList.first());
            if(url.protocol()== "mailto")
            {
                QString mail=url.path();

                bool handled=false;
                if(o == _mailEdit)
                {
                    handled=true;
                    _mailEdit->setText(mail);
                }
                else if(o == _listEdit)
                {
                    handled=true;
                    _listEdit->setText(mail);
                }

                if(handled)
                    return true;
            }
        }
    }

    return false;
}

void IdentityPreferences::checkTestPluralButton()
{
    int val = _pluralFormsBox->value();
    QString lang=_langCodeEdit->text();
    
    _testPluralButton->setEnabled(val==0 && !lang.isEmpty());
}

void IdentityPreferences::testPluralForm()
{
    QString lang=_langCodeEdit->text();

    if(lang.isEmpty())
    {
        KMessageBox::sorry(this,i18n("Please insert a language code first."));
        return;
    }

    int number=Catalog::getNumberOfPluralForms(lang);

    QString msg;
    
    if(number < 0)
    {
        msg = i18n("It is not possible to find out the number "
                "of singular/plural forms automatically for the "
                "language code \"%1\".\n"
                "Do you have kdelibs.po installed for this language?\n"
                "Please set the correct number manually.").arg(lang);
    }
    else
    {
        msg = i18n("The number of singular/plural forms found for "
                "the language code \"%1\" is %2.").arg(lang).arg(number);
    }

    if(!msg.isEmpty())
    {
        KMessageBox::information(this,msg);
    }
}

void IdentityPreferences::lookupGnuPluralFormHeader()
{
    QString lang=_langCodeEdit->text();

    if(lang.isEmpty())
    {
        KMessageBox::sorry(this,i18n("Please insert a language code first."));
        return;
    }

    QString header=GNUPluralForms(lang);
    
    if( header.isEmpty() )
    {
	KMessageBox::information(this, i18n("It was not possible to determine "
	"GNU header for plural forms. Maybe your GNU gettext tools are too "
	"old or they do not contain a suggested value for your language.") );
    }
    else
    {
	_gnuPluralFormHeaderEdit->setText( header );
    }
}


MiscPreferences::MiscPreferences(QWidget *parent)
                : QWidget(parent), _regExpEditDialog(0)
{
    QWidget* page = this;

    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* box=new QGroupBox(1,Qt::Horizontal,page);
    box->setMargin(KDialog::marginHint());
    layout->addWidget(box);

    QHBox *hbox = new QHBox(box);
    hbox->setSpacing(KDialog::spacingHint());

    QLabel *label = new QLabel(i18n("&Marker for keyboard accelerator:"),hbox);
    accelMarkerEdit = new KLineEdit(hbox, "kcfg_AccelMarker");
    accelMarkerEdit->setMaxLength(1);
    label->setBuddy(accelMarkerEdit);
    hbox->setStretchFactor(accelMarkerEdit,1);
    QString msg=i18n("<qt><p><b>Marker for keyboard accelerator</b></p>"
        "<p>Define here, what character marks the following "
        "character as keyboard accelerator. For example in Qt it is "
        "'&amp;' and in Gtk it is '_'.</p></qt>");
    QWhatsThis::add(label,msg);
    QWhatsThis::add(accelMarkerEdit,msg);


    hbox = new QHBox(box);
    hbox->setSpacing(KDialog::spacingHint());

    label = new QLabel(i18n("&Regular expression for context information:")
            ,hbox);
    contextInfoEdit = new KLineEdit(hbox, "kcfg_ContextInfo");
    label->setBuddy(contextInfoEdit);
    hbox->setStretchFactor(contextInfoEdit,1);

    msg=i18n("<qt><p><b>Regular expression for context information</b></p>"
        "<p>Enter a regular expression here which defines what is "
        "context information in the message and must not get "
        "translated.</p></qt>");
    QWhatsThis::add(label,msg);
    QWhatsThis::add(contextInfoEdit,msg);

    if( !KTrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty() )
    {
	_regExpButton = new QPushButton( i18n("&Edit..."), hbox );
	connect( _regExpButton, SIGNAL( clicked() ), this, SLOT( regExpButtonClicked()));
    }


    // preferences for mail attachments
    QVButtonGroup* vbgroup = new QVButtonGroup(page);
    vbgroup->setTitle(i18n("Compression Method for Mail Attachments"));
    vbgroup->setRadioButtonExclusive(true);
    vbgroup->setMargin(KDialog::marginHint());
    layout->addWidget(vbgroup);

    bzipButton = new QRadioButton(i18n("tar/&bzip2"), vbgroup, "kcfg_BZipCompression");
    gzipButton = new QRadioButton(i18n("tar/&gzip"), vbgroup);

    compressSingle = new QCheckBox(i18n("&Use compression when sending "
					"a single file"), vbgroup, "kcfg_CompressSingleFile");

    layout->addStretch(1);
    page->setMinimumSize(sizeHintForWidget(page));
}

void MiscPreferences::defaults(const MiscSettings& settings)
{
    accelMarkerEdit->setText(settings.accelMarker);
    contextInfoEdit->setText(settings.contextInfo.pattern());
    if( settings.useBzip )
	bzipButton->setChecked (true);
    else
	gzipButton->setChecked (true);
	
    compressSingle->setChecked(settings.compressSingleFile);
}

QString MiscPreferences::contextInfo() const
{
    QString temp=contextInfoEdit->text();

    bool quoted=false;
    QString newStr;

    for(uint i=0; i<temp.length(); i++)
    {
        if(temp[i]=='n')
        {
            quoted=!quoted;
            newStr+=temp[i];
        }
        else if(temp[i]=='n' && quoted)
        {
            newStr[newStr.length()-1]='\n';
            quoted=false;
        }
        else
        {
            quoted=false;
            newStr+=temp[i];
        }
    }

    return newStr;
}

void MiscPreferences::setContextInfo(QString reg)
{
    reg.replace("\n","\\n");
    contextInfoEdit->setText(reg);
}

void MiscPreferences::regExpButtonClicked()
{
    if ( _regExpEditDialog==0 )
      _regExpEditDialog = KParts::ComponentFactory::createInstanceFromQuery<QDialog>
	("KRegExpEditor/KRegExpEditor", QString::null, this );

    KRegExpEditorInterface *iface = dynamic_cast<KRegExpEditorInterface *>( _regExpEditDialog );
    if( iface )
    {
	iface->setRegExp( contextInfoEdit->text() );
	if( _regExpEditDialog->exec() == QDialog::Accepted )
	    contextInfoEdit->setText( iface->regExp() );
    }
}


SpellPreferences::SpellPreferences(QWidget* parent)
         : QWidget(parent)
{
    QWidget* page = this;
    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());


    onFlyBtn = new QCheckBox(i18n("On the &fly spellchecking"),page, "kcfg_OnFlySpellCheck");
    layout->addWidget(onFlyBtn);

    QWhatsThis::add(onFlyBtn, i18n("<qt><p><b>On the fly spellchecking</b></p>"
        "<p>Activate this to let KBabel spell check the text "
	"as you type. Mispelled words will be colored by the error color.</p></qt>"));
	
    spellConfig = new KSpellConfig(page,"spellConfigWidget",0,false);
    layout->addWidget(spellConfig);
    remIgnoredBtn = new QCheckBox(i18n("&Remember ignored words"),page, "kcfg_RememberIgnored");
    layout->addWidget(remIgnoredBtn);
    
    connect( spellConfig, SIGNAL( configChanged() )
	, this, SIGNAL ( settingsChanged() ) );

    QLabel *tempLabel = new QLabel(i18n("F&ile to store ignored words:"),page);
    layout->addWidget(tempLabel);
    ignoreURLEdit = new KURLRequester(page, "kcfg_IgnoreURL");
    layout->addWidget(ignoreURLEdit);
    tempLabel->setBuddy(ignoreURLEdit);

    connect(remIgnoredBtn,SIGNAL(toggled(bool)),ignoreURLEdit
                        ,SLOT(setEnabled(bool)));


    QString msg = i18n("<qt><p><b>Remember ignored words</b></p>"
        "<p>Activate this, to let KBabel ignore the words, where you have "
        "chosen <i>Ignore All</i> in the spell check dialog, "
        "in every spell check.</p></qt>");

    QWhatsThis::add(remIgnoredBtn,msg);
    QWhatsThis::add(tempLabel,msg);
    QWhatsThis::add(ignoreURLEdit,msg);

    layout->addStretch(1);

    page->setMinimumSize(sizeHintForWidget(page));

    setMinimumSize(sizeHint());
}



void SpellPreferences::updateWidgets(const SpellcheckSettings& settings)
{
    spellConfig->setClient(settings.spellClient);
    spellConfig->setNoRootAffix(settings.noRootAffix);
    spellConfig->setRunTogether(settings.runTogether);
    spellConfig->setEncoding(settings.spellEncoding);
    spellConfig->setDictionary(settings.spellDict);
}


void SpellPreferences::mergeSettings(SpellcheckSettings& settings) const
{
    settings.noRootAffix=spellConfig->noRootAffix();
    settings.runTogether=spellConfig->runTogether();
    settings.spellClient=spellConfig->client();
    settings.spellEncoding=spellConfig->encoding();
    settings.spellDict=spellConfig->dictionary();

    settings.valid=true;
}

void SpellPreferences::defaults(const SpellcheckSettings& settings)
{
    remIgnoredBtn->setChecked(settings.rememberIgnored);
    ignoreURLEdit->setURL(settings.ignoreURL);
    
    onFlyBtn->setChecked(settings.onFlySpellcheck);

    KSpellConfig spCfg;
    *spellConfig = spCfg;
}

CatmanPreferences::CatmanPreferences(QWidget* parent)
         : QWidget(parent)
{
    QWidget* page = this;

    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* box=new QGroupBox(1,Qt::Horizontal,page);
    box->setMargin(KDialog::marginHint());
    layout->addWidget(box);

    QLabel* label=new QLabel(i18n("&Base folder of PO files:"),box);
    QHBox* hbox = new QHBox(box);
    hbox->setSpacing(KDialog::spacingHint());

    const KFile::Mode mode = static_cast<KFile::Mode>( KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly );
	
												
    _poDirEdit = new KURLRequester(hbox, "kcfg_PoBaseDir");
    _poDirEdit->setMode( mode );
    _poDirEdit->setMinimumSize(250,_poDirEdit->sizeHint().height());
    label->setBuddy(_poDirEdit);


    label=new QLabel(i18n("Ba&se folder of POT files:"),box);
    hbox = new QHBox(box);
    hbox->setSpacing(KDialog::spacingHint());

    _potDirEdit = new KURLRequester(hbox, "kcfg_PotBaseDir");
    _potDirEdit->setMode( mode );
    _potDirEdit->setMinimumSize(250,_potDirEdit->sizeHint().height());
    label->setBuddy(_potDirEdit);



    QWhatsThis::add(box,i18n("<qt><p><b>Base folders</b></p>\n"
     "<p>Type in the folders which contain all your PO and POT files.\n"
     "The files and the folders in these folders will then be merged into one\n"
     "tree.</p></qt>"));


    box=new QGroupBox(1,Qt::Horizontal,page);
    box->setMargin(KDialog::marginHint());
    layout->addWidget(box);

    _openWindowButton = new QCheckBox(i18n("O&pen files in new window"),box, "kcfg_OpenWindow");


    QWhatsThis::add(_openWindowButton,i18n("<qt><p><b>Open files in new window</b></p>\n"
"<p>If this is activated all files that are opened from the Catalog Manager are opened\n"
"in a new window.</p></qt>"));

    _killButton = new QCheckBox( i18n("&Kill processes on exit") , box, "kcfg_KillCmdOnExit" );

    QWhatsThis::add( _killButton , i18n("<qt><p><b>Kill processes on exit</b></p>\n"
"<p>If you check this, KBabel tries to kill the processes, that have not exited already when KBabel exits,\n"
"by sending a kill signal to them.</p>\n"
"<p>NOTE: It is not guaranteed that the processes will be killed.</p></qt>") );


    _indexButton = new QCheckBox( i18n("Create inde&x for file contents"), box, "kcfg_IndexWords" );
    
    QWhatsThis::add( _indexButton , i18n("<qt><p><b>Create index for file contents</b></p>\n"
"<p>If you check this, KBabel will create an index for each PO file to speed up the find/replace functions.</p>\n"
"<p>NOTE: This will slow down updating the file information considerably.</p></qt>") );

    m_msgfmtButton = new QCheckBox( i18n("Run &msgfmt before processing a file"), box, "kcfg_msgfmt" );

    QWhatsThis::add( m_msgfmtButton, i18n("<qt><p><b>Run msgfmt before processing a file</b></p>"
        "<p>If you enable this, KBabel will run Gettext's "
        "msgfmt tool before processing a file.</p>"
        "<p>Enabling this setting is recommended, even if it causes processing to be slower. "
        "This setting is enabled by default.</p>"
        "<p>Disabling is useful for slow computers and when you want "
        "to translate PO files that are not supported by the current version "
        "of the Gettext tools that are on your system. "
        "The drawback of disabling is that hardly any syntax checking is done by the processing code, "
        "so invalid PO files could be shown as good ones, "
        "even if Gettext tools would reject such files.</p></qt>") );

    layout->addStretch(1);

    page->setMinimumSize(sizeHintForWidget(page));

    setMinimumSize(sizeHint());
}


void CatmanPreferences::defaults(const CatManSettings& settings)
{
   _poDirEdit->setURL(settings.poBaseDir);
   _potDirEdit->setURL(settings.potBaseDir);

   _openWindowButton->setChecked(settings.openWindow);

   _killButton->setChecked(settings.killCmdOnExit );
   _indexButton->setChecked(settings.indexWords );
   m_msgfmtButton->setChecked( settings.msgfmt );
}

DirCommandsPreferences::DirCommandsPreferences(QWidget* parent)
         : QWidget(parent)
{
    QWidget* page = this;

    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* box = new QGroupBox( 1 , Qt::Horizontal , i18n("Commands for Folders") , page );
    box->setMargin( KDialog::marginHint() );
    layout->addWidget( box );

    _dirCmdEdit = new CmdEdit( box );
    new QLabel( i18n("Replaceables:\n@PACKAGE@, @PODIR@, @POTDIR@\n"
	"@POFILES@, @MARKEDPOFILES@"), box);
	
    connect (_dirCmdEdit, SIGNAL(widgetChanged()), this, SIGNAL(settingsChanged()));

    QWhatsThis::add( box , i18n("<qt><p><b>Commands for folders</b></p>"
"<p>Insert here the commands you want to execute in folders from "
"the Catalog Manager. The commands are then shown in the submenu "
"<b>Commands</b> in the Catalog Manager's context menu.</p>"
"<p>The following strings will be replaced in a command:<ul>"
"<li>@PACKAGE@: The name of the folder without path</li>"
"<li>@PODIR@: The name of the PO-folder with path</li>"
"<li>@POTDIR@: The name of the template folder with path</li>"
"<li>@POFILES@: The names of the PO files with path</li>"
"<li>@MARKEDPOFILES@: The names of the marked PO files with path</li>"
"</ul></p>"
"</qt>") );



    layout->addStretch(1);
    page->setMinimumSize(sizeHintForWidget(page));

    setMinimumSize(sizeHint());
}


DirCommandsPreferences::~DirCommandsPreferences()
{
}


void DirCommandsPreferences::updateWidgets(const CatManSettings& settings)
{
   _dirCmdEdit->setCommands( settings.dirCommands , settings.dirCommandNames );
}


void DirCommandsPreferences::mergeSettings(CatManSettings& settings) const
{
    _dirCmdEdit->commands( settings.dirCommands , settings.dirCommandNames );
}

void DirCommandsPreferences::defaults(const CatManSettings& settings)
{
   _dirCmdEdit->setCommands( settings.dirCommands, settings.dirCommandNames );
}


FileCommandsPreferences::FileCommandsPreferences(QWidget* parent)
         : QWidget(parent)
{
    QWidget* page = this;

    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* box=new QGroupBox( 1 , Qt::Horizontal , i18n("Commands for Files") , page );
    box->setMargin( KDialog::marginHint() );
    layout->addWidget( box );

    _fileCmdEdit = new CmdEdit( box );
    new QLabel( i18n("Replaceables:\n"
"@PACKAGE@, @POFILE@,@POTFILE@,\n@PODIR@, @POTDIR@"), box);

    connect (_fileCmdEdit, SIGNAL(widgetChanged()), this, SIGNAL(settingsChanged()));

    QWhatsThis::add( box , i18n("<qt><p><b>Commands for files</b></p>"
"<p>Insert here the commands you want to execute on files from "
"the Catalog Manager. The commands are then shown in the submenu "
"<b>Commands</b> in the Catalog Manager's context menu.</p>"
"<p>The following strings will be replaced in a command:<ul>"
"<li>@PACKAGE@: The name of the file without path and extension</li>"
"<li>@POFILE@: The name of the PO-file with path and extension</li>"
"<li>@POTFILE@: The name of the corresponding template file with path "
"and extension</li>"
"<li>@POEMAIL@: The name and email address of the last translator</li>"
"<li>@PODIR@: The name of the folder the PO-file is in, with path</li>"
"<li>@POTDIR@: The name of the folder the template file is in, with "
"path</li></ul></p></qt>") );



    layout->addStretch(1);
    page->setMinimumSize(sizeHintForWidget(page));

    setMinimumSize(sizeHint());
}


FileCommandsPreferences::~FileCommandsPreferences()
{
}


void FileCommandsPreferences::updateWidgets(const CatManSettings& settings)
{
   _fileCmdEdit->setCommands( settings.fileCommands , settings.fileCommandNames );
}


void FileCommandsPreferences::mergeSettings(CatManSettings& settings) const
{
    _fileCmdEdit->commands( settings.fileCommands , settings.fileCommandNames );
}

void FileCommandsPreferences::defaults(const CatManSettings& settings)
{
   _fileCmdEdit->setCommands( settings.fileCommands, settings.fileCommandNames );
}

ViewPreferences::ViewPreferences(QWidget* parent)
         : QWidget(parent)
{
    QWidget* page = this;

    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    QGroupBox* box=new QGroupBox(2, Qt::Horizontal,i18n("Shown Columns"),page);
    box->setMargin(KDialog::marginHint());
    layout->addWidget(box);

    _flagColumnCheckbox = new QCheckBox( i18n("Fla&g"), box, "kcfg_ShowFlagColumn" );
    _fuzzyColumnCheckbox = new QCheckBox( i18n("&Fuzzy"), box, "kcfg_ShowFuzzyColumn" );
    _untranslatedColumnCheckbox = new QCheckBox( i18n("&Untranslated"), box, "kcfg_ShowUntranslatedColumn" );
    _totalColumnCheckbox = new QCheckBox( i18n("&Total"), box, "kcfg_ShowTotalColumn" );
    _cvsColumnCheckbox = new QCheckBox( i18n("SVN/&CVS status"), box, "kcfg_ShowCVSColumn" );
    _revisionColumnCheckbox = new QCheckBox( i18n("Last &revision"), box, "kcfg_ShowRevisionColumn" );
    _translatorColumnCheckbox = new QCheckBox( i18n("Last t&ranslator"), box, "kcfg_ShowTranslatorColumn" );

    QWhatsThis::add(box,i18n("<qt><p><b>Shown columns</b></p>\n"
     "<p></p></qt>"));
     
    layout->addStretch(1);

    page->setMinimumSize(sizeHintForWidget(page));

    setMinimumSize(sizeHint());
}


void ViewPreferences::defaults(const CatManSettings& _settings)
{
    _flagColumnCheckbox->setChecked(_settings.flagColumn);
    _fuzzyColumnCheckbox->setChecked(_settings.fuzzyColumn);
    _untranslatedColumnCheckbox->setChecked(_settings.untranslatedColumn);
    _totalColumnCheckbox->setChecked(_settings.totalColumn);
    _cvsColumnCheckbox->setChecked(_settings.cvsColumn);
    _revisionColumnCheckbox->setChecked(_settings.revisionColumn);
    _translatorColumnCheckbox->setChecked(_settings.translatorColumn);
}

SourceContextPreferences::SourceContextPreferences(QWidget* parent): QWidget(parent)
{
    QWidget* page = this;
    QVBoxLayout* layout=new QVBoxLayout(page);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());
    
    QHBox* box = new QHBox(page);
    box->setSpacing(KDialog::spacingHint());
    QLabel* tempLabel=new QLabel(i18n("&Base folder for source code:"),box);

    const KFile::Mode mode = static_cast<KFile::Mode>( KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly );
    _coderootEdit = new KURLRequester ( box, "kcfg_CodeRoot" );
    _coderootEdit->setMode( mode );
    _coderootEdit->setMinimumSize( 250, _coderootEdit->sizeHint().height() );
    tempLabel->setBuddy( _coderootEdit );
    layout->addWidget(box);
												
    // FIXME: use KConfigXT    
    _pathsEditor = new KListEditor(page);
    _pathsEditor->setTitle(i18n("Path Patterns"));
    layout->addWidget(_pathsEditor);
    
    connect ( _pathsEditor, SIGNAL (itemsChanged ())
	, this, SIGNAL (itemsChanged ()));
    
    _pathsEditor->installEventFilter(this);
				    
    setMinimumSize(sizeHint());
}

SourceContextPreferences::~SourceContextPreferences()
{
}

void SourceContextPreferences::mergeSettings(KBabel::SourceContextSettings& settings) const
{
    settings.sourcePaths=_pathsEditor->list();
}

void SourceContextPreferences::updateWidgets(const KBabel::SourceContextSettings& settings)
{
    _pathsEditor->setList(settings.sourcePaths);
}

void SourceContextPreferences::defaults(const KBabel::SourceContextSettings& settings)
{
    _pathsEditor->setList(settings.sourcePaths);
}

bool SourceContextPreferences::eventFilter( QObject *, QEvent *e )
{
    if( e->type() == QEvent::KeyPress )
    {
        QKeyEvent *ke = dynamic_cast<QKeyEvent*>(e);
        if( ke->key() == Key_Return || ke->key() == Key_Enter )
            return true;
    }
    return false;
}

#include "projectprefwidgets.moc"
