%{H_TEMPLATE}

#ifndef _%{APPNAMEUC}_H_
#define _%{APPNAMEUC}_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kmainwindow.h>

#include "%{APPNAMELC}view.h"

class KPrinter;
class KURL;

/**
 * This class serves as the main window for %{APPNAME}.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class %{APPNAMELC} : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    %{APPNAMELC}();

    /**
     * Default Destructor
     */
    virtual ~%{APPNAMELC}();

private slots:

    void fileNew();
    void optionsPreferences();
    void newToolbarConfig();

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);

private:

    void setupActions();

private:
    %{APPNAMELC}View *m_view;

    KPrinter   *m_printer;
};

#endif // _%{APPNAMEUC}_H_

