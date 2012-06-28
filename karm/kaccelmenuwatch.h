/*
* kaccelmenuwatch.h -- Declaration of class KAccelMenuWatch.
* Generated by newclass on Thu Jan  7 15:05:26 EST 1999.
*/
#ifndef KARM_K_ACCEL_MENU_WATCH_H
#define KARM_K_ACCEL_MENU_WATCH_H

#include <qobject.h>
#include <qptrlist.h>

#include <kaccel.h>

class QPopupMenu;

/**
 * Easy updating of menu accels when changing a KAccel object. 
 *
 * Since a KAccel object does not keep track of menu items to which it
 * is connected, we normally have to manually call 
 * KAccel::changeMenuAccel() again for each update of the KAccel object.
 *
 * With KAccelMenuWatch you provide the kaccel object and the menu
 * items to which it connects, and if you update the kaccel you just have
 * to call KAccelMenuWatch::updateMenus() and the menu items will be updated.
 *
 * It is safe to delete menus that have connections handled by this class.
 * On deletion of a menu, all associated accelerators will be deleted.
 *
 * Note that you _have_ to call KAccelMenuWatch::updateMenus() after you
 * connect the * accelerators, as they are not activated till then.
 * 
 * @author Sirtaj Singh Kang (taj@kde.org)
 */

class KAccelMenuWatch : public QObject
{
  Q_OBJECT

  private:
    enum AccelType { StdAccel, StringAccel };

    typedef struct AccelItem {
      QPopupMenu  *menu;
      int itemId;

      AccelType type;

      // only one of these is used at a time
      QString action;
      KStdAccel::StdAccel stdAction;
    } AccelItem;

    KAccel *_accel;
    QPtrList<AccelItem> _accList;
    QPtrList<QPopupMenu> _menuList;

    QPopupMenu  *_menu;

    KAccelMenuWatch::AccelItem *newAccelItem( QPopupMenu *menu, 
                                              int itemId, AccelType type );

  public:
    /**
     * KAccelMenuWatch Constructor
     */
    KAccelMenuWatch( KAccel *accel, QObject *parent = 0 );

    /**
     * KAccelMenuWatch Destructor
     */
    virtual ~KAccelMenuWatch() {}

    /** 
     * Set the menu on which connectAccel calls will operate.
     * All subsequent calls to connectAccel will be associated
     * with this menu. You can call this function any number of
     * times, so multiple menus can be handled.
     */
    void setMenu( QPopupMenu *menu );

    /** 
     * Return the last menu set with KAccelMenuWatch::setMenu(QPopupMenu*),
     * or 0 if none has been set.
     */
    QPopupMenu *currentMenu() const  { return _menu; }

    /** 
     * Connect the menu item identified to currentMenu()/id to
     * the accelerator action.
     */
    void connectAccel( int itemId, const char *action );

    /** 
     * Same as above, but connects to standard accelerators.
     */
    void connectAccel( int itemId, KStdAccel::StdAccel );
  
  public slots:
    /** 
     * Updates all menu accelerators. Call this after all accelerators
     * have been connected or the kaccel object has been updated.
     */
    void updateMenus();

  private slots:
    void removeDeadMenu();

  private:
    KAccelMenuWatch& operator=( const KAccelMenuWatch& );
    KAccelMenuWatch( const KAccelMenuWatch& );
};

#endif // KARM_K_ACCEL_MENU_WATCH_H
