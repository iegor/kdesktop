#include <qpushbutton.h>
#include <qpainter.h>

class KMidButton : public QPushButton
{
protected:
    
    QPixmap pixmap1,pixmap2;

    virtual void drawButton(QPainter *paint)
    {
        if ((isOn())&&(!pixmap1.isNull())) paint->drawPixmap(0,0,pixmap1);
        else if ((!isOn())&&(!pixmap2.isNull())) paint->drawPixmap(0,0,pixmap2);
    };
    
public:
    
    KMidButton (QWidget *parent,const char *name) : QPushButton (parent,name)
    {
    };
    
    ~KMidButton()
    {
    };
    
    void setPixmaps(const QPixmap& p1, const QPixmap& p2)
    {
        pixmap1=p1;
        pixmap2=p2;
    };
    
};
