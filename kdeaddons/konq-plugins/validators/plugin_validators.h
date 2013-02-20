/* This file is part of Validators
 *
 *  It's a merge of the HTML- and the CSSValidator
 *
 *  Copyright (C) 2001 by  Richard Moore <rich@kde.org>
 *                         Andreas Schlapbach <schlpbch@iam.unibe.ch>
 *
 *  for information how to write your own plugin see:
 *    http://developer.kde.org/documentation/tutorials/dot/writing-plugins.html
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

/* $Id: plugin_validators.h 465369 2005-09-29 14:33:08Z mueller $ */

#ifndef __plugin_validators_h
#define __plugin_validators_h

#include <kparts/plugin.h>
#include <klibloader.h>

#include "validatorsdialog.h"
#include <qguardedptr.h>

class KAction;
class KURL;

class PluginValidators : public KParts::Plugin
{
  Q_OBJECT
public:
  PluginValidators( QObject* parent, const char* name,
                    const QStringList & );
  virtual ~PluginValidators();

public slots:
  void slotValidateHTML();
  void slotValidateCSS();
  void slotValidateLinks();
  void slotConfigure();

private slots:
  void slotStarted( KIO::Job* );

private:
  KActionMenu *m_menu;
  QGuardedPtr<ValidatorsDialog> m_configDialog; // |
                                    // +-> Order dependency.
  KHTMLPart* m_part;                // |

  KURL m_WWWValidatorUrl, m_WWWValidatorUploadUrl;
  KURL m_CSSValidatorUrl, m_CSSValidatorUploadUrl;
  KURL m_linkValidatorUrl;

  void setURLs();
  void validateURL(const KURL &url, const KURL &uploadUrl = KURL());
};

#endif
