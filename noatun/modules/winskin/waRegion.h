#ifndef WAREGION_H
#define WAREGION_H

#include <qcolor.h>
#include <qstring.h>

class WaRegion {
public:
    WaRegion(QString filename);
    ~WaRegion();

    const QBitmap *mainWindowMask() const { return window_mask; }
    const QBitmap *mainWindowShadeMask() const { return shade_mask; }

private:
   QValueList<int> parseList(const QString &list) const;
   void buildPixmap(const QValueList<int> &num_points, const QValueList<int> &point_list, QBitmap *dest);

   QBitmap *window_mask;
   QBitmap *shade_mask;
};

extern WaRegion *windowRegion;

#endif

