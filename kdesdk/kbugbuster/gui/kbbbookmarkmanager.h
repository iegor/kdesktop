#ifndef KBBBOOKMARKMANAGER_H
#define KBBBOOKMARKMANAGER_H

#include <kbookmarkmanager.h>
#include <kstandarddirs.h>

class KBBBookmarkManager
{
public:
    static KBookmarkManager * self() {
        if ( !s_bookmarkManager )
        {
            QString bookmarksFile = locateLocal("data", QString::fromLatin1("kbugbuster/bookmarks.xml"));
            s_bookmarkManager = KBookmarkManager::managerForFile( bookmarksFile );
        }
        return s_bookmarkManager;
    }


    static KBookmarkManager *s_bookmarkManager;
};

#endif
