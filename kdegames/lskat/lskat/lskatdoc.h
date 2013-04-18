/***************************************************************************
                          lskatdoc.h  -  description
                             -------------------
    begin                : Tue May  2 15:47:11 CEST 2000
    copyright            : (C) 2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LSKATDOC_H
#define LSKATDOC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for QT
#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>

#include <kconfig.h>
#include "lskat.h"
#include "KEInput.h"

// forward declaration of the LSkat classes
class LSkatView;

/**	LSkatDoc provides a document object for a document-view model.
  *
  * The LSkatDoc class provides a document object that can be used in conjunction with the classes LSkatApp and LSkatView
  * to create a document-view model for standard KDE applications based on KApplication and KTMainWindow. Thereby, the document object
  * is created by the LSkatApp instance and contains the document structure with the according methods for manipulation of the document
  * data by LSkatView objects. Also, LSkatDoc contains the methods for serialization of the document data from and to files.
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team. 	
  * @version KDevelop version 0.4 code generation
  */
class LSkatDoc : public QObject
{
  Q_OBJECT
  public:
    /** Constructor for the fileclass of the application */
    LSkatDoc(QWidget *parent, const char *name=0);
    /** Destructor for the fileclass of the application */
    ~LSkatDoc();

    /** adds a view to the document which represents the document contents. Usually this is your main view. */
    void addView(LSkatView *view);
    /** removes a view from the list of currently connected views */
    void removeView(LSkatView *view);
    /** sets the modified flag for the document after a modifying action on the view connected to the document.*/
    void setModified(bool _m=true){ modified=_m; }
    /** returns if the document is modified or not. Use this to determine if your document needs saving by the user on closing.*/
    bool isModified(){ return modified; }
    /** "save modified" - asks the user for saving if the document is modified */
    bool saveModified();	
    /** deletes the document's contents */
    void deleteContents();
    /** initializes the document generally */
    bool newDocument(KConfig *config,QString path);
    /** closes the acutal document */
    void closeDocument();
    /** loads the document by filename and format and emits the updateViews() signal */
    bool openDocument(const QString &filename, const char *format=0);
    /** saves the document under filename and format.*/	
    bool saveDocument(const QString &filename, const char *format=0);
    /** sets the path to the file connected with the document */
    void setAbsFilePath(const QString &filename);
    /** returns the pathname of the current document file*/
    const QString &getAbsFilePath() const;
    /** sets the filename of the document */
    void setTitle(const QString &_t);
    /** returns the title of the document */
    const QString &getTitle() const;

   int random(int max);
   void NewGame();
   void EndGame(bool aborted=false);
   void ClearStats();
   void EvaluateGame();
   int GetCard(int player,int pos,int height=0);
   int GetHeight(int player, int pos);
   void SetHeight(int player, int pos,int h);
   int GetMove(int i);
   int PrepareMove(int player,int pos);
   int MakeMove();
   bool IsRunning();
   CCOLOUR GetTrump();
   void SetTrump(CCOLOUR i);
   int GetMoveStatus();
   void SetMoveStatus(int i);
   int GetCurrentPlayer();
   void SetCurrentPlayer(int i);
   int GetStartPlayer();
   void SetStartPlayer(int i);
   void SetName(int no,QString n);
   QString GetName(int no);
   int GetScore(int no);
   int GetMoveNo();
   bool LegalMove(int card1, int card2);
   int WonMove(int card1,int card2);
   int CardValue(int card);
   /*
   int GetDeckNo();
   void SetDeckNo(int no);
   */
   int GetLastStartPlayer();
   KG_INPUTTYPE GetPlayedBy(int no);
   void SetPlayedBy(int no,KG_INPUTTYPE type);
   int GetComputerLevel();
   void SetComputerLevel(int lev);
   void SetComputerScore(int sc);
   int GetComputerScore();
   void SetLock();
   void ReleaseLock();
   bool IsLocked();
   bool IsIntro();
   bool WasRunning();
   void SetIntro(bool b);

   // Stats
   int GetStatWon(int no);
   int GetStatGames(int no);
   int GetStatAborted(int no);
   int GetStatPoints(int no);

   void WriteConfig(KConfig *config);
   void ReadConfig(KConfig *config);
   
   void SetInputHandler(KEInput *i);
   KEInput *QueryInputHandler();
   void InitMove(LSkatView *sender,int player,int x,int y);
   int SwitchStartPlayer();
   void UpdateViews(int mode);
   bool IsServer();
   void SetServer(bool b);
   void SetHost(QString h);
   void SetName(const QString& n);
   void SetPort(short p);
   QString QueryHost();
   short QueryPort();
   QString QueryName() const;
   // Only for fast remote access
   int *GetCardP();
   int *GetCardHeightP();
   void SetCard(int i,int c);
   bool IsRemoteSwitch();
   void SetRemoteSwitch(bool b);
   QString GetProcess();
   int LoadCards(QString path);
   int LoadDeck(QString path);
   bool SetCardDeckPath(QString deck,QString card);
   QString GetDeckpath() {return deckPath;}
   QString GetCardpath() {return cardPath;}
   bool LoadGrafix(QString path);

  protected:
  void initrandom();
  int LoadBitmap(QString path);

public:
  QPixmap mPixCard[NO_OF_CARDS];
  QPixmap mPixTrump[NO_OF_TRUMPS];
  QPixmap mPixDeck;
  QPixmap mPixBackground;
  QSize cardsize;
  QPixmap mPixType[3];
  QPixmap mPixAnim[NO_OF_ANIM];

  private:
  QString procfile;
  QString picpath;
  int delpath;
  bool remoteswitch;
  KEInput *inputHandler;
  short port;
  QString host;
  QString Name;
  bool server;
  bool lock;
  int startplayer;
  int cardheight[16];
  int card[NO_OF_CARDS];
  int curmove[2];
  int moveno;
  int isrunning;
  bool isintro;
  bool wasgame;
  int movestatus;
  int currentplayer;
  CCOLOUR trump;
  QString names[2];
  int score[2];
  int laststartplayer;
  int began_game;

  KG_INPUTTYPE playedby[2];
  int computerlevel;
  int computerscore;

  int stat_won[2];      // how many wins
  int stat_points[2];   // how many points
  int stat_games[2];    // how many games
  int stat_brk[2];    // how many games aborted

  int cardvalues[14];
	
  public slots:
    /** calls repaint() on all views connected to the document object and is called by the view by which the document has been changed.
     * As this view normally repaints itself, it is excluded from the paintEvent.
     */
    void slotUpdateAllViews(LSkatView *sender);
 	
  public:	
    /** the list of the views currently connected to the document */
    static QPtrList<LSkatView> *pViewList;	

  private:
    /** the modified flag of the current document */
    bool modified;
    QString title;
    QString absFilePath;
    QString deckPath,cardPath;
};

#endif // LSKATDOC_H