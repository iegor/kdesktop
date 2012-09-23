#include <klocale.h>
#include <qlayout.h>
#include <klineedit.h>
#include <qlabel.h>
#include <qptrlist.h>

#include <kmimetype.h>
#include <kicontheme.h>
#include <qtooltip.h>
#include <kstringvalidator.h>
#include <qspinbox.h>
#include <qhbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <kdialogbase.h>
#include <kdebug.h>
#include <kseparator.h>
#include <kfilemetainfo.h>
#include <qvalidator.h>

#include "metatag.h"
#include "edit.h"

Editor::Editor()
:  KDialogBase((QWidget*)0L, 0, false, i18n("Tag Editor"), Ok | Cancel)
{
    mMainWidget = makeMainWidget();
//    mMainWidget->setMinimumWidth(325);

    mGrid = new QGridLayout(mMainWidget, 1, 1, 0, spacingHint(), "Editor::mGrid");

    mGrid->setColStretch(1, 1);
    mGrid->setColStretch(2, 1);

    QHBox *heading = new QHBox(mMainWidget, "Editor::heading");
    heading->setSpacing(4);
    mFileIcon = new QLabel(heading);
    mFileIcon->setAlignment(AlignVCenter | AlignLeft);
    mFile = new QLabel(heading);
    mFile->setAlignment(AlignVCenter | AlignLeft);
    heading->setStretchFactor(mFile, 2);
    mGrid->addMultiCellWidget(heading, 0, 0, 0, 2);

    KSeparator *sep = new KSeparator(KSeparator::HLine, mMainWidget);
    mGrid->addMultiCellWidget(sep, 1, 1, 0, 2);

    mControls.setAutoDelete(true);
    mNextRow = 2;

    connect(this, SIGNAL(closeClicked()), SLOT(delayedDestruct()));
    connect(this, SIGNAL(okClicked()), SLOT(save()));

    enableButtonSeparator(true);
    setFixedHeight(sizeHint().height());
}

void Editor::open(const PlaylistItem & file)
{
    KFileMetaInfo file_meta_info(file.file(), file.mimetype());
    KFileMetaInfoItem info_item;

    item = file;
    mDirty = false;

    mFile->setText("<nobr><b>" + file.url().fileName(false) + "</b></nobr>");
    QToolTip::add(mFile, file.url().prettyURL());
    mFileIcon->
        setPixmap(KMimeType::pixmapForURL(file.url(), 0, KIcon::Small));

    if (file.url().isLocalFile()) {
        QFileInfo file_info(file.file());
        mFileWritable = file_info.isWritable();
    }
    else {
        // KFileMetaInfo doesn't work on remote files
        mFileWritable = false;
    }

    if ( !file_meta_info.isValid() ) // go ahead people, nothing to see here
        return;

    mControls.append(createControl(file_meta_info, i18n("&Title"), "Title", QVariant::String, false, mMainWidget));
    mControls.append(createControl(file_meta_info, i18n("&Artist"), "Artist", QVariant::String, false, mMainWidget));
    mControls.append(createControl(file_meta_info, i18n("A&lbum"), "Album", QVariant::String, false, mMainWidget));
    mControls.append(createControl(file_meta_info, i18n("&Date"), "Date", QVariant::String, false, mMainWidget));
    mControls.append(createControl(file_meta_info, i18n("T&rack"), "Tracknumber", QVariant::UInt, false, mMainWidget));
    mControls.append(createControl(file_meta_info, i18n("&Genre"), "Genre", QVariant::String, false, mMainWidget));
    mControls.append(createControl(file_meta_info, i18n("Co&mment"), "Comment", QVariant::String, false, mMainWidget));
}

void Editor::save()
{
    // Only write to disk if something actually changed
    if (!mDirty)
    {
        delayedDestruct();
        return;
    }

    KFileMetaInfo meta_info(item.file(), item.mimetype());
    if ( !meta_info.isValid() )
    {
        delayedDestruct();
        return;
    }

    for (MetaWidget *meta_widget = mControls.first(); meta_widget; meta_widget = mControls.next() )
        saveControl(meta_info, *meta_widget);

    meta_info.applyChanges();

    emit(saved(item));
    delayedDestruct();
}

void Editor::saveControl(KFileMetaInfo& meta_info, const MetaWidget &meta_widget) {
    QVariant value;
    const KFileMimeTypeInfo *info = KFileMetaInfoProvider::self()->mimeTypeInfo( meta_info.mimeType() );

     if (!meta_widget.widget->isEnabled())
         return;

     if (meta_widget.widget->inherits("QSpinBox"))
         value = static_cast<QSpinBox *>(meta_widget.widget)->value();
     else if (meta_widget.widget->inherits("QComboBox"))
         value = static_cast<QComboBox *>(meta_widget.widget)->currentText();
     else if (meta_widget.widget->inherits("QLineEdit"))
         value = static_cast<QLineEdit *>(meta_widget.widget)->text();

     QString group = keyGroup(meta_info, meta_widget.key);

     if (group.isNull()) {
         kdWarning() << "Cannot find group for " << meta_widget.key << endl;
         return;
     }

     if (info->groupInfo(group)->itemInfo(meta_widget.key)) {
        if (info->groupInfo(group)->attributes() & KFileMimeTypeInfo::Addable) {
            kdDebug(66666) << "Adding group " << group << endl;
            if (!meta_info.addGroup(group))
                kdWarning() << "Adding group \"" << group << "\" failed!" << endl;
        }

        if (info->groupInfo(group)->itemInfo(meta_widget.key)->attributes() & KFileMimeTypeInfo::Addable) {
            kdDebug(66666) << "Adding key " << meta_widget.key << endl;
            if (!meta_info.group(group).addItem(meta_widget.key).isValid())
                kdWarning() << "Adding key \"" << meta_widget.key << "\" failed!" << endl;
        }
     }

    if (value.cast(meta_info.item(meta_widget.key).type())) {
        if (!meta_info.item(meta_widget.key).setValue(value))
            kdWarning() << "setValue() failed on " << group << "/" << meta_widget.key << endl;
    }
    else
         kdWarning() << "Cannot save " << meta_widget.key << " as required type." << endl;
}

MetaWidget* Editor::createControl(KFileMetaInfo& meta_info, const QString &label, const QString &key, QVariant::Type default_type, bool optional, QWidget *parent) {
    QLabel *tmp_label = 0L;
    KFileMetaInfoItem info_item = meta_info.item(key);
    QVariant::Type type;
    MetaWidget *meta_widget = 0L;
    QValidator *validator = 0L;
    QString groupName = keyGroup(meta_info, key);
    bool known_key = ((!groupName.isNull()) && meta_info.group(groupName).contains(key));
    bool addable = keyAddable(meta_info, key);
    const KFileMimeTypeInfo *info = KFileMetaInfoProvider::self()->mimeTypeInfo( meta_info.mimeType() );

    // This key isn't a real thing, and we can't even create it
    if ((!info_item.isEditable()) && (!addable) && optional)
        return 0;

    if (!groupName.isNull()) {
         type = info->groupInfo(groupName)->itemInfo(key)->type();
    }
    else {
         type = default_type;
    }

    // Get the correct validator
    if ( info && !groupName.isNull() )
        validator = info->createValidator( groupName, key, parent );

    // meta_widget is used for book-keeping internally
    meta_widget = new MetaWidget;
    meta_widget->key = key;

    if ((type == QVariant::Int) || (type == QVariant::UInt)) {
        // We're an int, make a spin box
        QSpinBox *box = new QSpinBox(parent);

        // Well, normally metatag doesn't care that much about suffixes
        // and prefixes, but this is just too easy.
        box->setPrefix(info_item.prefix());
        box->setSuffix(info_item.suffix());
        // Kinda a hack... display " " instead of "0" (think track numbers)
        box->setSpecialValueText(" ");

        // Do we have a validator?
        if (validator) {
            box->setValidator(validator);

            // Is it an integer validator
            if (validator->inherits("QIntValidator")) {
                QIntValidator *int_validator = static_cast<QIntValidator *>(validator);

                // FIXME: Why the -hell- doesn't QSpinBox::setValidator() do this??
                box->setMinValue(int_validator->bottom());
                box->setMaxValue(int_validator->top());
            }
        }

        box->setValue(info_item.value().toInt());

        connect(box, SIGNAL(valueChanged(int)), this, SLOT(modified()));
        meta_widget->widget = box;
    }
    else {
        // We're not an int, make a KLineEdit/QComboBox, depending on our validator
        bool combo_box = false;

        if (validator)
           combo_box = validator->isA("KStringListValidator");

        if (combo_box) {
             QComboBox *combo = new QComboBox(parent);

             combo->clear();
             combo->insertStringList(static_cast<
                                     KStringListValidator *
                                     >(validator)->stringList());

             combo->setCurrentText(info_item.value().toString());
             connect(combo, SIGNAL(activated(int)), this, SLOT(modified()));

             meta_widget->widget = combo;
        }
        else {
             KLineEdit *edit;

             edit = new KLineEdit(parent);
             edit->setText(info_item.value().toString());
             edit->setValidator(validator);
             connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(modified()));

             meta_widget->widget = edit;
        }
    }

    if (known_key)
        meta_widget->widget->setEnabled(info_item.isEditable() && mFileWritable);
    else
        meta_widget->widget->setEnabled(addable && mFileWritable);

    mGrid->addMultiCellWidget(meta_widget->widget, mNextRow, mNextRow, 1, 2);

    // Add our label. This is the easy part
    tmp_label = new QLabel(meta_widget->widget, label + ":", parent);
    mGrid->addWidget(tmp_label, mNextRow, 0);


    mNextRow++;

    return meta_widget;
}

QString Editor::keyGroup(const KFileMetaInfo& i, QString key) {
    const KFileMimeTypeInfo *info = KFileMetaInfoProvider::self()->mimeTypeInfo( i.mimeType() );
    QStringList groups = info->supportedGroups();

    for (QStringList::Iterator it = groups.begin();it != groups.end();++it) {
        if (info->groupInfo(*it)->itemInfo(key)) {
            return *it;
        }
    }

    return QString::null;
}

bool Editor::keyAddable(const KFileMetaInfo &i, QString key) {
    const KFileMimeTypeInfo *info = KFileMetaInfoProvider::self()->mimeTypeInfo( i.mimeType() );
    QStringList groups = info->supportedGroups();

    kdDebug(66666) << "Testing if " << key << " is addable" << endl;
    for (QStringList::Iterator it = groups.begin();it != groups.end();++it) {
        if (info->groupInfo(*it)->supportsVariableKeys()) {
            kdDebug(66666) << "Group " << *it << " supports variable keys" << endl;
            return true;
        }

        if (info->groupInfo(*it)->itemInfo(key)) {
            if (info->groupInfo(*it)->attributes() & KFileMimeTypeInfo::Addable) {
                 kdDebug(66666) << "Group " << *it << " is addable" << endl;
                 return true;
            }

            if (info->groupInfo(*it)->itemInfo(key)->attributes() & KFileMimeTypeInfo::Addable)
                return true;
        }
    }

    return false;
}

void Editor::modified() {
	mDirty = true;
}

#include "edit.moc"
