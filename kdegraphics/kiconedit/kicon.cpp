/*
    KDE Icon Editor - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/  

#include <stdlib.h>

#include <qimage.h>

#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <kimageio.h>
#include <kdebug.h>
#include <klocale.h>

#include "kicon.h"
#include "utils.h"

KIconEditIcon::KIconEditIcon(QObject *parent, const QImage *img, KURL url) 
   : QObject(parent)
{
    f = 0;
    _lastdir = "/";
    //checkUnNamedBackup(img);

    if(!url.isEmpty())
    {
        open(img, url);
    }
}


KIconEditIcon::~KIconEditIcon()
{
}


bool KIconEditIcon::open(const QImage *image, KURL url)
{
    QImage *img = (QImage*)image;

    if(url.isEmpty())
        return false;

    kdDebug(4640) << "KIconEditIcon::open " << url.prettyURL() << endl;
    
    if(!url.isValid()) // try to see if it is a relative filename
    {
        kdDebug(4640) << "KIconEditIcon::open (malformed) " << url.prettyURL() << endl;

        QFileInfo fi(url.url());
        if(fi.isRelative())
            url = "file:" + fi.absFilePath();

        if(!url.isValid()) // Giving up
        {
            QString msg = i18n("The URL: %1 \nseems to be malformed.\n").arg(url.url());
            KMessageBox::sorry((QWidget*)parent(), msg);
            return false;
        }
    }

    QString filename;

    if(url.isLocalFile())
    {
        filename = url.path();
    }
    else
    {
        if(!KIO::NetAccess::download( url, filename, (QWidget*)parent() ))
        {
            QString msg = i18n("There was an error loading:\n%1\n").arg(url.prettyURL());
            KMessageBox::error((QWidget*)parent(), msg);
            return false;
        }
    }

    bool loadedOk = img->load(filename);

    if(!url.isLocalFile())
    {
        KIO::NetAccess::removeTempFile( filename );
    }

    if(!loadedOk) 
    {
          QString msg = i18n("There was an error loading:\n%1\n").arg(url.prettyURL());
          KMessageBox::error((QWidget*)parent(), msg);
    }
    else
    {
        kdDebug(4640) << "KIconEditIcon::open - Image loaded" << endl;
        
        // _url is saved off for use in saving the image to the same 
        // file later - should include full path
        if(url.isLocalFile())
        {
            _url = url.path();
        }
        else
        {
            _url = "";
        }
        
        kdDebug(4640) << "KIcon: _url: " << _url << endl;
        
        // this causes updates of preview, color palettes, etc.    
        emit loaded(img);
        kdDebug(4640) << "loaded(img)" <<  endl;    
        
        // this is the name that shows up in status bar - 
        // should be filename with path
        emit newname(url.prettyURL()); 
        kdDebug(4640) << "newname(_url) : " << url.prettyURL() << endl;    
        
        emit addrecent(url.prettyURL());
        
        kdDebug(4640) << "KIconEditIcon::open - done" << endl;
    }

    return loadedOk;
}



bool KIconEditIcon::promptForFile(const QImage *img)
{
    kdDebug(4640) << "KIconEditIcon::promptForFile(const QImage *img)" << endl;
    /*
    QString filter = i18n("*|All Files (*)\n"
                        "*.xpm|XPM (*.xpm)\n"
                        "*.png|PNG (*.png)\n"
                        "*.gif|GIF files (*.gif)\n"
                        "*.jpg|JPEG files (*.jpg)\n"
                        "*.ico|Icon files (*.ico)\n");

  
    KURL url = KFileDialog::getOpenURL( QString::null, filter );
    */
    bool loaded = false;
    KURL url = KFileDialog::getImageOpenURL( QString::null, static_cast<QWidget *>(parent()) );

    if( !url.isEmpty() )
    {
        loaded = open( img, url );
    }

    return loaded;
}



bool KIconEditIcon::saveAs(const QImage *image)
{
    kdDebug(4640) << "KIconEditIcon::saveAs" << endl;

    QString file;

    //Get list of file types..
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, static_cast<QWidget *>(parent()), "file dialog", true);
    dialog->setCaption( i18n("Save Icon As") );
    dialog->setKeepLocation( true );
    dialog->setMimeFilter( KImageIO::mimeTypes(KImageIO::Writing), "image/png" );
    dialog->setOperationMode( KFileDialog::Saving );

    if(dialog->exec()==QDialog::Accepted)
    {
        file = dialog->selectedFile();
        if( file.isNull() )
        {
            delete dialog;
            return false;
        }
        if ( !KImageIO::canWrite(KImageIO::type(file)) )
        {
            if ( KImageIO::canWrite(KImageIO::typeForMime(dialog->currentFilter())) )
                file += "."+KImageIO::suffix(KImageIO::typeForMime(dialog->currentFilter()));
            else
                file += ".png";
        }
    }
    else
    {
        delete dialog;
        return false;
    }
        
    delete dialog;

    if(QFile::exists(file))
    {
        int r=KMessageBox::warningContinueCancel(static_cast<QWidget *>(parent()),
            i18n( "A file named \"%1\" already exists. "
                  "Overwrite it?" ).arg(file),
            i18n( "Overwrite File?" ),
            i18n( "&Overwrite" ) );
  
        if(r==KMessageBox::Cancel)
        {
            return false;
        }
    }

    return save( image, file );
}



bool KIconEditIcon::save(const QImage *image, const QString &_filename)
{
    kdDebug(4640) << "KIconEditIcon::save" << endl;
    QString filename = _filename;

    if(filename.isEmpty())
    {
        if(_url.isEmpty())
        {
            return saveAs(image);
        }
        else
        {
            KURL turl(_url);
            filename = turl.path();
        }    
    }

    QImage *img = (QImage*)image;
    img->setAlphaBuffer(true);
    
    KURL turl(filename);
    QString str = turl.path();
    bool savedOk = false;

    /* base image type on file extension - let kimageio
    take care of this determination */
    
    if(img->save(str, KImageIO::type(str).ascii()))    
    {
        kdDebug(4640) << "img->save()) successful" << endl;
        emit saved();    
        _url = filename;
        
        // emit signal to change title bar to reflect new name
        emit newname(filename); 
        kdDebug(4640) << "newname(filenamme) : " << _url << endl;    
        savedOk = true;
        emit addrecent(filename);
    }
    else 
    {
        QString msg = i18n("There was an error saving:\n%1\n").arg(str);
        KMessageBox::error((QWidget*)parent(), msg);
        kdDebug(4640) << "KIconEditIcon::save - " << msg << endl;
    }

    kdDebug(4640) << "KIconEditIcon::save - done" << endl;    

    return savedOk;
}


#include "kicon.moc"
