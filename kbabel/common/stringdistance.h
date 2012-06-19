/* ****************************************************************************
    Copyright (C) 2003-2004 Eva Brucherseifer <eva.brucherseifer@basyskom.com>
		  2005	    Stanislav Visnovsky <visnovsky@kde.org>

    This file is part of the KDE project

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#ifndef STRINGDISTANCE_H
#define STRINGDISTANCE_H

#include <qstring.h>

//#include <boost/numeric/ublas/matrix.hpp>


/** Private copy constructor and copy assignment ensure classes derived from
  * class noncopyable cannot be copied.
  * Taken from Boost library
  * Contributed by Dave Abrahams
  *
  * If anyone needs a namespace here, please tell me at eva@rt.e-technik.tu-darmstadt.de
  */
class NonCopyable
{
protected:
        NonCopyable(){}
        virtual ~NonCopyable(){}
private:  // emphasize the following members are private
        NonCopyable( const NonCopyable& );
        const NonCopyable& operator=( const NonCopyable& );
};


/**
  * @class Distance
  * @author Eva Brucherseifer
  *
  * The class Distance calculates the distance
  * between two Entities (left & right).
  * It is the parent for other distance-classes.
  */
class Distance : public NonCopyable
{
public:
	virtual ~Distance(){}
	double operator()(const QString& left, const QString& right);

	int editCostReplace() { return editCost_replace_base; }
	static int debug;
	
protected:
	virtual double calculate(const QString& left_string, const QString& right_string) = 0;
	int nodeDistance(const QString& left_letter, const QString& right_letter);
	static const int editCost_replace_base;
	double m_distance;
};


double relativeDistance(double distance, const QString& left_string, const QString right_string);


/**
  * @class HammingDistance
  * @author Eva Brucherseifer
  *
  * The class HammingDistance is based on an algorithm
  * of Hamming. It increase the distance if the nodes from
  * the tree are not the same. Also called edit-distance.
  */
class HammingDistance : public Distance
{
protected:
	virtual double calculate(const QString& left_string, const QString& right_string);
	int editCostReplace() { return editCost; }
	static const int editCost;
};

/**
  * @class LevenshteinDistance
  * @author Eva Brucherseifer
  *
  * The class LevenshteinDistance is based on an algorithm
  * of Levenshtein. It search for the minimum distance of
  * two trees. You can specify the distance between
  * a gap & a node and between two different nodes.
  */
class LevenshteinDistance : public Distance
{
protected:
	virtual double calculate(const QString& left_string, const QString& right_string);
	int editCostReplace() { return editCost_replace; }
	static const int editCost_replace;
	static const int editCost_insert;
	static const int editCost_delete;
};


/** wrapper function for replacement of fstrcmp from gettext */
inline double fstrcmp(const QString& left, const QString& right)
{
	return LevenshteinDistance()(left,right);
}


#endif
