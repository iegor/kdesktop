/* Qt Plugin interface for the QtCurve style
 *
 * Taken from B???Curve from R?dH?t
 *
 * (C) Craig Drummond, 2003
 * --------
 *
 * (c) 2002 R?d H?t, Inc.
 * Programmed by Bernhard Rosenkraenzer <bero@r?dh?t.com>
 * Released under the GNU General Public License (GPL) version 2.
 */
#include <qstyleplugin.h>
#include "qindustrial.h"
#include "config.h"

class QIndustrialStylePlugin : public QStylePlugin
{
public:
    QIndustrialStylePlugin();

    QStringList keys() const;
    QStyle *create(const QString &);
};

QIndustrialStylePlugin::QIndustrialStylePlugin()
    : QStylePlugin()
{
}

QStringList QIndustrialStylePlugin::keys() const
{
    QStringList list;
    list << "QIndustrial";
    return list;
}

QStyle * QIndustrialStylePlugin::create(const QString &s)
{
    if (s.lower() == "qindustrial")
	return new Industrial::QIndustrialStyle;
    return 0;
}

Q_EXPORT_PLUGIN( QIndustrialStylePlugin )
