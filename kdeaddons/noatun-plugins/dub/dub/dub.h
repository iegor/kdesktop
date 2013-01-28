// -*-c++-*-
/***************************************************************************
                          dub.h  -  description
                             -------------------
    begin                : Tue Oct 23 01:44:51 EEST 2001
    copyright            : (C) 2001 by Eray Ozkural (exa)
    email                : erayo@cs.bilkent.edu.tr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef Dub_Interface
#define Dub_Interface

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// KDE includes
#include <kapplication.h>
#include <kmainwindow.h>
#include <kurl.h>
#include <noatun/plugin.h>
#include <noatun/playlist.h>

// sys includes
#include <vector>
#include <list>

// forward decl
class KFileItem;
class KDirLister;
class DubPlaylist;

// application specific includes
#include "dubapp.h"
#include "dubconfigmodule.h"

// dub specific application code
class Dub : public DubApp
{
  Q_OBJECT

public:

  Dub(DubPlaylist* plist);

public slots:
  /** A file selected */
  void fileSelected(const KFileItem*);
  void mediaHomeSelected(const QString& url);

  signals:
void setMediaHome(KURL);
  // xemacs rules

public:

  /** playlist object to interface noatun
   */
  DubPlaylist& playlist;
  /** noatun configuration object
   */
  DubConfigModule& dubconfig;

  KFileItem* getActiveFile() { return activeFile; }
  /** find root */
  KFileItem* queryRoot();
  /** First file in the sequence */
  const KFileItem* queryFirstFile();
  /** Select next file in order */
  void selectNextFile();
  /** Select previous file in order */
  void selectPreviousFile();

public:

  /*  The chosen file */
  KFileItem* activeFile;

private:

  void configure_sequencing();

  // expansion slot :P
  struct Sequencer {
    Sequencer(Dub* d) : dub(*d) {}
    virtual KFileItem* first() = 0;
    virtual void next() = 0;
    virtual void prev() = 0;
    Dub& dub;
    void set_file(KFileItem** file, KFileItem* val);
  };
  Sequencer* sequencer;

  // possible sequencers are linear/oneDir, linear/recursive,
  // shuffle/oneDir, shuffle/recursive, repeat, single

  // linear sequencing subsystem
  struct Linear_Seq : public Sequencer {
    Linear_Seq(Dub* d) : Sequencer(d) {}
    KFileItem* first(QPtrList<KFileItem> & items);
    KFileItem* last(QPtrList<KFileItem> & items);
    KFileItem* next(QPtrList<KFileItem> & items, KFileItem** active_file);
    KFileItem* prev(QPtrList<KFileItem> & items, KFileItem** active_file);
    bool find(QPtrList<KFileItem> & items, KFileItem* a_file);
  };

  // sequencer that traverses current directory in view order
  struct Linear_OneDir : public Linear_Seq {
    Linear_OneDir(Dub* d) : Linear_Seq(d), first_file(0) {}
    virtual ~Linear_OneDir() {}
    KFileItem* first();
    void next();
    void prev();
    KFileItem* first_file;
  };
  Linear_OneDir linear_onedir;

  // directory node for recursive play
  struct Dir_Node
  {
    Dir_Node(QString dir, bool forward = true);
    QString dir;
    QStringList subdirs;
    QStringList::iterator current_subdir;
    QPtrList<KFileItem> file_items;
    KFileItem* current_file;
    void init_traversal(bool forward);
    bool past_begin;		// stupid iterators
  };

  // recursive play sequencing subsystem
  struct Recursive_Seq {
    Recursive_Seq();
    void init(const KURL & root);
    QString canonical_path(QString dir);
    bool check_dir(QString dir);
    Dir_Node* top_dir() { return play_stack.getLast(); }
    Dir_Node* bottom_dir() { return play_stack.getFirst(); }
    bool push_dir(QString dir, bool forward = true);
    bool pop_dir();
    bool advance(bool forward = true);
    void pop_preorder(bool forward = true);
    void next_preorder();
    void prev_preorder();
    void print_stack();

    QString recursion_root;
    QPtrList<Dir_Node> play_stack;
  };

  // sequencer to make a preorder walk of the directory tree
  struct Linear_Recursive
    : public Sequencer, public Recursive_Seq {
    Linear_Recursive(Dub* d);
    virtual ~Linear_Recursive() {};
    KFileItem* first();
    void next();
    void prev();
  };
  Linear_Recursive linear_recursive;

  // shuffle/onedir sequencer
  struct Shuffle_OneDir
    : public Sequencer {
    Shuffle_OneDir(Dub* d) : Sequencer(d) {
      items.setAutoDelete(true);
    }
    virtual ~Shuffle_OneDir() {};
    void init(const QString& dir);
    KFileItem* first();
    void next();
    void prev();
    void init();

    int play_index;
    std::vector<int> play_order;
    KURL shuffle_dir;
    QPtrList<KFileItem> items;
  };
  Shuffle_OneDir shuffle_onedir;

  // shuffle/recursive sequencer
  struct Shuffle_Recursive
    : public Sequencer, public Recursive_Seq {
    Shuffle_Recursive(Dub* d) : Sequencer(d) {}
    virtual ~Shuffle_Recursive() {}
    KFileItem* random_file();
    KFileItem* first();
    void next();
    void prev();

    QString shuffle_root;
  };
  Shuffle_Recursive shuffle_recursive;

};

#endif // DUB_H
