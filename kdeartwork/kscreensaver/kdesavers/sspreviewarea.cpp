//============================================================================
//
// KRotation screen saver for KDE
// Copyright (C) 2004 Georg Drenkhahn
// $Id: sspreviewarea.cpp 304354 2004-04-16 23:42:39Z kniederk $
//
//============================================================================

#include "sspreviewarea.h"
#include "sspreviewarea.moc"

SsPreviewArea::SsPreviewArea(QWidget* parent, const char* name)
   : QWidget(parent, name)
{
}

void SsPreviewArea::resizeEvent(QResizeEvent* e)
{
   emit resized(e);
}

