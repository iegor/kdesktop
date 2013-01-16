/* ui.h
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by M. Laurent and W. Bastian.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
/* UI definitions... Coding these things twice (with and without KDE)
 * wouldn't make sense...						*/

#ifndef _UI_H
#define _UI_H

#ifdef USE_KDE
#include <kmessagebox.h>
#include <klocale.h>
#include <ktabctl.h>
#include <kdialog.h>
#include <kfiledialog.h>
#define _(x) i18n(x)
#define WarningYesNo(parent, caption, text) \
	KMessageBox::warningYesNo(parent, text, caption)
#define ErrorOK(parent, caption, text) \
	KMessageBox::sorry(parent, text, caption)
#define InformationOK(parent, text, caption, dontShowAgainName) \
	KMessageBox::information(parent, text, caption, dontShowAgainName)
#define CriticalErrorOK(parent, text, caption) \
	KMessageBox::error(parent, text, caption)
#define TabBar KTabCtl
#define FileDlg KFileDialog
#define SPACE_MARGIN KDialog::marginHint()
#define SPACE_INSIDE KDialog::spacingHint()
#else
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qdialog.h>
#include <qfiledialog.h>
#define _(x) tr(x)
#define WarningYesNo(parent, caption, text) \
	QMessageBox::warning(parent, caption, text, QMessageBox::Yes, QMessageBox::No)
#define ErrorOK(parent, caption, text) \
	QMessageBox::information(parent, caption, text)
#define InformationOK(parent, text, caption, dontShowAgainName) \
	QMessageBox::information(parent, caption, text)
#define CriticalErrorOK(parent, text, caption) \
	QMessageBox::critical(parent, caption, text)
#define TabBar QTabWidget
#define FileDlg QFileDialog
#define SPACE_MARGIN 5
#define SPACE_INSIDE 5
#endif
#define IDYES 3
#define IDNO 4

#endif

