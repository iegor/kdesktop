/****************************************************************************
** $Id: indextest.h 170939 2002-08-07 23:47:07Z pfeiffer $
**
** Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org> 
**
****************************************************************************/

#ifndef INDEXTEST_H
#define INDEXTEST_H

class IndexTest : public QObject
{
    Q_OBJECT
    
public:
    IndexTest();
    ~IndexTest();
    
private slots:
    void slotFinished( bool success );
    void slotProgress( int percent, const QString& message );
    
};


#endif // INDEXTEST_H
