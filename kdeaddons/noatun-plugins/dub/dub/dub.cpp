/***************************************************************************
                          dub.cpp  -  description
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

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>

// include files for KDE
#include <kiconloader.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kurl.h>
#include <kurlrequester.h>
#include <noatun/playlist.h>

#include <vector>
#include <algorithm>
using std::vector;
using std::iterator;

// application specific includes
#include "dub.h"
#include "dub.moc"
#include "dubview.h"
#include "dubplaylist.h"
#include "dubprefs.h"
#include "dubconfigmodule.h"

#include "random.h"
int Random::seed;

#define ID_STATUS_MSG 1

Dub::Dub(DubPlaylist* plist)
  : DubApp(0)
  , playlist(*plist)
  , dubconfig(*plist->dubconfig)
  , activeFile(0)
  , linear_onedir(this)
  , linear_recursive(this)
  , shuffle_onedir(this)
  , shuffle_recursive(this)
{
  connect( view->dirOperator(),
           SIGNAL(fileSelected(const KFileItem*)),
           this,
           SLOT(fileSelected(const KFileItem*)) );
  connect( dubconfig.prefs->mediaDirectory,
           SIGNAL( urlSelected (const QString &) ),
           this,
           SLOT( mediaHomeSelected (const QString &) ) );
  connect( this,
           SIGNAL(setMediaHome(KURL)),
           view,
           SLOT(setDir(KURL)) );
  configure_sequencing();
  emit setMediaHome(dubconfig.mediaDirectory);
}

/** File selected */
void Dub::fileSelected(const KFileItem * file) {
  kdDebug(90010) << "dub: file selected " << file << endl;
  activeFile = const_cast<KFileItem *>(file);
  playlist.setCurrent(file, true);
}

void Dub::mediaHomeSelected(const QString& url) {
  kdDebug(90010) << "media home selected:" << endl;
  emit setMediaHome( KURL(url) );
}

/** changes the active file to the next item
 */
void Dub::selectNextFile() {
  configure_sequencing();
  sequencer->next();
}

/** No descriptions */
KFileItem* Dub::queryRoot() {
  return view->dirLister()->rootItem();
}

/** First file in the directory */
const KFileItem* Dub::queryFirstFile() {
  return sequencer->first();
}

/** Select previous file */
void Dub::selectPreviousFile() {
  configure_sequencing();
  sequencer->prev();
}

void Dub::configure_sequencing()
{
  switch (dubconfig.playMode) {
  case DubConfigModule::allFiles:
    if (dubconfig.playOrder==DubConfigModule::normal) {
      linear_recursive.init(dubconfig.mediaDirectory);
      sequencer = &linear_recursive;
    }
    else if (dubconfig.playOrder==DubConfigModule::shuffle) {
      shuffle_recursive.init(dubconfig.mediaDirectory);
      sequencer = &shuffle_recursive;
    }
    break;
  case DubConfigModule::recursiveDir:
    linear_recursive.init(view->currentDirectory().path());
    sequencer = &linear_recursive;
    break;
  case DubConfigModule::oneDir:
    if (dubconfig.playOrder==DubConfigModule::normal)
      sequencer = &linear_onedir;
    else if (dubconfig.playOrder==DubConfigModule::shuffle) {
      shuffle_onedir.init(view->currentDirectory().path());
      sequencer = &shuffle_onedir;
    }
    break;
  }
}

void Dub::Sequencer::set_file(KFileItem** file, KFileItem* val) {
  assert(val);
  if (*file)
    delete *file;
  *file = new KFileItem(*val);
  kdDebug(90010) << "set_file to " << val->url() << endl;
}

KFileItem* Dub::Linear_Seq::first(QPtrList<KFileItem> & items)
{
  // find first file
  KFileItem* firstFile = 0;
  for (KFileItem* item = items.first(); item; item = items.next() ) {
    if (item->isFile()) {
      firstFile = item;
      break;
    }
  }
  return firstFile;
}

KFileItem* Dub::Linear_Seq::last(QPtrList<KFileItem> & items)
{
  // find last file
  KFileItem* lastFile = 0;
  for (KFileItem* item = items.last(); item; item = items.prev() ) {
    if (item->isFile()) {
      lastFile = item;
      break;
    }
  }
  return lastFile;
}

bool Dub::Linear_Seq::find(QPtrList<KFileItem> & items, KFileItem* a_file)
{
  // find file
   for (KFileItem *file=items.first(); file; file=items.next() )
      if (file->isFile() && file->cmp(*a_file)) {
        kdDebug(90010) << " found " << (file->url()) << endl;
        return true;
      }
  return false;
}

KFileItem* Dub::Linear_Seq::next(QPtrList<KFileItem> & items,
			      KFileItem** active_file)
{
  KFileItem* ret = 0;
  assert(active_file);
  bool found = false;
  if (*active_file) {
    if (find(items, *active_file)) {
      KFileItem* next = items.next();
      for (; next && !next->isFile(); next = items.next()) ; // find next file
      if (next && next->isFile())
	set_file(active_file, next);
        found = true;
        ret = next;
    }
  }
  if (!found) { // try to get the first one then
     KFileItem *fst = first(items);
     if (fst) {
       set_file(active_file, fst);
       ret = fst;
     }
   }
  return ret;
}

KFileItem* Dub::Linear_Seq::prev(QPtrList<KFileItem> & items,
			      KFileItem** active_file)
{
  KFileItem* ret = 0;
  assert(active_file);
  bool found = false;
  if (*active_file) {
    // locate current item
    if (find(items, *active_file)) {
      KFileItem* prev = items.prev();
      for (; prev && !prev->isFile(); prev = items.prev()) ; // find prev file
      if (prev && prev->isFile()) {
        set_file(active_file, prev);
        found = true;
        ret = prev;
      }
    }
  }
  if (!found) { // try to get the last one then
     KFileItem *lst = last(items);
     if (lst) {
       set_file(active_file, lst);
       ret = lst;
     }
   }
  return ret;
}

KFileItem* Dub::Linear_OneDir::first()
{
  KFileItem* first = Linear_Seq::first(dub.view->items());
  if (first)
    set_file(&first_file, first);
  else {
    if (first_file) {		// invalidate first
      delete first_file;
      first_file = 0;
    }
  }
  return first_file;
}

//KFileItem* Dub::Linear_OneDir::getAfter(KFileItem* item)
//{

//}

void Dub::Linear_OneDir::next()
{
  KFileItem *f = Linear_Seq::next(dub.view->items(), &dub.activeFile);
  if (f) {
    dub.view->selectFile(f);
  }
}

void Dub::Linear_OneDir::prev()
{
  KFileItem *f = Linear_Seq::prev(dub.view->items(), &dub.activeFile);
  if (f) {
    dub.view->selectFile(f);
  }
}

Dub::Dir_Node::Dir_Node(QString d, bool forward)
  : dir(d), past_begin(false)
{
  kdDebug(90010) << "cons dir node " << d << endl;
  // process entry list, form a list of subdirs and normal files
  file_items.setAutoDelete(true);
  QDir dir_obj(dir);
  QFileInfoList* entries =
    const_cast<QFileInfoList*>(dir_obj.entryInfoList());
  for ( QFileInfo *file = entries->first(); file; file = entries->next() ) {
    if (file->isDir() && file->absFilePath().length()>d.length()) {
      kdDebug(90010) << "dub: dir " << file->absFilePath() << endl;
      subdirs.append(file->absFilePath());
    }
    if (file->isFile()) {
      // price for portability
      kdDebug(90010) << "dub: file " << file->absFilePath() << endl;
      KFileItem* item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
				      file->absFilePath(), true);
      file_items.append(item);
    }
  } // for

  init_traversal(forward);

  kdDebug(90010) << "dir node cons end" << endl;
}

void Dub::Dir_Node::init_traversal(bool forward)
{
  kdDebug(90010) << "init traversal" << endl;
  // initialize traversal information
  if (forward) {
    current_subdir = subdirs.begin();
    file_items.first();
  }
  else {
    current_subdir = subdirs.end();
    if (current_subdir!=subdirs.begin())
      current_subdir--; // last item
    else
      past_begin=true;
    file_items.last();
  }
  current_file = file_items.current();
  kdDebug(90010) << "current subdir " << *current_subdir << endl;
  kdDebug(90010) << "current file " << current_file << endl;
}

Dub::Recursive_Seq::Recursive_Seq()
{
  play_stack.setAutoDelete(true);
}

void Dub::Recursive_Seq::init(const KURL & root)
{
  QString new_root = canonical_path(root.path());
  if (recursion_root != new_root) {
    // change recursion stack
    recursion_root = new_root;
    kdDebug(90010) << "rec: new root is " << recursion_root << endl;
    play_stack.clear();
    push_dir(recursion_root); // start pre-order traversal
  }
}

// get canonical path, we need this
QString Dub::Recursive_Seq::canonical_path(QString dir)
{
//   kdDebug(90010) << "canonical_path " << dir << endl;
  //assert(dir.isLocalFile());
  QDir path(dir);
  return path.canonicalPath();
}

// check if dir is contained in the stack
bool Dub::Recursive_Seq::check_dir(QString dir)
{
  kdDebug(90010) << "check_dir " << dir << endl;
  bool found = false;
  for ( Dir_Node *cur_dir = play_stack.first();
        !found && cur_dir; cur_dir = play_stack.next() ) {
    if (cur_dir->dir==dir)
      found = true;
  }
  return found;
}

bool Dub::Recursive_Seq::push_dir(QString dir, bool forward)
{
  kdDebug(90010) << "push_dir " << dir << ", forward?" << forward << endl;
  QString cpath = canonical_path(dir);
  if (check_dir(cpath))		// is it in stack?
    return false;		// avoid infinite recursion
  else {
    Dir_Node* node = new Dir_Node(cpath, forward);
    play_stack.append(node);
    kdDebug(90010) << "stack after push:" << endl;
    print_stack();
    return true;
  }
}

bool Dub::Recursive_Seq::pop_dir()
{
  assert(!play_stack.isEmpty());
  kdDebug(90010) << "pop_dir" << endl;
  play_stack.removeLast();
  return !play_stack.isEmpty();
}

bool Dub::Recursive_Seq::advance(bool forward)
{
  Dir_Node* top = play_stack.getLast();
  kdDebug(90010) << "first child " << top->subdirs.first() << endl;
  kdDebug(90010) << "current child " << *top->current_subdir << endl;
  kdDebug(90010) << "last child " << top->subdirs.last() << endl;
  if (forward) {
    top->current_subdir++;	// advance dir
    return top->current_subdir!=top->subdirs.end();
  }
  else
    if (top->current_subdir!=top->subdirs.begin()) {
      top->current_subdir--;
      return true;
    }
    else {
      top->past_begin=true;
      return false;
    }
}

void Dub::Recursive_Seq::pop_preorder(bool forward)
{
  if (pop_dir()) {		// pop visited
    advance(forward);		// advance to next node
    Dir_Node* top = play_stack.getLast();
    kdDebug(90010) << "new child " << *top->current_subdir << endl;
    if (forward)
      next_preorder();		// continue processing
    else
      prev_preorder();		// continue processing
  }
  else {
    kdDebug(90010) << "push root" << endl;
    push_dir(recursion_root, forward);	// back to the beginning if at end
  }
}

void Dub::Recursive_Seq::next_preorder()
{
  assert(!play_stack.isEmpty());	// recursion stack cannot be empty
  kdDebug(90010) << "next_preorder, stack:" << endl;
  print_stack();
  Dir_Node* top = play_stack.getLast();
  if (top->subdirs.isEmpty() || top->current_subdir==top->subdirs.end()) {
    kdDebug(90010) << "rec: subtrees done" << endl;
    pop_preorder(true);    // pop if subtrees done
  }
  else {
    QString subdir = *top->current_subdir; // we have a subdir
    push_dir(subdir, true); // push directory w/ forward iterators
  }
}

void Dub::Recursive_Seq::prev_preorder()
{
  assert(!play_stack.isEmpty());	// recursion stack cannot be empty
  kdDebug(90010) << "prev_preorder, stack:" << endl;
  print_stack();
  Dir_Node* top = play_stack.getLast();
  if (top->subdirs.isEmpty() || top->past_begin) { // subtrees done?
    kdDebug(90010) << "rec: subtrees done" << endl;
    pop_preorder(false);
  }
  else {
    QString subdir = *top->current_subdir;
    kdDebug(90010) << "we have children, pushing now " << subdir << endl;
    push_dir(subdir, false); // push directory w/ backward iterators
  }
}

void Dub::Recursive_Seq::print_stack() {
  for ( Dir_Node *cur_dir = play_stack.first();
	cur_dir; cur_dir = play_stack.next() ) {
    kdDebug(90010) <<  cur_dir->dir << endl;
  }
}

Dub::Linear_Recursive::Linear_Recursive(Dub* d)
  : Sequencer(d) {
  kdDebug(90010) << "cons linear/recursive" << endl;
}

KFileItem* Dub::Linear_Recursive::first()
{
  KFileItem* first = bottom_dir()->file_items.getFirst();
  return first;
}

void Dub::Linear_Recursive::next()
{
  assert(!play_stack.isEmpty());
  Dir_Node* top = top_dir();
  QString dir = top->dir;
  top->current_file = top->file_items.next();
  kdDebug(90010) << "dub current dir: " << dir << endl;
  kdDebug(90010) << "dub current file: " << top->current_file << endl;
  bool cycle = false;
  while (!top_dir()->current_file && !cycle) {
    next_preorder();  // traverse until a non-empty dir or cycle
   if (top_dir()->dir==dir) {
     kdDebug(90010) << "we got a cycle" << endl;
     cycle = true;
     top_dir()->init_traversal(true);
   }
  }
  top = play_stack.getLast();
  kdDebug(90010) << "dub new dir: " << *top->current_subdir << endl;
  kdDebug(90010) << "dub new file: " << top->current_file << endl;
  if (top->current_file) {
    kdDebug(90010) << "dub new file: " << top->current_file->url() << endl;
    dub.activeFile = top->current_file;
    dub.fileSelected(dub.activeFile);
  }
}

void Dub::Linear_Recursive::prev()
{
  assert(!play_stack.isEmpty());
  Dir_Node* top = top_dir();
  QString dir = top->dir;
  top->current_file = top->file_items.prev();
  kdDebug(90010) << "dub current dir: " << dir << endl;
  kdDebug(90010) << "dub current file: " << top->current_file << endl;
  bool cycle = false;
  while (!top_dir()->current_file && !cycle) {
   prev_preorder();  // traverse until a non-empty dir or cycle
   if (top_dir()->dir==dir) {
     kdDebug(90010) << "we got a cycle" << endl;
     cycle = true;
     top_dir()->init_traversal(false);
   }
  }
  top = play_stack.getLast();
  kdDebug(90010) << "dub new dir: " << *top->current_subdir << endl;
  kdDebug(90010) << "dub new file: " << top->current_file << endl;
  if (top->current_file) {
    kdDebug(90010) << "dub new file: " << top->current_file->url() << endl;
    dub.activeFile = top->current_file;
    dub.fileSelected(dub.activeFile);
  }
}

void Dub::Shuffle_OneDir::init(const QString& dir)
{
  if (shuffle_dir != dir) {
    kdDebug(90010) << "shuffle/onedir init" << endl;
    shuffle_dir = dir;
    play_index = 0;

    // make a deep copy
    items.clear();
    QPtrList<KFileItem> & view_items = dub.view->items(); //
    for (KFileItem *file=view_items.first(); file; file=view_items.next() )
      if (file->isFile())	// add only files
	items.append(new KFileItem(*file));

    int num_items = items.count();
    play_order.resize(num_items);
    if (num_items) {		// generate shuffled order
      kdDebug(90010) << num_items << " file items" << endl;
      for (int i=0; i<num_items; i++)
	play_order[i] = i;
      Random random;
      Random::init();
      std::random_shuffle(play_order.begin(), play_order.end(), random);
    }
  }
}

KFileItem* Dub::Shuffle_OneDir::first()
{
  return 0;
}

void Dub::Shuffle_OneDir::next()
{
  kdDebug(90010) << "shuffle/onedir next" << endl;
  if (!items.isEmpty()) {
    play_index = ++play_index % play_order.size();
    dub.activeFile = items.at(play_order[play_index]);
    if (dub.activeFile)
      dub.fileSelected(dub.activeFile);
  }
}

void Dub::Shuffle_OneDir::prev()
{
  kdDebug(90010) << "shuffle/onedir prev" << endl;
  if (!items.isEmpty()) {
    play_index = --play_index % play_order.size();
    dub.activeFile = items.at(play_order[play_index]);
    if (dub.activeFile)
      dub.fileSelected(dub.activeFile);
  }
}

KFileItem* Dub::Shuffle_Recursive::random_file()
{
  assert(!play_stack.isEmpty());
  play_stack.clear();
  push_dir(recursion_root); // start pre-order traversal
  KFileItem* selected = 0;
  double file_probability = 0.3;
  Random::init();
  while (!top_dir()->subdirs.isEmpty() && !selected) {
    if (top_dir()->file_items.isEmpty()) {
      int ix = Random::random_int(top_dir()->subdirs.count());
      push_dir(top_dir()->subdirs[ix]);
    }
    else {
      if (Random::random_double(1.0)<file_probability) {
	int ix = Random::random_int(top_dir()->file_items.count());
	selected = top_dir()->file_items.at(ix);
      }
      else {
	int ix = Random::random_int(top_dir()->subdirs.count());
	push_dir(top_dir()->subdirs[ix]);
      }
    }
  }
  if (!selected) {
    if (!top_dir()->file_items.isEmpty()) {
      int ix = Random::random_int(top_dir()->file_items.count());
      selected = top_dir()->file_items.at(ix);
    }
  }
  return selected;
}

KFileItem* Dub::Shuffle_Recursive::first()
{
  return random_file();
}

void Dub::Shuffle_Recursive::next()
{
  KFileItem* file = random_file();
  if (file) {
    kdDebug(90010) << "shuffle/rec: new file: " << file->url() << endl;
    dub.activeFile = file;
    dub.fileSelected(file);
  }
}

void Dub::Shuffle_Recursive::prev()
{
  KFileItem* file = random_file();
  if (file) {
    kdDebug(90010) << "shuffle/rec: new file: " << file->url() << endl;
    dub.activeFile = file;
    dub.fileSelected(file);
  }
}
