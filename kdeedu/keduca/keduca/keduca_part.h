/*
 *
 * KEduca - Interactive Tests and Exams for KDE
 *
 * Copyright (c)
 *
 * 2004: Henrique Pinto <henrique.pinto@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KEDUCA_PART_H
#define KEDUCA_PART_H

#include <kparts/part.h>

class KEducaView;
class KAboutData;

/**
 * KPart component for KEduca
 * @author Henrique Pinto <henrique.pinto@kdemail.net>
 */
class KEducaPart : public KParts::ReadOnlyPart
{
	Q_OBJECT
	public:
		/*
		 * Constructor.
		 */
		KEducaPart( QWidget *parentWidget, const char *widgetName, QObject *parent,
		            const char *name, const QStringList &args );

		/*
		 * Destructor.
		 */
		virtual ~KEducaPart();

		/*
		 * Returns information about this part.
		 */
		static KAboutData* createAboutData();

	protected:
		/*
		 * Opens the file indicated in the m_file member variable.
		 * Returns true on success, false if errors occured.
		 */
		virtual bool openFile();
		
	protected slots:
		/*
		 * Shows the configuration dialog.
		 */
		void slotConfigure();
		
		/*
		 * Updates the configuration in case the user has changed it in
		 * the Config Dialog.
		 */
		void updateConfiguration();

	private:
		/*
		 * Sets the KActions for this part.
		 */
		void setupActions();
		
		/*
		 * The widget for this part.
		 */
		KEducaView *m_view;
};

#endif // KEDUCA_PART_H
