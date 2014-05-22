/*
name: Ray Sun
id: r7sun
student number: 20343049
*/
#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow();
  
protected:
  virtual bool on_key_press_event( GdkEventKey *ev );

private:
  // A "vertical box" which holds everything in our window
  Gtk::VBox m_vbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar m_menubar;

  Gtk::RadioButtonGroup tools;
  Gtk::RadioButtonGroup speeds;

  Glib::RefPtr<Gtk::RadioMenuItem> m_low_elem;
  Glib::RefPtr<Gtk::RadioMenuItem> m_med_elem;
  Glib::RefPtr<Gtk::RadioMenuItem> m_high_elem;

  Glib::RefPtr<Gtk::RadioMenuItem> m_face_elem;
  Glib::RefPtr<Gtk::RadioMenuItem> m_wire_elem;
  Glib::RefPtr<Gtk::RadioMenuItem> m_mult_elem;

  Gtk::Menu_Helpers::RadioMenuElem m_low;
  Gtk::Menu_Helpers::RadioMenuElem m_med;
  Gtk::Menu_Helpers::RadioMenuElem m_high;

  Gtk::Menu_Helpers::RadioMenuElem m_face;
  Gtk::Menu_Helpers::RadioMenuElem m_wire;
  Gtk::Menu_Helpers::RadioMenuElem m_mult;

  // Each menu itself
  Gtk::Menu m_menu_app;
  Gtk::Menu m_menu_draw;
  Gtk::Menu m_menu_speed;
  Gtk::Menu m_menu_blind;

  // The main OpenGL area
  Viewer m_viewer;

  sigc::slot1<void, Viewer::cubeType> mode_slot;
  sigc::slot1<void, int> speed_slot;
  sigc::slot0<bool> tick;
  sigc::slot0<bool> tickGame;
  sigc::connection tickConn;

  void speed(int s);
  void dmode(int t);
  void reMenu();
};

#endif
