/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#include <klocale.h>

void PropertiesDialog::setPlayObject( PlaylistItem pi, Arts::PlayObject po )
{
    // Arts::PlayObject properties
    if (!po.isNull())
    {
	Arts::poCapabilities ncaps = po.capabilities();
	QCheckListItem *item;

	descriptionLabel->setText( po.description().c_str() );

	// Determine capabilities
	if (!(item = (QCheckListItem *)capsList->findItem( "capSeek", 0 )))
	{
	    item = new QCheckListItem( capsList, "capSeek",
				       QCheckListItem::CheckBox );
	}
	item->setOn( (ncaps & Arts::capSeek) );

	if (!(item = (QCheckListItem *)capsList->findItem( "capPause", 0 )))
	{
	    item = new QCheckListItem( capsList, "capPause",
				       QCheckListItem::CheckBox );
	}
	item->setOn( (ncaps & Arts::capPause) );

	// Defaults
	nameField->setText( i18n("unknown") );
	typeLabel->setText( i18n("unknown") );
	lengthLabel->setText( i18n("unknown") );
	audioLabel->setText( i18n("unknown") );
	videoLabel->setText( i18n("unknown") );
    }

    // PlaylistItem properties (name and mimetype)
    if (!pi.isNull())
    {
	setCaption( i18n("Properties for %1").arg(pi.url().fileName()) );

	KSharedPtr<KMimeType> mime = KMimeType::mimeType( pi.mimetype() );
	iconLabel->setPixmap( mime->pixmap( KIcon::Desktop, KIcon::SizeMedium ) );

	nameField->setText( pi.url().fileName() );
	typeLabel->setText( pi.mimetype() );
	lengthLabel->setText( pi.lengthString() );
    }
}
