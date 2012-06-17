/* vi: ts=8 sts=4 sw=4
 *
 * $Id: sshdlg.h 59416 2000-07-31 19:33:42Z bieker $
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 */

#ifndef __SshDlg_h_Included__
#define __SshDlg_h_Included__

#include <kpassdlg.h>

class KDEsshDialog
    : public KPasswordDialog
{
    Q_OBJECT

public:
    KDEsshDialog(QCString host, QCString user, QCString stub, 
	    QString prompt, bool enableKeep);
    ~KDEsshDialog();

protected:
    bool checkPassword(const char *password);
    
private:
    QCString m_User, m_Host, m_Stub;
};
    

#endif // __SshDlg_h_Included__
