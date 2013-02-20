/***************************************************************************
                          tcl_parser.cpp  -  description
                             -------------------
    begin                : Apr 2 2003
    author               : 2003 Massimo Callegari
    email                : massimocallegari@yahoo.it
 ***************************************************************************/
 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include "plugin_katesymbolviewer.h"

void KatePluginSymbolViewerView::parseTclSymbols(void)
{
  if (!win->viewManager()->activeView())
   return;

 QString currline, prevline;
 bool    prevComment = false;
 QString varStr("set ");
 QString procStr("proc");
 QString stripped;
 uint i, j, args_par = 0, graph = 0;
 char block = 0, parse_func = 0;

 QListViewItem *node = NULL;
 QListViewItem *mcrNode = NULL, *clsNode = NULL;
 QListViewItem *lastMcrNode = NULL, *lastClsNode = NULL;

 QPixmap mcr( ( const char** ) macro_xpm );
 QPixmap cls( ( const char** ) class_xpm );

 if(treeMode)
  {
   clsNode = new QListViewItem(symbols, symbols->lastItem(), i18n("Functions"));
   mcrNode = new QListViewItem(symbols, symbols->lastItem(), i18n("Globals"));
   lastMcrNode = mcrNode;
   lastClsNode = clsNode;
   if (expanded_on)
      {
       clsNode->setOpen(TRUE);
       mcrNode->setOpen(TRUE);
      }
   symbols->setRootIsDecorated(1);
  }
 else
   symbols->setRootIsDecorated(0);

 Kate::Document *kDoc = win->viewManager()->activeView()->getDoc();
 
 //positions.resize(kDoc->numLines() + 3); // Maximum symbols number o.O
 //positions.fill(0);
 
 for (i = 0; i<kDoc->numLines(); i++)
   {
    currline = kDoc->textLine(i);
    currline = currline.stripWhiteSpace();
    bool comment = false;
    kdDebug(13000)<<currline<<endl;
    if(currline.at(0) == '#') comment = true;

    if(i > 0)
      {
       prevline = kDoc->textLine(i-1);
       if(prevline.endsWith("\\") && prevComment) comment = true;
      }
    prevComment = comment;

    if(!comment)
      {
       if(currline.startsWith(varStr) && block == 0)
         {
          if (macro_on == true) // not really a macro, but a variable
            {
             stripped = currline.right(currline.length() - 3);
             stripped = stripped.simplifyWhiteSpace();
             int fnd = stripped.find(' ');
             //fnd = stripped.find(";");
             if(fnd > 0) stripped = stripped.left(fnd);

             if (treeMode)
               {
                node = new QListViewItem(mcrNode, lastMcrNode, stripped);
                lastMcrNode = node;
               }
             else
                node = new QListViewItem(symbols, symbols->lastItem(), stripped);

             node->setPixmap(0, (const QPixmap &)mcr);
             node->setText(1, QString::number( i, 10));
             stripped = "";
            }//macro
          } // starts with "set"

       else if(currline.startsWith(procStr)) { parse_func = 1; }
       
       if (parse_func == 1)
             {
              for (j = 0; j < currline.length(); j++)
                 {
                  if (block == 1)
                    {
                     if(currline.at(j)=='{') graph++;
                     if(currline.at(j)=='}')
                       {
                        graph--;
                        if (graph == 0) { block = 0; parse_func = 0; continue; }
                       }
                    }                  
                  if (block == 0)
                    {
                     stripped += currline.at(j);
                     if(currline.at(j) == '{') args_par++;
                     if(currline.at(j) == '}') 
                         {
                          args_par--;
                          if (args_par == 0) 
                            {
                             //stripped = stripped.simplifyWhiteSpace();
                             if(func_on == true)
                               {
                                if (treeMode)
                                  {
                                   node = new QListViewItem(clsNode, lastClsNode, stripped);
                                   lastClsNode = node;
                                  }
                                else
                                   node = new QListViewItem(symbols, symbols->lastItem(), stripped);
                                node->setPixmap(0, (const QPixmap &)cls);
                                node->setText(1, QString::number( i, 10));
                               }                             
                             stripped = "";
                             block = 1;
                            }
                         }
                    } // block = 0
                  } // for j loop
               }//func_on
      } // not a comment
    } //for i loop

 //positions.resize(symbols->itemIndex(node) + 1);
}

