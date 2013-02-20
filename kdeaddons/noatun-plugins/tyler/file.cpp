/*
// Copyright (C) 2000 Julien Carme
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

extern "C"
{
#include "renderer.h"
#include <stdlib.h>
}

#include <qfile.h>
#include <kglobal.h>
#include <kstandarddirs.h>

static QString getDataPath(void)
{
	KStandardDirs &dirs = *KGlobal::dirs();

	return dirs.findResource("data", "noatun/tylerstates");
}

static QString getSavePath(void)
{
	KStandardDirs &dirs = *KGlobal::dirs();
	return dirs.saveLocation("data", "noatun/") + "tylerstates";
}

extern "C" void save_effect(t_effect *effect)
{
	QFile file(getSavePath());
	if(!file.open(IO_WriteOnly))
		return;

	for(unsigned i = 0; i < sizeof(t_effect); i++)
		file.putch( *((byte *)effect + i) );
}

t_effect effects[100];
int nb_effects=0;

extern "C" void load_effects()
{
	QFile file(getDataPath());
	if(!file.open(IO_ReadOnly))
		exit(1);

	unsigned int i;
	nb_effects = 0;
	while(!file.atEnd())
	{
		byte* ptr_effect = (byte *)&effects[nb_effects++];
		for(i = 0; i < sizeof(t_effect); i++)
			ptr_effect[i] = file.getch();
	}
}

extern "C" void load_random_effect(t_effect *effect)
{
	if(nb_effects > 0)
	{
		int num_effect = rand() % nb_effects;
		unsigned int i;

		for(i = 0; i < sizeof(t_effect) ; i++)
			*((byte*)effect+i)=*((byte*)(&effects[num_effect])+i);
	}
}
