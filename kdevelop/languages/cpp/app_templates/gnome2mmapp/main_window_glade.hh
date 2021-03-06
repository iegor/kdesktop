%{HH_TEMPLATE}

// DO NOT EDIT THIS FILE ! It was created using
// glade-- /home/amp8165/Projects/gnome2mm/gnome2mm.glade
// for gtk 2.2.4 and gtkmm 1.2.10
//
// Please modify the corresponding derived classes in ./src/main_window.hh and./src/main_window.cc

#ifndef _MAIN_WINDOW_GLADE_HH
#  define _MAIN_WINDOW_GLADE_HH


#if !defined(GLADEMM_DATA)
#define GLADEMM_DATA 
#include <gtk--/accelgroup.h>

class GlademmData
{  
        
        Gtk::AccelGroup *accgrp;
public:
        
        GlademmData(Gtk::AccelGroup *ag) : accgrp(ag)
        {  
        }
        
        Gtk::AccelGroup * getAccelGroup()
        {  return accgrp;
        }
};
#endif //GLADEMM_DATA

#include <gtk--/window.h>

class main_window_glade : public Gtk::Window
{  
        
        GlademmData *gmm_data;
public:
        class Gtk::Window *main_window;
protected:
        
        main_window_glade();
        
        ~main_window_glade();

        virtual gint quit(GdkEventAny *ev) = 0;
};
#endif
