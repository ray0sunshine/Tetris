/*
name: Ray Sun
id: r7sun
student number: 20343049
*/
#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP
#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "game.hpp"

class Viewer:public Gtk::GL::DrawingArea{
public:
  //types of cubes to draw
  enum cubeType {wire,face,multi};
  enum transType {xrot,yrot,zrot,scl,none};

  //struct for point containing normal
  struct n3f{
    float x;
    float y;
    float z;
  };

  struct pt3f{
    float x;
    float y;
    float z;
    n3f n;
  };

  struct pt{
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    float r;
    float g;
    float b;
  };

  struct shineCube{
    float x;
    float y;
    float d;
  };

  std::vector<shineCube> shines;

  Viewer();
  virtual ~Viewer();

  //causes rerender
  void invalidate();

  //resets variables
  void init();
  void viewInit();

  //called by timer
  bool update();
  bool updateGame();

  //pre-computes un weighted vertex normals from the standard cube center
  void preCompNorm();

  //does initial transformations for rotation, scale and translation, lighting and floor should be independent of game well
  void mainOrient();
  void reflOrient();

  //draws the "mirrored ground"
  void drawGround();

  //calls all the polygon drawing
  void drawState();

  void drawShine();

  //inits the lights and draws the indicator
  void lighting();

  // Called when keys pressed
  void keyHandle(GdkEventKey* e);

  //draws unitcube at x,y,z of type w/e
  void unitCube(float x, float y, float z, cubeType type);
  void unitShine(float x, float y, float z, cubeType type);

  //draws a 8 vertex polygon
  void polyFace(pt3f p1, pt3f p2, pt3f p3, pt3f p4, pt3f p5, pt3f p6, pt3f p7, pt3f p8);
  void polyWire(pt3f p1, pt3f p2, pt3f p3, pt3f p4, pt3f p5, pt3f p6, pt3f p7, pt3f p8);

  //draws a triangle
  void triFace(pt3f p1, pt3f p2, pt3f p3);
  void triWire(pt3f p1, pt3f p2, pt3f p3);

  //draw a hex on XZ plane
  void heXYZ(float x, float y, float z);

  void shine(pt3f p1, pt3f p2);

  void particles();
  void randPart(float x, float y, float z);
  void randPart2(float x, float y, float z);
  float rd(float high, float low);
  float rad(float deg);

  void newGame();
  void setMode(cubeType type);

  void toggleBlind();

private:
  float angleAxisX;
  float angleAxisY;
  float angleAxisZ;
  float angleAxisXActive;
  float angleAxisYActive;
  float angleAxisZActive;
  float lightX;
  float lightY;
  float lightZ;

  float vX;
  float mx;
  float mbx;
  float mBaseX;
  bool mDown;

  float scale;
  float baseScale;
  cubeType renderMode;
  transType trans;
  Game* game;
  bool gameEnd;
  bool blinding;

  std::vector<pt> pts;

  //normals
  n3f nftrx,nftry,nftrz,nftlx,nftly,nftlz,nfbrx,nfbry,nfbrz,nfblx,nfbly,nfblz,nbtrx,nbtry,nbtrz,nbtlx,nbtly,nbtlz,nbbrx,nbbry,nbbrz,nbblx,nbbly,nbblz;
  
protected:
  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);
};

#endif
