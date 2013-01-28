#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <qdatastream.h>

#include "lib/libksirtet_export.h"


struct ClientPlayData { Q_UINT8 height, gift, end; };
LIBKSIRTET_EXPORT QDataStream &operator <<(QDataStream &s, const ClientPlayData &d);
LIBKSIRTET_EXPORT QDataStream &operator >>(QDataStream &s, ClientPlayData &d);

struct ServerPlayData { Q_UINT8 prevHeight, nextHeight, gift; };
LIBKSIRTET_EXPORT QDataStream &operator <<(QDataStream &s, const ServerPlayData &d);
LIBKSIRTET_EXPORT QDataStream &operator >>(QDataStream &s, ServerPlayData &d);

class ServerInitData
{
 public:
	QString  prevName, nextName, name;
	Q_UINT32 initLevel, seed;
};
LIBKSIRTET_EXPORT QDataStream &operator <<(QDataStream &s, const ServerInitData &d);
LIBKSIRTET_EXPORT QDataStream &operator >>(QDataStream &s, ServerInitData &d);

#endif
