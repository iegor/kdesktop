/* 
   Copyright (c) 2001 Malte Starostik <malte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// $Id: svgcreator.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef __svgcreator_h__
#define __svgcreator_h__

#include <kio/thumbcreator.h>

class SVGCreator : public QObject, public ThumbCreator
{
    Q_OBJECT
public:
    SVGCreator();
    virtual ~SVGCreator();
    virtual bool create(const QString &path, int width, int height, QImage &img);
    virtual Flags flags() const;

private slots:
    void slotFinished();

private:
    bool m_finished;
};

#endif

// vim: ts=4 sw=4 noet
