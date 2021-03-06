/*
    This file is part of libkabc.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KABC_DISTRIBUTIONLIST_H
#define KABC_DISTRIBUTIONLIST_H

#include <kdirwatch.h>

#include "addressbook.h"

namespace KABC {

class DistributionListManager;

/**
  @short Distribution list of email addresses
 
  This class represents a list of email addresses. Each email address is
  associated with an address book entry. If the address book entry changes, the
  entry in the distribution list is automatically updated.
*/
class KABC_EXPORT DistributionList
{
  public:
    /**
      @short Distribution List Entry

      This class represents an entry of a distribution list. It consists of an
      addressee and an email address. If the email address is null, the
      preferred email address of the addressee is used.
    */
    struct Entry
    {
      typedef QValueList<Entry> List;

      Entry() {}
      Entry( const Addressee &_addressee, const QString &_email ) :
          addressee( _addressee ), email( _email ) {}

      Addressee addressee;
      QString email;
    };

    /**
       Create distribution list object.

      @param manager Managing object of this list.
      @param name    Name of this list.
    */
    DistributionList( DistributionListManager *manager, const QString &name );

    /**
      Destructor.
    */
    ~DistributionList();

    /**
      Set name of this list. The name is used as key by the
      DistributinListManager.
    */
    void setName( const QString & );

    /**
      Get name of this list.
    */
    QString name() const;

    /**
      Insert an entry into this distribution list. If the entry already exists
      nothing happens.
    */
    void insertEntry( const Addressee &, const QString &email=QString::null );

    /**
      Remove an entry from this distribution list. If the entry doesn't exist
      nothing happens.
    */
    void removeEntry( const Addressee &, const QString &email=QString::null );

    /**
      Return list of email addresses, which belong to this distributon list.
      These addresses can be directly used by e.g. a mail client.
    */
    QStringList emails() const;

    /**
      Return list of entries belonging to this distribution list. This function
      is mainly useful for a distribution list editor.
    */
    Entry::List entries() const;

  private:
    DistributionListManager *mManager;
    QString mName;

    Entry::List mEntries;
};

/**
  @short Manager of distribution lists
 
  This class represents a collection of distribution lists, which are associated
  with a given address book.
*/
class KABC_EXPORT DistributionListManager
{
  public:
    /**
      Create manager for given address book.
    */
    DistributionListManager( AddressBook * );

    /**
      Destructor.
    */
    ~DistributionListManager();

    /**
      Return distribution list with given name.
    */
    DistributionList *list( const QString &name ); // KDE4: add bool caseSensitive = true

    /**
      Insert distribution list. If a list with this name already exists, nothing
      happens. The passed object is deleted by the manager.
    */
    void insert( DistributionList * );

    /**
      Remove distribution list. If a list with this name doesn't exist, nothing
      happens.
    */
    void remove( DistributionList * );

    /**
      Return names of all distribution lists managed by this manager.
    */
    QStringList listNames();

    /**
      Load distribution lists form disk.
    */
    bool load();

    /**
      Save distribution lists to disk.
    */
    bool save();

  private:
    class DistributionListManagerPrivate;
    DistributionListManagerPrivate *d;

    QPtrList<DistributionList> mLists;
};

/**
  @short Watchdog for distribution lists

  This class provides a changed() signal that i emitted when the
  distribution lists has changed in some way.

  Exapmle:

  \code
  KABC::DistributionListWatcher *watchdog = KABC::DistributionListWatcher::self()

  connect( watchdog, SIGNAL( changed() ), SLOT( doSomething() ) );
  \endcode
*/

class KABC_EXPORT DistributionListWatcher : public QObject
{
  Q_OBJECT

  public:
    /**
     * Returns the watcher object.
     */
    static DistributionListWatcher *self();

  signals:
    /**
     * This signal is emmitted whenever the distribution lists has
     * changed (if a list was added or removed, when a list was
     * renamed or the entries of the list changed).
     */
    void changed();

  protected:
    DistributionListWatcher();
    ~DistributionListWatcher();

  private:
    static DistributionListWatcher* mSelf;
    KDirWatch *mDirWatch;
};

}
#endif
