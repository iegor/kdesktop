/***************************************************************************
                          schemaeditor.cpp  -  description
                             -------------------
    begin                : mar apr 17 16:44:59 CEST 2001
    copyright            : (C) 2001 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define TABLE_COLORS 20


#include "schemaeditor.h"
#include "schemaeditor.moc"

#include <dcopclient.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qwmatrix.h>
#include <qcombobox.h>
#include <kdebug.h>
#include <qcheckbox.h>
#include <kstandarddirs.h>

//#include <errno.h>

#include <qslider.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kinputdialog.h>
#include <qtoolbutton.h>
#include <kmessagebox.h>
#include <ksharedpixmap.h>
#include <kimageeffect.h>
#include <qimage.h>

// SchemaListBoxText is a list box text item with schema filename
class SchemaListBoxText : public QListBoxText
{
  public:
    SchemaListBoxText(const QString &title, const QString &filename): QListBoxText(title)
    {
      m_filename = filename;
    };

    const QString filename() { return m_filename; };

  private:
    QString m_filename;
};


SchemaEditor::SchemaEditor(QWidget * parent, const char *name)
:SchemaDialog(parent, name)
{
    schMod= false;
    loaded = false;
    schemaLoaded = false;
    change = false;
    oldSlot = 0;
    oldSchema = -1;
    color.resize(20);
    type.resize(20);
    bold.resize(20);
    transparent.resize(20);
    defaultSchema = "";
    spix = new KSharedPixmap;

    connect(spix, SIGNAL(done(bool)), SLOT(previewLoaded(bool)));

    DCOPClient *client = kapp->dcopClient();
    if (!client->isAttached())
	client->attach();
    QByteArray data;

    QDataStream args(data, IO_WriteOnly);
    args << 1;
    client->send("kdesktop", "KBackgroundIface", "setExport(int)", data);

    transparencyCheck->setChecked(true);
    transparencyCheck->setChecked(false);

    KGlobal::locale()->insertCatalogue("konsole"); // For schema translations
    connect(imageBrowse, SIGNAL(clicked()), this, SLOT(imageSelect()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveCurrent()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeCurrent()));
//    connect(colorCombo, SIGNAL(activated(int)), this, SLOT(slotColorChanged(int)));
    connect(tcNorm0, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt0, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm1, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt1, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm2, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt2, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm3, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt3, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm4, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt4, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm5, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt5, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm6, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt6, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm7, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt7, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm8, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt8, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcNorm9, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
	connect(tcInt9, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));

    connect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
    connect(shadeColor, SIGNAL(changed(const QColor&)), this, SLOT(updatePreview()));
    connect(shadeSlide, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(transparencyCheck, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(backgndLine, SIGNAL(returnPressed()), this, SLOT(updatePreview()));

    connect(titleLine, SIGNAL(textChanged(const QString&)), this, SLOT(schemaModified()));
    connect(shadeColor, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
    connect(shadeSlide, SIGNAL(valueChanged(int)), this, SLOT(schemaModified()));
    connect(transparencyCheck, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(modeCombo, SIGNAL(activated(int)), this, SLOT(schemaModified()));
    connect(backgndLine, SIGNAL(returnPressed()), this, SLOT(schemaModified()));

    connect(trcNormal0, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal1, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal2, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal3, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal4, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(trcNormal5, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal6, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal7, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal8, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcNormal9, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));

	connect(trcInt0, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt1, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt2, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt3, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt4, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(trcInt5, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt6, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt7, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt8, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(trcInt9, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));

    connect(bcNormal0, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal1, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal2, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal3, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal4, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(bcNormal5, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal6, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal7, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal8, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcNormal9, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt0, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt1, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt2, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt3, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt4, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(bcInt5, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt6, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt7, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt8, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
	connect(bcInt9, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));

	connect(colorButtonNormal0, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt0, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal1, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt1, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal2, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt2, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal3, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt3, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal4, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt4, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal5, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt5, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal6, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt6, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal7, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt7, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal8, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt8, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(colorButtonNormal9, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
	connect(kColorButtonInt9, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));

    connect(backgndLine, SIGNAL(textChanged(const QString&)), this, SLOT(schemaModified()));

    connect(defaultSchemaCB, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    removeButton->setEnabled( schemaList->currentItem() );
}


QString SchemaEditor::schema()
{
    QString filename = defaultSchema;

    int i = schemaList->currentItem();
    if (defaultSchemaCB->isChecked() && i>=0)
      filename = ((SchemaListBoxText *) schemaList->item(i))->filename();

    return filename.section('/',-1);
}


void SchemaEditor::setSchema(QString sch)
{
    defaultSchema = sch;
    sch = locate("data", "konsole/"+sch);

    int sc = -1;
    for (int i = 0; i < (int) schemaList->count(); i++)
	if (sch == ((SchemaListBoxText *) schemaList->item(i))->filename())
	    sc = i;

    oldSchema = sc;
    if (sc == -1)
	sc = 0;
    schemaList->setCurrentItem(sc);
//    readSchema(sc);
}

SchemaEditor::~SchemaEditor()
{
    delete spix;
}



void SchemaEditor::updatePreview()
{

    if (transparencyCheck->isChecked()) {
	if (loaded) {
	    float rx = (100.0 - shadeSlide->value()) / 100;
	    QImage ima(pix.convertToImage());
	    ima = KImageEffect::fade(ima, rx, shadeColor->color());
	    QPixmap pm;
	    pm.convertFromImage(ima);
	    previewPixmap->setPixmap(pm);
	    previewPixmap->setScaledContents(true);
	}
	 else  //try to reload
	{
           if(!spix->loadFromShared(QString("DESKTOP1")))
              kdDebug(0) << "cannot load" << endl;

	}
      } else {
	QPixmap pm;
	pm.load(backgndLine->text());
	if ( pm.isNull() ) {
	    previewPixmap->clear();
	} else {
	    previewPixmap->setPixmap(pm);
	    previewPixmap->setScaledContents(true);
	}
    }

}

void SchemaEditor::previewLoaded(bool l)
{
    if (l) {
	QWMatrix mat;
	pix =
	    spix->xForm(mat.
			scale(180.0 / spix->QPixmap::width(),
			      100.0 / spix->QPixmap::height()));
	kdDebug(0) << "Loaded" << endl;
	loaded = true;
	if (transparencyCheck->isChecked()) {
	    updatePreview();
	}

    } else
	kdDebug(0) << "error loading" << endl;

}


void SchemaEditor::getList()
{
    if (! schemaLoaded) {
	loadAllSchema();
	setSchema(defaultSchema);
	schemaLoaded = true;
	change = true;
    }
}

void SchemaEditor::show()
{
    getList();
    SchemaDialog::show();
}


void SchemaEditor::loadAllSchema(QString currentFile)
{
    QStringList list = KGlobal::dirs()->findAllResources("data", "konsole/*.schema");
    QStringList::ConstIterator it;
    disconnect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
    schemaList->clear();

    QListBoxItem* currentItem = 0;
    for (it = list.begin(); it != list.end(); ++it) {

	QString name = (*it);

	QString title = readSchemaTitle(name);

	// Only insert new items so that local items override global
	if (schemaList->findItem(title, ExactMatch) == 0) {
	    if (title.isNull() || title.isEmpty())
		title=i18n("untitled");

		schemaList->insertItem(new SchemaListBoxText(title, name));
	    if (currentFile==name.section('/',-1))
                currentItem = schemaList->item( schemaList->count()-1 );
	}
    }
    schemaList->sort();
    schemaList->setCurrentItem(0);   // select the first added item correctly too
    schemaList->setCurrentItem(currentItem);
    connect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
    schemaListChanged();
}

void SchemaEditor::imageSelect()
{
    QString start;
    start = backgndLine->text();
    if (start.isEmpty())
    {
        QStringList list=KGlobal::dirs()->resourceDirs("wallpaper");
        if(list.count()>0)
            start= list.last();
    }

    KURL url = KFileDialog::getImageOpenURL(start, 0, i18n("Select Background Image"));
// KURL url=KFileDialog::getOpenURL(start,"",0,i18n("Select Background Image"));
    if(!url.path().isEmpty())
    {
        backgndLine->setText(url.path());
        updatePreview();
    }
}

void SchemaEditor::slotTypeChanged(int slot)
{
    schemaModified();

    //bool active = slot == 0 || slot == 3;
    //colorButton->setEnabled(active);
    //boldCheck->setEnabled(active);
    //transparentCheck->setEnabled(active);
	bool inactive = true;

	inactive = type[0] == 1 || type[0] == 2; colorButtonNormal0->setDisabled(inactive); trcNormal0->setDisabled(inactive); bcNormal0->setDisabled(inactive);
	inactive = type[10] == 1 || type[10] == 2; kColorButtonInt0->setDisabled(inactive); trcInt0->setDisabled(inactive); bcInt0->setDisabled(inactive);

	inactive = type[1] == 1 || type[1] == 2; colorButtonNormal1->setDisabled(inactive); trcNormal1->setDisabled(inactive); bcNormal1->setDisabled(inactive);
	inactive = type[11] == 1 || type[11] == 2; kColorButtonInt1->setDisabled(inactive); trcInt1->setDisabled(inactive); bcInt1->setDisabled(inactive);

	inactive = type[2] == 1 || type[2] == 2; colorButtonNormal2->setDisabled(inactive); trcNormal2->setDisabled(inactive); bcNormal2->setDisabled(inactive);
	inactive = type[12] == 1 || type[12] == 2; kColorButtonInt2->setDisabled(inactive); trcInt2->setDisabled(inactive); bcInt2->setDisabled(inactive);

	inactive = type[3] == 1 || type[13] == 2; colorButtonNormal3->setDisabled(inactive); trcNormal3->setDisabled(inactive); bcNormal3->setDisabled(inactive);
	inactive = type[13] == 1 || type[13] == 2; kColorButtonInt3->setDisabled(inactive); trcInt3->setDisabled(inactive); bcInt3->setDisabled(inactive);

	inactive = type[4] == 1 || type[4] == 2; colorButtonNormal4->setDisabled(inactive); trcNormal4->setDisabled(inactive); bcNormal4->setDisabled(inactive);
	inactive = type[14] == 1 || type[14] == 2; kColorButtonInt4->setDisabled(inactive); trcInt4->setDisabled(inactive); bcInt4->setDisabled(inactive);

	inactive = type[5] == 1 || type[5] == 2; colorButtonNormal5->setDisabled(inactive); trcNormal5->setDisabled(inactive); bcNormal5->setDisabled(inactive);
	inactive = type[15] == 1 || type[15] == 2; kColorButtonInt5->setDisabled(inactive); trcInt5->setDisabled(inactive); bcInt5->setDisabled(inactive);

	inactive = type[6] == 1 || type[6] == 2; colorButtonNormal6->setDisabled(inactive); trcNormal6->setDisabled(inactive); bcNormal6->setDisabled(inactive);
	inactive = type[16] == 1 || type[16] == 2; kColorButtonInt6->setDisabled(inactive); trcInt6->setDisabled(inactive); bcInt6->setDisabled(inactive);

	inactive = type[7] == 1 || type[7] == 2; colorButtonNormal7->setDisabled(inactive); trcNormal7->setDisabled(inactive); bcNormal7->setDisabled(inactive);
	inactive = type[17] == 1 || type[17] == 2; kColorButtonInt7->setDisabled(inactive); trcInt7->setDisabled(inactive); bcInt7->setDisabled(inactive);

	inactive = type[8] == 1 || type[8] == 2; colorButtonNormal8->setDisabled(inactive); trcNormal8->setDisabled(inactive); bcNormal8->setDisabled(inactive);
	inactive = type[18] == 1 || type[18] == 2; kColorButtonInt8->setDisabled(inactive); trcInt8->setDisabled(inactive); bcInt8->setDisabled(inactive);

	inactive = type[9] == 1 || type[9] == 2; colorButtonNormal9->setDisabled(inactive); trcNormal9->setDisabled(inactive); bcNormal9->setDisabled(inactive);
	inactive = type[19] == 1 || type[19] == 2; kColorButtonInt9->setDisabled(inactive); trcInt9->setDisabled(inactive); bcInt9->setDisabled(inactive);
}


// void SchemaEditor::slotColorChanged(int slot)
// {
//     kdDebug(0) << slot << endl;
//     color[oldSlot] = colorButton->color();
//     type[oldSlot] = typeCombo->currentItem();
//     bold[oldSlot] = boldCheck->isChecked();
//     transparent[oldSlot] = transparentCheck->isChecked();
// 
//     change = false; // Don't mark as modified
//     transparentCheck->setChecked(transparent[slot]);
//     boldCheck->setChecked(bold[slot]);
//     typeCombo->setCurrentItem(type[slot]);
//     colorButton->setColor(color[slot]);
//     oldSlot = slot;
//     change = true;
// }

void SchemaEditor::removeCurrent()
{
    int i = schemaList->currentItem();
    if(i==-1)
        return;
    QString base = ((SchemaListBoxText *) schemaList->item(i))->filename();

    // Query if system schemas should be removed
    if (locateLocal("data", "konsole/" + base.section('/', -1)) != base) {
	int code = KMessageBox::warningContinueCancel(this,
	    i18n("You are trying to remove a system schema. Are you sure?"),
	    i18n("Removing System Schema"),
	    KGuiItem(i18n("&Delete"), "editdelete"));
	if (code != KMessageBox::Continue)
	    return;
    }

    QString base_filename = base.section('/',-1);

    if(base_filename==schema())
     setSchema("");

    if (!QFile::remove(base))
	KMessageBox::error(this,
			   i18n("Cannot remove the schema.\nMaybe it is a system schema.\n"),
			   i18n("Error Removing Schema"));

    loadAllSchema();

    setSchema(defaultSchema);

}

void SchemaEditor::saveCurrent()
{

    //This is to update the color table
    //colorCombo->setCurrentItem(0);
    //slotColorChanged(0);

	schemaModified();

    QString fullpath;
    if (schemaList->currentText() == titleLine->text()) {
	int i = schemaList->currentItem();
	fullpath = ((SchemaListBoxText *) schemaList->item(i))->filename().section('/',-1);
    }
    else {
	// Only ask for a name for changed titleLine, considered a "save as"
	fullpath = titleLine->text().stripWhiteSpace().simplifyWhiteSpace()+".schema";

    bool ok;
    fullpath = KInputDialog::getText( i18n( "Save Schema" ),
        i18n( "File name:" ), fullpath, &ok, this );
	if (!ok) return;
    }

    if (fullpath[0] != '/')
        fullpath = KGlobal::dirs()->saveLocation("data", "konsole/") + fullpath;

    QFile f(fullpath);
    if (f.open(IO_WriteOnly)) {
	QTextStream t(&f);
        t.setEncoding( QTextStream::UnicodeUTF8 );

	t << "# schema for konsole autogenerated with the schema editor" << endl;
	t << endl;
	t << "title " << titleLine->text() << endl; // Use title line as schema title
	t << endl;
	if (transparencyCheck->isChecked()) {
	    QColor c = shadeColor->color();
	    QString tra;
	    tra.sprintf("transparency %1.2f %3d %3d %3d",
			1.0 * (100 - shadeSlide->value()) / 100, c.red(), c.green(), c.blue());
	    t << tra << endl;
	}

	if (!backgndLine->text().isEmpty()) {
	    QString smode;
	    int mode;
	    mode = modeCombo->currentItem();
	    if (mode == 0)
		smode = "tile";
	    if (mode == 1)
		smode = "center";
	    if (mode == 2)
		smode = "full";

	    QString image;
	    image.sprintf("image %s %s",
			  (const char *) smode.latin1(),
			  (const char *) backgndLine->text().utf8());
	    t << image << endl;
	}
	t << endl;
	t << "# foreground colors" << endl;
	t << endl;
	t << "# note that the default background color is flagged" << endl;
	t << "# to become transparent when an image is present." << endl;
	t << endl;
	t << "#   slot    transparent bold" << endl;
	t << "#      | red grn blu  | |" << endl;
	t << "#      V V--color--V  V V" << endl;

	for (int i = 0; i < 20; i++) {
	    QString scol;
	    if (type[i] == 0) {
			scol.sprintf("color %2d %3d %3d %3d %2d %1d # %s", i, color[i].red(), color[i].green(), color[i].blue(), transparent[i], bold[i], ""); //(const char *) colorCombo->text(i).utf8());
		}
	    else if (type[i] == 1) {
			scol.sprintf("sysfg %2d %2d %1d # %s", i, transparent[i], bold[i], ""); //(const char *) colorCombo->text(i).utf8());
		}
	    else if (type[i] == 2) {
			scol.sprintf("sysbg %2d %2d %1d # %s", i, transparent[i], bold[i], ""); //(const char *) colorCombo->text(i).utf8());
		}
	    else {
			int ch, cs, cv;
			color[i].hsv(&ch, &cs, &cv);
			scol.sprintf("rcolor %1d %3d %3d %2d %1d # %s", i, cs, cv, transparent[i], bold[i], ""); //(const char *) colorCombo->text(i).utf8());
	    }
	    t << scol << endl;
	}


	f.close();
    } else
	KMessageBox::error(this, i18n("Cannot save the schema.\nMaybe permission denied.\n"),
			   i18n("Error Saving Schema"));

    schMod=false;
    loadAllSchema(fullpath.section('/',-1));
}

void SchemaEditor::schemaModified()
{
	type[0] = tcNorm0->currentItem(); color[0] = colorButtonNormal0->color(); transparent[0] = trcNormal0->isChecked(); bold[0] = bcNormal0->isChecked();
	type[10] = tcInt0->currentItem(); color[10] = kColorButtonInt0->color(); transparent[10] = trcInt0->isChecked(); bold[10] = bcInt0->isChecked();
	type[1] = tcNorm1->currentItem(); color[1] = colorButtonNormal0->color(); transparent[1] = trcNormal0->isChecked(); bold[1] = bcNormal0->isChecked();
	type[11] = tcInt1->currentItem(); color[11] = kColorButtonInt0->color(); transparent[11] = trcInt0->isChecked(); bold[11] = bcInt0->isChecked();
	type[2] = tcNorm2->currentItem(); color[2] = colorButtonNormal0->color(); transparent[2] = trcNormal0->isChecked(); bold[2] = bcNormal0->isChecked();
	type[12] = tcInt2->currentItem(); color[12] = kColorButtonInt0->color(); transparent[12] = trcInt0->isChecked(); bold[12] = bcInt0->isChecked();
	type[3] = tcNorm3->currentItem(); color[3] = colorButtonNormal0->color(); transparent[3] = trcNormal0->isChecked(); bold[3] = bcNormal0->isChecked();
	type[13] = tcInt3->currentItem(); color[13] = kColorButtonInt0->color(); transparent[13] = trcInt0->isChecked(); bold[13] = bcInt0->isChecked();
	type[4] = tcNorm4->currentItem(); color[4] = colorButtonNormal0->color(); transparent[4] = trcNormal0->isChecked(); bold[4] = bcNormal0->isChecked();
	type[14] = tcInt4->currentItem(); color[14] = kColorButtonInt0->color(); transparent[14] = trcInt0->isChecked(); bold[14] = bcInt0->isChecked();
	type[5] = tcNorm5->currentItem(); color[5] = colorButtonNormal0->color(); transparent[5] = trcNormal0->isChecked(); bold[5] = bcNormal0->isChecked();
	type[15] = tcInt5->currentItem(); color[15] = kColorButtonInt0->color(); transparent[15] = trcInt0->isChecked(); bold[15] = bcInt0->isChecked();
	type[6] = tcNorm6->currentItem(); color[6] = colorButtonNormal0->color(); transparent[6] = trcNormal0->isChecked(); bold[6] = bcNormal0->isChecked();
	type[16] = tcInt6->currentItem(); color[16] = kColorButtonInt0->color(); transparent[16] = trcInt0->isChecked(); bold[16] = bcInt0->isChecked();
	type[7] = tcNorm7->currentItem(); color[7] = colorButtonNormal0->color(); transparent[7] = trcNormal0->isChecked(); bold[7] = bcNormal0->isChecked();
	type[17] = tcInt7->currentItem(); color[17] = kColorButtonInt0->color(); transparent[17] = trcInt0->isChecked(); bold[17] = bcInt0->isChecked();
	type[8] = tcNorm8->currentItem(); color[8] = colorButtonNormal0->color(); transparent[8] = trcNormal0->isChecked(); bold[8] = bcNormal0->isChecked();
	type[18] = tcInt8->currentItem(); color[18] = kColorButtonInt0->color(); transparent[18] = trcInt0->isChecked(); bold[18] = bcInt0->isChecked();
	type[9] = tcNorm9->currentItem(); color[9] = colorButtonNormal0->color(); transparent[9] = trcNormal0->isChecked(); bold[9] = bcNormal0->isChecked();
	type[19] = tcInt9->currentItem(); color[19] = kColorButtonInt0->color(); transparent[19] = trcInt0->isChecked(); bold[19] = bcInt0->isChecked();

    if (change) {
		bool inactive = true;
		tcNorm0->setCurrentItem(type[0]); colorButtonNormal0->setColor(color[0]); trcNormal0->setChecked(transparent[0]); bcNormal0->setChecked(bold[0]);
		inactive = type[0] == 1 || type[0] == 2; colorButtonNormal0->setDisabled(inactive); trcNormal0->setDisabled(inactive); bcNormal0->setDisabled(inactive);
		tcInt0->setCurrentItem(type[10]); kColorButtonInt0->setColor(color[10]); trcInt0->setChecked(transparent[10]); bcInt0->setChecked(bold[10]);
		inactive = type[10] == 1 || type[10] == 2; kColorButtonInt0->setDisabled(inactive); trcInt0->setDisabled(inactive); bcInt0->setDisabled(inactive);
	
		tcNorm1->setCurrentItem(type[1]); colorButtonNormal1->setColor(color[1]); trcNormal1->setChecked(transparent[1]); bcNormal1->setChecked(bold[1]);
		inactive = type[1] == 1 || type[1] == 2; colorButtonNormal1->setDisabled(inactive); trcNormal1->setDisabled(inactive); bcNormal1->setDisabled(inactive);
		tcInt1->setCurrentItem(type[11]); kColorButtonInt1->setColor(color[11]); trcInt1->setChecked(transparent[11]); bcInt1->setChecked(bold[11]);
		inactive = type[11] == 1 || type[11] == 2; kColorButtonInt1->setDisabled(inactive); trcInt1->setDisabled(inactive); bcInt1->setDisabled(inactive);
	
		tcNorm2->setCurrentItem(type[2]); colorButtonNormal2->setColor(color[2]); trcNormal2->setChecked(transparent[2]); bcNormal2->setChecked(bold[2]);
		inactive = type[2] == 1 || type[2] == 2; colorButtonNormal2->setDisabled(inactive); trcNormal2->setDisabled(inactive); bcNormal2->setDisabled(inactive);
		tcInt2->setCurrentItem(type[12]); kColorButtonInt2->setColor(color[12]); trcInt2->setChecked(transparent[12]); bcInt2->setChecked(bold[12]);
		inactive = type[12] == 1 || type[12] == 2; kColorButtonInt2->setDisabled(inactive); trcInt2->setDisabled(inactive); bcInt2->setDisabled(inactive);
	
		tcNorm3->setCurrentItem(type[3]); colorButtonNormal3->setColor(color[3]); trcNormal3->setChecked(transparent[3]); bcNormal3->setChecked(bold[3]);
		inactive = type[3] == 1 || type[13] == 2; colorButtonNormal3->setDisabled(inactive); trcNormal3->setDisabled(inactive); bcNormal3->setDisabled(inactive);
		tcInt3->setCurrentItem(type[13]); kColorButtonInt3->setColor(color[13]); trcInt3->setChecked(transparent[13]); bcInt3->setChecked(bold[13]);
		inactive = type[13] == 1 || type[13] == 2; kColorButtonInt3->setDisabled(inactive); trcInt3->setDisabled(inactive); bcInt3->setDisabled(inactive);
	
		tcNorm4->setCurrentItem(type[4]); colorButtonNormal4->setColor(color[4]); trcNormal4->setChecked(transparent[4]); bcNormal4->setChecked(bold[4]);
		inactive = type[4] == 1 || type[4] == 2; colorButtonNormal4->setDisabled(inactive); trcNormal4->setDisabled(inactive); bcNormal4->setDisabled(inactive);
		tcInt4->setCurrentItem(type[14]); kColorButtonInt4->setColor(color[14]); trcInt4->setChecked(transparent[14]); bcInt4->setChecked(bold[14]);
		inactive = type[14] == 1 || type[14] == 2; kColorButtonInt4->setDisabled(inactive); trcInt4->setDisabled(inactive); bcInt4->setDisabled(inactive);
	
		tcNorm5->setCurrentItem(type[5]); colorButtonNormal5->setColor(color[5]); trcNormal5->setChecked(transparent[5]); bcNormal5->setChecked(bold[5]);
		inactive = type[5] == 1 || type[5] == 2; colorButtonNormal5->setDisabled(inactive); trcNormal5->setDisabled(inactive); bcNormal5->setDisabled(inactive);
		tcInt5->setCurrentItem(type[15]); kColorButtonInt5->setColor(color[15]); trcInt5->setChecked(transparent[15]); bcInt5->setChecked(bold[15]);
		inactive = type[15] == 1 || type[15] == 2; kColorButtonInt5->setDisabled(inactive); trcInt5->setDisabled(inactive); bcInt5->setDisabled(inactive);
	
		tcNorm6->setCurrentItem(type[6]); colorButtonNormal6->setColor(color[6]); trcNormal6->setChecked(transparent[6]); bcNormal6->setChecked(bold[6]);
		inactive = type[6] == 1 || type[6] == 2; colorButtonNormal6->setDisabled(inactive); trcNormal6->setDisabled(inactive); bcNormal6->setDisabled(inactive);
		tcInt6->setCurrentItem(type[16]); kColorButtonInt6->setColor(color[16]); trcInt6->setChecked(transparent[16]); bcInt6->setChecked(bold[16]);
		inactive = type[16] == 1 || type[16] == 2; kColorButtonInt6->setDisabled(inactive); trcInt6->setDisabled(inactive); bcInt6->setDisabled(inactive);
	
		tcNorm7->setCurrentItem(type[7]); colorButtonNormal7->setColor(color[7]); trcNormal7->setChecked(transparent[7]); bcNormal7->setChecked(bold[7]);
		inactive = type[7] == 1 || type[7] == 2; colorButtonNormal7->setDisabled(inactive); trcNormal7->setDisabled(inactive); bcNormal7->setDisabled(inactive);
		tcInt7->setCurrentItem(type[17]); kColorButtonInt7->setColor(color[17]); trcInt7->setChecked(transparent[17]); bcInt7->setChecked(bold[17]);
		inactive = type[17] == 1 || type[17] == 2; kColorButtonInt7->setDisabled(inactive); trcInt7->setDisabled(inactive); bcInt7->setDisabled(inactive);
	
		tcNorm8->setCurrentItem(type[8]); colorButtonNormal8->setColor(color[8]); trcNormal8->setChecked(transparent[8]); bcNormal8->setChecked(bold[8]);
		inactive = type[8] == 1 || type[8] == 2; colorButtonNormal8->setDisabled(inactive); trcNormal8->setDisabled(inactive); bcNormal8->setDisabled(inactive);
		tcInt8->setCurrentItem(type[18]); kColorButtonInt8->setColor(color[18]); trcInt8->setChecked(transparent[18]); bcInt8->setChecked(bold[18]);
		inactive = type[18] == 1 || type[18] == 2; kColorButtonInt8->setDisabled(inactive); trcInt8->setDisabled(inactive); bcInt8->setDisabled(inactive);
	
		tcNorm9->setCurrentItem(type[9]); colorButtonNormal9->setColor(color[9]); trcNormal9->setChecked(transparent[9]); bcNormal9->setChecked(bold[9]);
		inactive = type[9] == 1 || type[9] == 2; colorButtonNormal9->setDisabled(inactive); trcNormal9->setDisabled(inactive); bcNormal9->setDisabled(inactive);
		tcInt9->setCurrentItem(type[19]); kColorButtonInt9->setColor(color[19]); trcInt9->setChecked(transparent[19]); bcInt9->setChecked(bold[19]);
		inactive = type[19] == 1 || type[19] == 2; kColorButtonInt9->setDisabled(inactive); trcInt9->setDisabled(inactive); bcInt9->setDisabled(inactive);
	
		saveButton->setEnabled(titleLine->text().length() != 0);
		schMod=true;
		emit changed();
    }
}

QString SchemaEditor::readSchemaTitle(const QString & file)
{
    /*
       Code taken from konsole/konsole/schema.cpp

     */


    QString fPath = locate("data", "konsole/" + file);

    if (fPath.isNull())
	fPath = locate("data", file);

    if (fPath.isNull())
	return 0;

    FILE *sysin = fopen(QFile::encodeName(fPath), "r");
    if (!sysin)
	return 0;


    char line[100];
    while (fscanf(sysin, "%80[^\n]\n", line) > 0)
	if (strlen(line) > 5)
	    if (!strncmp(line, "title", 5)) {
		fclose(sysin);
		return i18n(line + 6);
	    }

    return 0;
}

void SchemaEditor::schemaListChanged()
{
    QStringList titles, filenames;
    SchemaListBoxText *item;

    for (int index = 0; index < (int) schemaList->count(); index++) {
      item = (SchemaListBoxText *) schemaList->item(index);
      titles.append(item->text());
      filenames.append(item->filename().section('/', -1));
    }

    emit schemaListChanged(titles, filenames);
}

void SchemaEditor::querySave()
{
    int result = KMessageBox::questionYesNo(this,
			i18n("The schema has been modified.\n"
			"Do you want to save the changes?"),
			i18n("Schema Modified"),
			KStdGuiItem::save(),
			KStdGuiItem::discard());
    if (result == KMessageBox::Yes)
    {
        saveCurrent();
    }
}

void SchemaEditor::readSchema(int num)
{
    /*
       Code taken from konsole/konsole/schema.cpp

     */

    if (oldSchema != -1) {


	if (defaultSchemaCB->isChecked()) {

	    defaultSchema = ((SchemaListBoxText *) schemaList->item(oldSchema))->filename();

	}

	if(schMod) {
	    disconnect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
	    schemaList->setCurrentItem(oldSchema);
	    querySave();
	    schemaList->setCurrentItem(num);
	    connect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
	    schMod=false;
	}

    }

    QString fPath = locate("data", "konsole/" +
			   ((SchemaListBoxText *) schemaList->item(num))->filename());

    if (fPath.isNull())
	fPath = locate("data",
		       ((SchemaListBoxText *) schemaList->item(num))->filename());

    if (fPath.isNull()) {
	KMessageBox::error(this, i18n("Cannot find the schema."),
			   i18n("Error Loading Schema"));


	return;
    }
    removeButton->setEnabled( QFileInfo (fPath).isWritable () );
    defaultSchemaCB->setChecked(fPath.section('/',-1) == defaultSchema.section('/',-1));

    FILE *sysin = fopen(QFile::encodeName(fPath), "r");
    if (!sysin) {
	KMessageBox::error(this, i18n("Cannot load the schema."),
			   i18n("Error Loading Schema"));
	loadAllSchema();
	return;
    }

    char line[100];


    titleLine->setText(i18n("untitled"));
    transparencyCheck->setChecked(false);
    backgndLine->setText("");

    while (fscanf(sysin, "%80[^\n]\n", line) > 0) {
		if (strlen(line) > 5) {
	
			if (!strncmp(line, "title", 5)) {
				titleLine->setText(i18n(line + 6));
			}
	
	
	
			if (!strncmp(line, "image", 5)) {
				char rend[100], path[100];
				int attr = 1;
				if (sscanf(line, "image %s %s", rend, path) != 2)
					continue;
				if (!strcmp(rend, "tile"))
					attr = 2;
				else if (!strcmp(rend, "center"))
					attr = 3;
				else if (!strcmp(rend, "full"))
					attr = 4;
				else
					continue;
		
				QString qline(line);
				backgndLine->setText(locate("wallpaper", qline.mid( qline.find(" ",7)+1 ) ));
				modeCombo->setCurrentItem(attr - 2);
			}
	
	
			if (!strncmp(line, "transparency", 12)) {
				float rx;
				int rr, rg, rb;
				// Transparency needs 4 parameters: fade strength and the 3
				// components of the fade color.
				if (sscanf(line, "transparency %g %d %d %d", &rx, &rr, &rg, &rb) != 4)
					continue;
		
				transparencyCheck->setChecked(true);
				shadeSlide->setValue((int)(100 - rx * 100));
				shadeColor->setColor(QColor(rr, rg, rb));
			}

			if (!strncmp(line,"rcolor",6)) {
				int fi,ch,cs,cv,tr,bo;
				if(sscanf(line,"rcolor %d %d %d %d %d",&fi,&cs,&cv,&tr,&bo) != 5)
					continue;
				if (!(0 <= fi && fi <= TABLE_COLORS))
					continue;
				ch = 0; // Random hue - set to zero
				if (!(0 <= cs && cs <= 255         ))
					continue;
				if (!(0 <= cv && cv <= 255         ))
					continue;
				if (!(0 <= tr && tr <= 1           ))
					continue;
				if (!(0 <= bo && bo <= 1           ))
					continue;
				color[fi] = QColor();
				color[fi].setHsv(ch,cs,cv);
				transparent[fi] = tr;
				bold[fi] = bo;
				type[fi] = 3;
			}

			if (!strncmp(line, "color", 5)) {
				int fi, cr, cg, cb, tr, bo;
				if (sscanf(line, "color %d %d %d %d %d %d", &fi, &cr, &cg, &cb, &tr, &bo) != 6)
					continue;
				if (!(0 <= fi && fi <= TABLE_COLORS))
					continue;
				if (!(0 <= cr && cr <= 255))
					continue;
				if (!(0 <= cg && cg <= 255))
					continue;
				if (!(0 <= cb && cb <= 255))
					continue;
				if (!(0 <= tr && tr <= 1))
					continue;
				if (!(0 <= bo && bo <= 1))
					continue;
				color[fi] = QColor(cr, cg, cb);
				transparent[fi] = tr;
				bold[fi] = bo;
				type[fi] = 0;
			}

			if (!strncmp(line, "sysfg", 5)) {
				int fi, tr, bo;
				if (sscanf(line, "sysfg %d %d %d", &fi, &tr, &bo) != 3)
					continue;
				if (!(0 <= fi && fi <= TABLE_COLORS))
					continue;
				if (!(0 <= tr && tr <= 1))
					continue;
				if (!(0 <= bo && bo <= 1))
					continue;
				color[fi] = kapp->palette().active().text();
				transparent[fi] = tr;
				bold[fi] = bo;
				type[fi] = 1;
			}

			if (!strncmp(line, "sysbg", 5)) {
				int fi, tr, bo;
				if (sscanf(line, "sysbg %d %d %d", &fi, &tr, &bo) != 3)
					continue;
				if (!(0 <= fi && fi <= TABLE_COLORS))
					continue;
				if (!(0 <= tr && tr <= 1))
					continue;
				if (!(0 <= bo && bo <= 1))
					continue;
				color[fi] = kapp->palette().active().base();
				transparent[fi] = tr;
				bold[fi] = bo;
				type[fi] = 2;
			}
		}
    }

	fclose(sysin);

	// Set colors and parameters for this scheme
	bool inactive = true;
	tcNorm0->setCurrentItem(type[0]); colorButtonNormal0->setColor(color[0]); trcNormal0->setChecked(transparent[0]); bcNormal0->setChecked(bold[0]);
	inactive = type[0] == 1 || type[0] == 2; colorButtonNormal0->setDisabled(inactive); trcNormal0->setDisabled(inactive); bcNormal0->setDisabled(inactive);
	tcInt0->setCurrentItem(type[10]); kColorButtonInt0->setColor(color[10]); trcInt0->setChecked(transparent[10]); bcInt0->setChecked(bold[10]);
	inactive = type[10] == 1 || type[10] == 2; kColorButtonInt0->setDisabled(inactive); trcInt0->setDisabled(inactive); bcInt0->setDisabled(inactive);

	tcNorm1->setCurrentItem(type[1]); colorButtonNormal1->setColor(color[1]); trcNormal1->setChecked(transparent[1]); bcNormal1->setChecked(bold[1]);
	inactive = type[1] == 1 || type[1] == 2; colorButtonNormal1->setDisabled(inactive); trcNormal1->setDisabled(inactive); bcNormal1->setDisabled(inactive);
	tcInt1->setCurrentItem(type[11]); kColorButtonInt1->setColor(color[11]); trcInt1->setChecked(transparent[11]); bcInt1->setChecked(bold[11]);
	inactive = type[11] == 1 || type[11] == 2; kColorButtonInt1->setDisabled(inactive); trcInt1->setDisabled(inactive); bcInt1->setDisabled(inactive);

	tcNorm2->setCurrentItem(type[2]); colorButtonNormal2->setColor(color[2]); trcNormal2->setChecked(transparent[2]); bcNormal2->setChecked(bold[2]);
	inactive = type[2] == 1 || type[2] == 2; colorButtonNormal2->setDisabled(inactive); trcNormal2->setDisabled(inactive); bcNormal2->setDisabled(inactive);
	tcInt2->setCurrentItem(type[12]); kColorButtonInt2->setColor(color[12]); trcInt2->setChecked(transparent[12]); bcInt2->setChecked(bold[12]);
	inactive = type[12] == 1 || type[12] == 2; kColorButtonInt2->setDisabled(inactive); trcInt2->setDisabled(inactive); bcInt2->setDisabled(inactive);

	tcNorm3->setCurrentItem(type[3]); colorButtonNormal3->setColor(color[3]); trcNormal3->setChecked(transparent[3]); bcNormal3->setChecked(bold[3]);
	inactive = type[3] == 1 || type[13] == 2; colorButtonNormal3->setDisabled(inactive); trcNormal3->setDisabled(inactive); bcNormal3->setDisabled(inactive);
	tcInt3->setCurrentItem(type[13]); kColorButtonInt3->setColor(color[13]); trcInt3->setChecked(transparent[13]); bcInt3->setChecked(bold[13]);
	inactive = type[13] == 1 || type[13] == 2; kColorButtonInt3->setDisabled(inactive); trcInt3->setDisabled(inactive); bcInt3->setDisabled(inactive);

	tcNorm4->setCurrentItem(type[4]); colorButtonNormal4->setColor(color[4]); trcNormal4->setChecked(transparent[4]); bcNormal4->setChecked(bold[4]);
	inactive = type[4] == 1 || type[4] == 2; colorButtonNormal4->setDisabled(inactive); trcNormal4->setDisabled(inactive); bcNormal4->setDisabled(inactive);
	tcInt4->setCurrentItem(type[14]); kColorButtonInt4->setColor(color[14]); trcInt4->setChecked(transparent[14]); bcInt4->setChecked(bold[14]);
	inactive = type[14] == 1 || type[14] == 2; kColorButtonInt4->setDisabled(inactive); trcInt4->setDisabled(inactive); bcInt4->setDisabled(inactive);

	tcNorm5->setCurrentItem(type[5]); colorButtonNormal5->setColor(color[5]); trcNormal5->setChecked(transparent[5]); bcNormal5->setChecked(bold[5]);
	inactive = type[5] == 1 || type[5] == 2; colorButtonNormal5->setDisabled(inactive); trcNormal5->setDisabled(inactive); bcNormal5->setDisabled(inactive);
	tcInt5->setCurrentItem(type[15]); kColorButtonInt5->setColor(color[15]); trcInt5->setChecked(transparent[15]); bcInt5->setChecked(bold[15]);
	inactive = type[15] == 1 || type[15] == 2; kColorButtonInt5->setDisabled(inactive); trcInt5->setDisabled(inactive); bcInt5->setDisabled(inactive);

	tcNorm6->setCurrentItem(type[6]); colorButtonNormal6->setColor(color[6]); trcNormal6->setChecked(transparent[6]); bcNormal6->setChecked(bold[6]);
	inactive = type[6] == 1 || type[6] == 2; colorButtonNormal6->setDisabled(inactive); trcNormal6->setDisabled(inactive); bcNormal6->setDisabled(inactive);
	tcInt6->setCurrentItem(type[16]); kColorButtonInt6->setColor(color[16]); trcInt6->setChecked(transparent[16]); bcInt6->setChecked(bold[16]);
	inactive = type[16] == 1 || type[16] == 2; kColorButtonInt6->setDisabled(inactive); trcInt6->setDisabled(inactive); bcInt6->setDisabled(inactive);

	tcNorm7->setCurrentItem(type[7]); colorButtonNormal7->setColor(color[7]); trcNormal7->setChecked(transparent[7]); bcNormal7->setChecked(bold[7]);
	inactive = type[7] == 1 || type[7] == 2; colorButtonNormal7->setDisabled(inactive); trcNormal7->setDisabled(inactive); bcNormal7->setDisabled(inactive);
	tcInt7->setCurrentItem(type[17]); kColorButtonInt7->setColor(color[17]); trcInt7->setChecked(transparent[17]); bcInt7->setChecked(bold[17]);
	inactive = type[17] == 1 || type[17] == 2; kColorButtonInt7->setDisabled(inactive); trcInt7->setDisabled(inactive); bcInt7->setDisabled(inactive);

	tcNorm8->setCurrentItem(type[8]); colorButtonNormal8->setColor(color[8]); trcNormal8->setChecked(transparent[8]); bcNormal8->setChecked(bold[8]);
	inactive = type[8] == 1 || type[8] == 2; colorButtonNormal8->setDisabled(inactive); trcNormal8->setDisabled(inactive); bcNormal8->setDisabled(inactive);
	tcInt8->setCurrentItem(type[18]); kColorButtonInt8->setColor(color[18]); trcInt8->setChecked(transparent[18]); bcInt8->setChecked(bold[18]);
	inactive = type[18] == 1 || type[18] == 2; kColorButtonInt8->setDisabled(inactive); trcInt8->setDisabled(inactive); bcInt8->setDisabled(inactive);

	tcNorm9->setCurrentItem(type[9]); colorButtonNormal9->setColor(color[9]); trcNormal9->setChecked(transparent[9]); bcNormal9->setChecked(bold[9]);
	inactive = type[9] == 1 || type[9] == 2; colorButtonNormal9->setDisabled(inactive); trcNormal9->setDisabled(inactive); bcNormal9->setDisabled(inactive);
	tcInt9->setCurrentItem(type[19]); kColorButtonInt9->setColor(color[19]); trcInt9->setChecked(transparent[19]); bcInt9->setChecked(bold[19]);
	inactive = type[19] == 1 || type[19] == 2; kColorButtonInt9->setDisabled(inactive); trcInt9->setDisabled(inactive); bcInt9->setDisabled(inactive);

    oldSchema = num;
    updatePreview();
    schMod=false;
    return;
}

