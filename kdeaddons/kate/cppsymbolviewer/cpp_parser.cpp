/***************************************************************************
                          cpp_parser.cpp  -  description
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

void KatePluginSymbolViewerView::parseCppSymbols(void)
{
  if (!win->viewManager()->activeView())
   return;

 QString cl; // Current Line
 QString stripped;
 uint i, j, tmpPos = 0;
 int par = 0, graph = 0, retry = 0;
 char mclass = 0, block = 0, comment = 0; // comment: 0-no comment 1-inline comment 2-multiline comment 3-string
 char macro = 0, macro_pos = 0, func_close = 0;
 bool structure = false;
 QPixmap cls( ( const char** ) class_xpm );
 QPixmap sct( ( const char** ) struct_xpm );
 QPixmap mcr( ( const char** ) macro_xpm );
 QPixmap mtd( ( const char** ) method_xpm );
 QListViewItem *node = NULL;
 QListViewItem *mcrNode = NULL, *sctNode = NULL, *clsNode = NULL, *mtdNode = NULL;
 QListViewItem *lastMcrNode = NULL, *lastSctNode = NULL, *lastClsNode = NULL, *lastMtdNode = NULL;


 Kate::Document *kv = win->viewManager()->activeView()->getDoc();
 //kdDebug(13000)<<"Lines counted :"<<kv->numLines()<<endl;
 if(treeMode)
   {
    mcrNode = new QListViewItem(symbols, symbols->lastItem(), i18n("Macros"));
    sctNode = new QListViewItem(symbols, symbols->lastItem(), i18n("Structures"));
    clsNode = new QListViewItem(symbols, symbols->lastItem(), i18n("Functions"));
    mcrNode->setPixmap(0, (const QPixmap &)mcr);
    sctNode->setPixmap(0, (const QPixmap &)sct);
    clsNode->setPixmap(0, (const QPixmap &)cls);
    if (expanded_on)
      {
       mcrNode->setOpen(TRUE);
       sctNode->setOpen(TRUE);
       clsNode->setOpen(TRUE);
      }
    lastMcrNode = mcrNode;
    lastSctNode = sctNode;
    lastClsNode = clsNode;
    mtdNode = clsNode;
    lastMtdNode = clsNode;
    symbols->setRootIsDecorated(1);
   }
 else symbols->setRootIsDecorated(0);

 for (i=0; i<kv->numLines(); i++)
   {
    cl = kv->textLine(i);
    cl = cl.stripWhiteSpace();
    func_close = 0;
    if(cl.at(0) == '/' && cl.at(1) == '/') continue;
    if(cl.find("/*") == 0 && (cl.find("*/") == ((signed)cl.length() - 2)) && graph == 0) continue; // workaround :(
    if(cl.find("/*") >= 0 && graph == 0) comment = 1;
    if(cl.find("*/") >= 0 && graph == 0) comment = 0;
    if(cl.find("#") >= 0 && graph == 0 ) macro = 1;
    if (comment != 1)
      {
       /* *********************** MACRO PARSING *****************************/
       if(macro == 1)
         {
          //macro_pos = cl.find("#");
          for (j = 0; j < cl.length(); j++)
             {
              if(cl.at(j)=='/' && cl.at(j+1)=='/') { macro = 4; break; }
              if(  (uint)cl.find("define") == j &&
                 !((uint)cl.find("defined") == j))
                    {
                     macro = 2;
                     j += 6; // skip the word "define"
                    }
              if(macro == 2 && cl.at(j) != ' ') macro = 3;
              if(macro == 3)
                {
                 if (cl.at(j) >= 0x20) stripped += cl.at(j);
                 if (cl.at(j) == ' ' || j == cl.length() - 1)
                         macro = 4;
                }
              //kdDebug(13000)<<"Macro -- Stripped : "<<stripped<<" macro = "<<macro<<endl;
             }
           // I didn't find a valid macro e.g. include
           if(j == cl.length() && macro == 1) macro = 0;
           if(macro == 4)
             {
              //stripped.replace(0x9, " ");
              stripped = stripped.stripWhiteSpace();
              if (macro_on == true)
                 {
                  if (treeMode)
                    {
                     node = new QListViewItem(mcrNode, lastMcrNode, stripped);
                     lastMcrNode = node;
                    }
                  else node = new QListViewItem(symbols, symbols->lastItem(), stripped);
                  node->setPixmap(0, (const QPixmap &)mcr);
                  node->setText(1, QString::number( i, 10));
                 }
              macro = 0;
              macro_pos = 0;
              stripped = "";
              //kdDebug(13000)<<"Macro -- Inserted : "<<stripped<<" at row : "<<i<<endl;
              if (cl.at(cl.length() - 1) == '\\') macro = 5; // continue in rows below
              continue;
             }
          }
       if (macro == 5)
          {
           if (cl.at(cl.length() - 1) != '\\')
              macro = 0;
           continue;
          }

       /* ******************************************************************** */

       if ((cl.find("class") >= 0 && graph == 0 && block == 0))
         {
          mclass = 1;
          for (j = 0; j < cl.length(); j++)
             {
              if(cl.at(j)=='/' && cl.at(j+1)=='/') { mclass = 2; break; }
              if(cl.at(j)=='{') { mclass = 4; break;}
              stripped += cl.at(j);
             }
          if(func_on == true)
            {
             if (treeMode)
               {
                node = new QListViewItem(clsNode, lastClsNode, stripped);
                if (expanded_on) node->setOpen(TRUE);
                lastClsNode = node;
                mtdNode = lastClsNode;
                lastMtdNode = lastClsNode;
               }
             else node = new QListViewItem(symbols, symbols->lastItem(), stripped);
             node->setPixmap(0, (const QPixmap &)cls);
             node->setText(1, QString::number( i, 10));
             stripped = "";
             if (mclass == 1) mclass = 3;
            }
          continue;
         }
       if (mclass == 3)
         {
          if (cl.find('{') >= 0) 
            {
             cl = cl.right(cl.find('{'));
             mclass = 4;
            }
         }

       if(cl.find("(") >= 0 && cl.at(0) != '#' && block == 0 && comment != 2)
          { structure = false; block = 1; }
       if((cl.find("typedef") >= 0 || cl.find("struct") >= 0) &&
          graph == 0 && block == 0)
         { structure = true; block = 2; stripped = ""; }
       //if(cl.find(";") >= 0 && graph == 0)
       //    block = 0;

       if(block > 0 && mclass != 1 )
         {
          for (j = 0; j < cl.length(); j++)
            {
             if (cl.at(j) == '/' && (cl.at(j + 1) == '*')) comment = 2;
             if (cl.at(j) == '*' && (cl.at(j + 1) == '/')) {  comment = 0; j+=2; }
             // Handles a string. Those are freaking evilish !
             if (cl.at(j) == '"' && comment == 3) { comment = 0; j++; }
             else if (cl.at(j) == '"' && comment == 0) comment = 3;
             if(cl.at(j)=='/' && cl.at(j+1)=='/' && comment == 0)
               { if(block == 1 && stripped.isEmpty()) block = 0; break; }
             if (comment != 2 && comment != 3)
               {
                if (block == 1 && graph == 0 )
                  {
                   if(cl.at(j) >= 0x20) stripped += cl.at(j);
                   if(cl.at(j) == '(') par++;
                   if(cl.at(j) == ')')
                     {
                      par--;
                      if(par == 0)
                        {
                         stripped = stripped.stripWhiteSpace();
                         stripped.remove("static ");
                         //kdDebug(13000)<<"Function -- Inserted : "<<stripped<<" at row : "<<i<<endl;
                         block = 2;
                         tmpPos = i;
                        }
                     }
                  } // BLOCK 1
                if(block == 2 && graph == 0)
                  {
                   if(cl.at(j)=='/' && cl.at(j+1)=='/' && comment == 0) break;
                   //if(cl.at(j)==':' || cl.at(j)==',') { block = 1; continue; }
                   if(cl.at(j)==':') { block = 1; continue; }
                   if(cl.at(j)==';')
                     {
                      stripped = "";
                      block = 0;
                      structure = false;
                      break;
                     }

                   if(cl.at(j)=='{' && structure == false && cl.find(";") < 0 ||
                      cl.at(j)=='{' && structure == false && cl.find('}') > (int)j)
                     {
                      stripped.replace(0x9, " ");
                      if(func_on == true)
                        {
                         if (types_on == false)
                           {
                            while (stripped.find('(') >= 0)
                              stripped = stripped.left(stripped.find('('));
                            while (stripped.find("::") >= 0)
                              stripped = stripped.mid(stripped.find("::") + 2);
                            stripped = stripped.stripWhiteSpace();
                            while (stripped.find(0x20) >= 0)
                              stripped = stripped.mid(stripped.find(0x20, 0) + 1);
                           }
                         //kdDebug(13000)<<"Function -- Inserted: "<<stripped<<" at row: "<<tmpPos<<" mclass: "<<(uint)mclass<<endl;
                         if (treeMode)
                           {
                            if (mclass == 4)
                              {
                               node = new QListViewItem(mtdNode, lastMtdNode, stripped);
                               lastMtdNode = node;
                              }
                            else 
                              {
                               node = new QListViewItem(clsNode, lastClsNode, stripped);
                               lastClsNode = node;
                              }
                           }
                         else
                             node = new QListViewItem(symbols, symbols->lastItem(), stripped);
                         if (mclass == 4) node->setPixmap(0, (const QPixmap &)mtd);
                         else node->setPixmap(0, (const QPixmap &)cls);
                         node->setText(1, QString::number( tmpPos, 10));
                        }
                      stripped = "";
                      retry = 0;
                      block = 3;
                     }
                   if(cl.at(j)=='{' && structure == true)
                     {
                      block = 3;
                      tmpPos = i;
                     }
                   if(cl.at(j)=='(' && structure == true)
                     {
                      retry = 1;
                      block = 0;
                      j = 0;
                      //kdDebug(13000)<<"Restart from the beginning of line..."<<endl;
                      stripped = "";
                      break; // Avoid an infinite loop :(
                     }
                   if(structure == true && cl.at(j) >= 0x20) stripped += cl.at(j);
                  } // BLOCK 2

                if (block == 3)
                  {
                   // A comment...there can be anything
                   if(cl.at(j)=='/' && cl.at(j+1)=='/' && comment == 0) break;
                   if(cl.at(j)=='{') graph++;
                   if(cl.at(j)=='}')
                     {
                      graph--;
                      if (graph == 0 && structure == false) { block = 0; func_close = 1; }
                      if (graph == 0 && structure == true) block = 4;
                     }
                  } // BLOCK 3

                if (block == 4)
                  {
                   if(cl.at(j) == ';')
                     {
                      //stripped.replace(0x9, " ");
                      stripped.remove('{');
                      stripped.replace('}', " ");
                      if(struct_on == true)
                        {
                         if (treeMode)
                           {
                            node = new QListViewItem(sctNode, lastSctNode, stripped);
                            lastSctNode = node;
                           }
                         else node = new QListViewItem(symbols, symbols->lastItem(), stripped);
                         node->setPixmap(0, (const QPixmap &)sct);
                         node->setText(1, QString::number( tmpPos, 10));
                        }
                      //kdDebug(13000)<<"Structure -- Inserted : "<<stripped<<" at row : "<<i<<endl;
                      stripped = "";
                      block = 0;
                      structure = false;
                      //break;
                      continue;
                     }
                   if (cl.at(j) >= 0x20) stripped += cl.at(j);
                  } // BLOCK 4
               } // comment != 2
             //kdDebug(13000)<<"Stripped : "<<stripped<<" at row : "<<i<<endl;
            } // End of For cycle
         } // BLOCK > 0
       if (mclass == 4 && block == 0 && func_close == 0)
         {
          if (cl.find('}') >= 0) 
            {
             cl = cl.right(cl.find('}'));
             mclass = 0;
            }
         }
      } // Comment != 1
   } // for kv->numlines

 //for (i= 0; i < (symbols->itemIndex(node) + 1); i++)
 //    kdDebug(13000)<<"Symbol row :"<<positions.at(i) <<endl;
}


