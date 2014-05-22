/*
name: Ray Sun
id: r7sun
student number: 20343049
*/
#include "appwindow.hpp"
#include <iostream>

AppWindow::AppWindow():
m_low(speeds,"_Slow", Gtk::AccelKey("1"), sigc::bind(sigc::mem_fun(*this, &AppWindow::speed), 500)),
m_med(speeds,"_Med", Gtk::AccelKey("2"), sigc::bind(sigc::mem_fun(*this, &AppWindow::speed), 300)),
m_high(speeds,"_High", Gtk::AccelKey("3"), sigc::bind(sigc::mem_fun(*this, &AppWindow::speed), 100)),
m_face(tools,"_Face", Gtk::AccelKey("f"), sigc::bind(sigc::mem_fun(*this, &AppWindow::dmode), 1)),
m_wire(tools,"_Wire-frame", Gtk::AccelKey("w"), sigc::bind(sigc::mem_fun(*this, &AppWindow::dmode), 2)),
m_mult(tools,"_Multicolored", Gtk::AccelKey("m"), sigc::bind(sigc::mem_fun(*this, &AppWindow::dmode), 3))
{
  //set_title("488 Tetrominoes on the Wall");
  set_title("GTKmm gave me terminal cancer");

  // A utility class for constructing things that go into menus, which we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  using Gtk::Menu_Helpers::RadioMenuElem;

  //update timer
  tick = sigc::mem_fun(m_viewer, &Viewer::update);
  Glib::signal_timeout().connect(tick, 15);

  tickGame = sigc::mem_fun(m_viewer, &Viewer::updateGame);
  tickConn = Glib::signal_timeout().connect(tickGame, 500);

  // Set up the application menu:
  // The slot we use here just causes AppWindow::hide() on this, which shuts down the application.
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"), sigc::mem_fun(*this, &AppWindow::hide)));
  m_menu_app.items().push_back(MenuElem("_New Game", Gtk::AccelKey("n"), sigc::mem_fun(m_viewer, &Viewer::newGame)));
  m_menu_app.items().push_back(MenuElem("_Reset", Gtk::AccelKey("r"), sigc::mem_fun(m_viewer, &Viewer::viewInit)));

  m_menu_draw.items().push_back(m_face);
  m_menu_draw.items().push_back(m_wire);
  m_menu_draw.items().push_back(m_mult);

  m_face_elem = Glib::RefPtr<Gtk::RadioMenuItem>::cast_dynamic<Gtk::MenuItem>(m_face.get_child());
  m_wire_elem = Glib::RefPtr<Gtk::RadioMenuItem>::cast_dynamic<Gtk::MenuItem>(m_wire.get_child());
  m_mult_elem = Glib::RefPtr<Gtk::RadioMenuItem>::cast_dynamic<Gtk::MenuItem>(m_mult.get_child());

  m_menu_speed.items().push_back(m_low);
  m_menu_speed.items().push_back(m_med);
  m_menu_speed.items().push_back(m_high);

  m_low_elem = Glib::RefPtr<Gtk::RadioMenuItem>::cast_dynamic<Gtk::MenuItem>(m_low.get_child());
  m_med_elem = Glib::RefPtr<Gtk::RadioMenuItem>::cast_dynamic<Gtk::MenuItem>(m_med.get_child());
  m_high_elem = Glib::RefPtr<Gtk::RadioMenuItem>::cast_dynamic<Gtk::MenuItem>(m_high.get_child());

  m_menu_blind.items().push_back(MenuElem("_Toggle Blinding Light", Gtk::AccelKey("b"), sigc::mem_fun(m_viewer, &Viewer::toggleBlind)));

  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Draw Mode", m_menu_draw));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Speed", m_menu_speed));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Blinding Mode", m_menu_blind));

  // Pack in our widgets:
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget by default, so it'll take up the rest of the window.
  m_viewer.set_size_request(400,600);
  m_vbox.pack_start(m_viewer);

  show_all();
}

void AppWindow::speed(int s){
  tickConn.disconnect();
  tickConn = Glib::signal_timeout().connect(tickGame, s);
}

void AppWindow::dmode(int type){
  switch(type){
    case 1:
      m_viewer.setMode(Viewer::face);
      break;
    case 2:
      m_viewer.setMode(Viewer::wire);
      break;
    case 3:
      m_viewer.setMode(Viewer::multi);
      break;
  }
}

void AppWindow::reMenu(){
  m_menu_speed.items().pop_back();
  m_menu_speed.items().pop_back();
  m_menu_speed.items().pop_back();
  m_menu_draw.items().pop_back();
  m_menu_draw.items().pop_back();
  m_menu_draw.items().pop_back();
  m_menu_speed.items().push_back(m_low);
  m_menu_speed.items().push_back(m_med);
  m_menu_speed.items().push_back(m_high);
  m_menu_draw.items().push_back(m_face);
  m_menu_draw.items().push_back(m_wire);
  m_menu_draw.items().push_back(m_mult);
}

bool AppWindow::on_key_press_event(GdkEventKey *e){
  switch(e->keyval){
  case '1':
    m_low_elem->set_active(true);
    //reMenu();
    break;
  case '2':
    m_med_elem->set_active(true);
    //reMenu();
    break;
  case '3':
    m_high_elem->set_active(true);
    //reMenu();
    break;
  case 'w':case 'W':
    m_wire_elem->set_active(true);
    //reMenu();
    break;
  case 'f':case 'F':
    m_face_elem->set_active(true);
    //reMenu();
    break;
  case 'm':case 'M':
    m_mult_elem->set_active(true);
    //reMenu();
    break;
  }
  m_viewer.keyHandle(e);
  return Gtk::Window::on_key_press_event(e);
}
