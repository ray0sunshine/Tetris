/*
name: Ray Sun
id: r7sun
student number: 20343049
*/
#include "viewer.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define boardHeight 20
#define boardWidth 10
#define mainColor 0.8
#define subColor 0.2
#define floorHeight -13
#define floorRadius 18
#define edge 0.5 //edge offset from center
#define coff 0.25 //corner offset
#define faceWeight 0.7 //weight of normal using face normal
#define vertWeight 0.3 //weight of normal using vertex to center normal
#define brt 3
#define damp 10
#define minZoom 0.6
#define maxZoom 3
#define xOff 6.0
#define yOff 12.0
#define grav -0.01
#define timeSlow 0.9

//read games public remove list, remember to reset all the elements to -1 afterwards
//do reading on update 60fps
//also update particles

Viewer::Viewer(){
  Glib::RefPtr<Gdk::GL::Config> glconfig;
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH | Gdk::GL::MODE_DOUBLE);
  if(glconfig == 0){std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;abort();}
  set_gl_capability(glconfig);
  add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK | Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::VISIBILITY_NOTIFY_MASK);
  init();
}

Viewer::~Viewer(){}

void Viewer::invalidate(){
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}

void Viewer::on_realize(){
  // Do some OpenGL setup:
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable(); 
  if (!gldrawable) return;
  if (!gldrawable->gl_begin(get_gl_context())) return;

  // Just enable depth testing and set the background colour.
  glEnable(GL_DEPTH_TEST);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
  glClearColor(0.4, 0.4, 0.8, 0.0);
  gldrawable->gl_end();
}

bool Viewer::on_configure_event(GdkEventConfigure* event){
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context())) return false;

  // Set up perspective projection, using current size and aspect ratio of display
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

void Viewer::preCompNorm(){
  nftrx.x = (edge-coff)*vertWeight; nftrx.y = (edge)*vertWeight; nftrx.z = (edge)*vertWeight;
  nftry.x = (edge)*vertWeight; nftry.y = (edge-coff)*vertWeight; nftry.z = (edge)*vertWeight;
  nftrz.x = (edge)*vertWeight; nftrz.y = (edge)*vertWeight; nftrz.z = (edge-coff)*vertWeight;
  nftlx.x = (-edge+coff)*vertWeight; nftlx.y = (edge)*vertWeight; nftlx.z = (edge)*vertWeight;
  nftly.x = (-edge)*vertWeight; nftly.y = (edge-coff)*vertWeight; nftly.z = (edge)*vertWeight;
  nftlz.x = (-edge)*vertWeight; nftlz.y = (edge)*vertWeight; nftlz.z = (edge-coff)*vertWeight;
  nfbrx.x = (edge-coff)*vertWeight; nfbrx.y = (-edge)*vertWeight; nfbrx.z = (edge)*vertWeight;
  nfbry.x = (edge)*vertWeight; nfbry.y = (-edge+coff)*vertWeight; nfbry.z = (edge)*vertWeight;
  nfbrz.x = (edge)*vertWeight; nfbrz.y = (-edge)*vertWeight; nfbrz.z = (edge-coff)*vertWeight;
  nfblx.x = (-edge+coff)*vertWeight; nfblx.y = (-edge)*vertWeight; nfblx.z = (edge)*vertWeight;
  nfbly.x = (-edge)*vertWeight; nfbly.y = (-edge+coff)*vertWeight; nfbly.z = (edge)*vertWeight;
  nfblz.x = (-edge)*vertWeight; nfblz.y = (-edge)*vertWeight; nfblz.z = (edge-coff)*vertWeight;
  nbtrx.x = (edge-coff)*vertWeight; nbtrx.y = (edge)*vertWeight; nbtrx.z = (-edge)*vertWeight;
  nbtry.x = (edge)*vertWeight; nbtry.y = (edge-coff)*vertWeight; nbtry.z = (-edge)*vertWeight;
  nbtrz.x = (edge)*vertWeight; nbtrz.y = (edge)*vertWeight; nbtrz.z = (-edge+coff)*vertWeight;
  nbtlx.x = (-edge+coff)*vertWeight; nbtlx.y = (edge)*vertWeight; nbtlx.z = (-edge)*vertWeight;
  nbtly.x = (-edge)*vertWeight; nbtly.y = (edge-coff)*vertWeight; nbtly.z = (-edge)*vertWeight;
  nbtlz.x = (-edge)*vertWeight; nbtlz.y = (edge)*vertWeight; nbtlz.z = (-edge+coff)*vertWeight;
  nbbrx.x = (edge-coff)*vertWeight; nbbrx.y = (-edge)*vertWeight; nbbrx.z = (-edge)*vertWeight;
  nbbry.x = (edge)*vertWeight; nbbry.y = (-edge+coff)*vertWeight; nbbry.z = (-edge)*vertWeight;
  nbbrz.x = (edge)*vertWeight; nbbrz.y = (-edge)*vertWeight; nbbrz.z = (-edge+coff)*vertWeight;
  nbblx.x = (-edge+coff)*vertWeight; nbblx.y = (-edge)*vertWeight; nbblx.z = (-edge)*vertWeight;
  nbbly.x = (-edge)*vertWeight; nbbly.y = (-edge+coff)*vertWeight; nbbly.z = (-edge)*vertWeight;
  nbblz.x = (-edge)*vertWeight; nbblz.y = (-edge)*vertWeight; nbblz.z = (-edge+coff)*vertWeight;
}

void Viewer::init(){
  game = new Game(boardWidth,boardHeight);
  game->reset();
  viewInit();
  blinding = false;
  renderMode = Viewer::face;
  trans = Viewer::none;
  preCompNorm();
  gameEnd = false;
}

void Viewer::viewInit(){
  angleAxisX = 0.0;
  angleAxisY = 0.0;
  angleAxisZ = 0.0;
  angleAxisXActive = 0.0;
  angleAxisYActive = 0.0;
  angleAxisZActive = 0.0;
  lightX = 8.0;
  lightY = 16.0;
  lightZ = 8.0;
  mDown = false;
  vX = 0;
  mx = 0;
  mbx = 0;
  scale = 1;
  baseScale = 1;
}

bool Viewer::updateGame(){
  int stat = game->tick();
  if(stat < 0){
    gameEnd = true;
  }
  return true;
}

bool Viewer::update(){
  if(mDown){
    if(mx-mbx == 0){
      vX /= 3;
    }else{
      vX = (mx - mbx)/damp;
      if(vX > 1){
        vX = std::sqrt(vX);
      }else if(vX < -1){
        vX = -std::sqrt(abs(vX));
      }
    }
    mbx = mx;

    switch(trans){
      case Viewer::none:
        break;
      case Viewer::xrot:
        angleAxisXActive = angleAxisX + (mx - mBaseX)/damp;break;
      case Viewer::yrot:
        angleAxisYActive = angleAxisY + (mx - mBaseX)/damp;break;
      case Viewer::zrot:
        angleAxisZActive = angleAxisZ + (mx - mBaseX)/damp;break;
      case Viewer::scl:
        float nscl = baseScale + 0.002*(mx - mBaseX);
        if(nscl > minZoom && nscl < maxZoom){
          scale = nscl;
        }else if(nscl > maxZoom){
          scale = maxZoom;
        }else if(nscl < minZoom){
          scale = minZoom;
        }
        break;
    }
  }else{
    switch(trans){
      case Viewer::xrot:
        angleAxisXActive += vX;
        angleAxisX = angleAxisXActive;
        break;
      case Viewer::yrot:
        angleAxisYActive += vX;
        angleAxisY = angleAxisYActive;
        break;
      case Viewer::zrot:
        angleAxisZActive += vX;
        angleAxisZ = angleAxisZActive;
        break;
      case Viewer::none:
      case Viewer::scl:
        break;
    }
  }

  invalidate();
  return true;
}

void Viewer::toggleBlind(){
  if(!blinding){
    viewInit();
  }
  blinding = !blinding;
}

bool Viewer::on_expose_event(GdkEventExpose* event){
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  if (!gldrawable) return false;
  if (!gldrawable->gl_begin(get_gl_context())) return false;

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Modify the current projection matrix so that we move the camera away from the origin.  We'll draw the game at the origin, and we need to back up to see it.
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glTranslated(0.0, 0.0, -42.0);

  // Reset the matrix to identity
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  lighting();
  mainOrient();
  drawState();

  glLoadIdentity();
  reflOrient();
  drawState();

  glLoadIdentity();
  glScaled(scale,scale,scale);
  drawGround();
  particles();

  glLoadIdentity();
  mainOrient();
  if(blinding){
    glBegin(GL_QUADS);
    glNormal3f(0,5,0);
    float midOff = 2;
    for(float a=1; a>=0; a-=0.1){
      glColor4f(1,1,1,a);
      glVertex3f(0+a+midOff,0+a+midOff,-1);
      glVertex3f(0+a+midOff,2*yOff-a-midOff,-1);
      glVertex3f(2*xOff-a-midOff,2*yOff-a-midOff,-1);
      glVertex3f(2*xOff-a-midOff,0+a+midOff,-1);
    }
    glEnd();
  }

  glLoadIdentity();
  mainOrient();
  if(blinding){
    drawShine();
  }

  // We pushed a matrix onto the PROJECTION stack earlier, we need to pop it.
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // Swap the contents of the front and back buffers so we see what we just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  return true;
}

float Viewer::rd(float high, float low){
  return low + (((float)rand())/((float)RAND_MAX))*(high-low);
}

float Viewer::rad(float deg){
  return deg*3.141592653589793/180;
}

void Viewer::randPart(float x, float y, float z){
  Viewer::pt npt;
  npt.x = x;
  npt.y = y;
  npt.z = z;
  npt.vx = rd(0.5,-0.5);
  npt.vy = rd(0.5,-0.1);
  npt.vz = rd(0.5,-0.5);
  npt.r = rd(1 , 0.8);
  npt.g = rd(1 , 0.4);
  npt.b = rd(1 , 0);
  pts.push_back(npt);
}

void Viewer::randPart2(float x, float y, float z){
  Viewer::pt npt;
  npt.x = x;
  npt.y = y;
  npt.z = z;
  npt.vx = rd(0.15,-0.15);
  npt.vy = rd(0.25, 0);
  npt.vz = rd(0.15,-0.15);
  npt.r = rd(1 , 0.5);
  npt.g = rd(1 , 0.2);
  npt.b = rd(1 , 0);
  pts.push_back(npt);
}

void Viewer::particles(){
  for(int i=0; i<4; i++){
    int r = game->rem[i];
    if(r >= 0){
      for(float x=-xOff+1; x<xOff-1; x+=1){
        float xx = x;
        float z = 0;
        float y = 1.5 + r - yOff;
        float nx, ny, nz;
        float rads;

        rads = rad(angleAxisZActive);
        nx = xx*cos(rads) - y*sin(rads);
        ny = xx*sin(rads) + y*cos(rads);
        xx = nx;
        y = ny;
        
        rads = rad(angleAxisYActive);
        nz = z*cos(rads) - xx*sin(rads);
        nx = z*sin(rads) + xx*cos(rads);
        z = nz;
        xx = nx;
        
        rads = rad(angleAxisXActive);
        ny = y*cos(rads) - z*sin(rads);
        nz = y*sin(rads) + z*cos(rads);
        y = ny;
        z = nz;

        for(int c=0; c<40; c++){
          randPart2(xx,y,z);
        }

        for(int c=0; c<20; c++){
          randPart(xx,y,z);
        }
      }
      game->rem[i] = -1;
    }
  }

  std::vector<Viewer::pt> npts;
  glLineWidth(rd(3 , 1));
  glBegin(GL_LINES);
  for(uint i=0; i<pts.size(); i++){
    glColor4f(pts[i].r,pts[i].g,pts[i].b,1);
    glVertex3f(pts[i].x,pts[i].y,pts[i].z);
    pts[i].vy += grav;
    pts[i].x += (pts[i].vx)/timeSlow;
    pts[i].y += (pts[i].vy)/timeSlow;
    pts[i].z += (pts[i].vz)/timeSlow;
    glVertex3f(pts[i].x,pts[i].y,pts[i].z);
    if(pts[i].y > floorHeight){
      npts.push_back(pts[i]);
    }else if(pts[i].vy < -0.2){
      pts[i].vy = -0.33*pts[i].vy;
      pts[i].y = floorHeight;
      npts.push_back(pts[i]);
    }
  }
  glEnd();
  pts.swap(npts);
}

void Viewer::mainOrient(){
  glScaled(scale,scale,scale);
  glRotated(angleAxisXActive, 1.0, 0.0, 0.0);
  glRotated(angleAxisYActive, 0.0, 1.0, 0.0);
  glRotated(angleAxisZActive, 0.0, 0.0, 1.0);
  glTranslated(-xOff, -yOff, 0.0);
}

void Viewer::reflOrient(){
  glScaled(scale,-scale,scale);
  glTranslated(0, 26.0, 0);
  glRotated(angleAxisXActive, 1.0, 0.0, 0.0);
  glRotated(angleAxisYActive, 0.0, 1.0, 0.0);
  glRotated(angleAxisZActive, 0.0, 0.0, 1.0);
  glTranslated(-xOff, -yOff, 0);
}

void Viewer::drawGround(){
  glBegin(GL_QUADS);
  glColor4f(0,0,0,1.0);
  glVertex3f(-3*floorRadius,floorHeight,2*floorRadius);
  glVertex3f(3*floorRadius,floorHeight,2*floorRadius);
  glColor4f(0,0,0,0);
  glVertex3f(3*floorRadius,floorHeight,-2*floorRadius);
  glVertex3f(-3*floorRadius,floorHeight,-2*floorRadius);
  glEnd();

  glLineWidth(1);

  glColor4f(1,1,1,0.2);
  for(float x=-3*floorRadius; x<3*floorRadius; x+=4){
    for(float z=-2*floorRadius; z<2*floorRadius; z+=3){
      heXYZ(x,floorHeight + 0.1,z);
    }
  }

  for(float x=-3*floorRadius+2; x<3*floorRadius+2; x+=4){
    for(float z=-2*floorRadius-1.5; z<2*floorRadius; z+=3){
      heXYZ(x,floorHeight + 0.1,z);
    }
  }

  glBegin(GL_QUADS);
  glColor4f(0.4,0.4,0.8,0);
  glVertex3f(-3*floorRadius,floorHeight+0.2,-1*floorRadius);
  glVertex3f(3*floorRadius,floorHeight+0.2,-1*floorRadius);
  glColor4f(0.4,0.4,0.8,1.0);
  glVertex3f(3*floorRadius,floorHeight+0.2,-2*floorRadius-3);
  glVertex3f(-3*floorRadius,floorHeight+0.2,-2*floorRadius-3);
  glEnd();
}

void Viewer::lighting(){
  glShadeModel (GL_SMOOTH);
  GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat mat_shininess[] = {50.0};
  GLfloat light_position[] = {lightX, lightY, lightZ, 1};
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 10.0);
  glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glBegin(GL_TRIANGLES);
  glVertex3f(lightX-0.2, lightY-0.2, lightZ);
  glVertex3f(lightX+0.2, lightY-0.2, lightZ);
  glVertex3f(lightX, lightY+0.2, lightZ);
  glEnd();
}

// You'll be drawing unit cubes, so the game will have width 10 and height 24 (game = 20, stripe = 4).  Let's translate the game so that we can draw it starting at (0,0) but have it appear centered in the window.
// Well edges: X[0 to 10] y[0 to 20]
// split drawstate with draw reflexion and draw extra
void Viewer::drawState(){
  glColor4f(0.4,0.4,0.4,1);
  for(float x=0.5; x<12; x+=1){
    unitCube(x,0.5,0,renderMode);
  }

  for(float y=1.5; y<21; y+=1){
    unitCube(0.5,y,0,renderMode);
    unitCube(11.5,y,0,renderMode);
  }

  for(int x=0; x<boardWidth; x++){
    for(int y=0; y<boardHeight+4; y++){
      if(game->get(y,x) >= 0){
        switch(game->get(y,x)){
        case 0:
          glColor4f(mainColor,mainColor,mainColor,1);
          break;
        case 1:
          glColor4f(mainColor,mainColor,subColor,1);
          break;
        case 2:
          glColor4f(mainColor,subColor,mainColor,1);
          break;
        case 3:
          glColor4f(mainColor,subColor,subColor,1);
          break;
        case 4:
          glColor4f(subColor,mainColor,mainColor,1);
          break;
        case 5:
          glColor4f(subColor,mainColor,subColor,1);
          break;
        case 6:
          glColor4f(subColor,subColor,mainColor,1);
          break;
        case 7:
          glColor4f(subColor,subColor,subColor,1);
          break;
        }
        unitCube((float)x+1.5,(float)y+1.5,0,renderMode);
      }
    }
  }
}

struct compClass{
  bool operator() (Viewer::shineCube a, Viewer::shineCube b){
    return a.d > b.d;
  }
}compare;

void Viewer::drawShine(){
  shines.clear();
  for(int x=0; x<boardWidth; x++){
    for(int y=0; y<boardHeight+2; y++){
      if(game->get(y,x) < 0){
        Viewer::shineCube nc;
        nc.x = (float)x+1.5;
        nc.y = (float)y+1.5;
        nc.d = (xOff - nc.x)*(xOff - nc.x) + (yOff - nc.y)*(yOff - nc.y);
        shines.push_back(nc);
      }
    }
  }

  //sort based so that be can render those with furthest distance first
  std::sort(shines.begin(), shines.end(), compare);

  //loop and call unitShine
  for(uint i=0; i<shines.size(); i++){
    unitShine(shines[i].x, shines[i].y, 0, renderMode);
  }
}

void Viewer::newGame(){
  gameEnd = false;
  game->reset();
}

void Viewer::setMode(Viewer::cubeType type){
  renderMode = type;
}

void Viewer::keyHandle(GdkEventKey* e){
  float move = 0.5;
  switch (e->keyval){
  case 'w':case 'W':
    renderMode = Viewer::wire;break;
  case 'i':case 'I':
    lightY += move;break;
  case 'j':case 'J':
    lightX -= move;break;
  case 'k':case 'K':
    lightY -= move;break;
  case 'l':case 'L':
    lightX += move;break;
  case 'r':case 'R':
    viewInit();break;
  case 'n':case 'N':
    newGame();break;
  case 'f':case 'F':
    renderMode = Viewer::face;break;
  case 'm':case 'M':
    renderMode = Viewer::multi;break;
  case 't':case 'T':
    game->rem[0] = 0;
    game->rem[1] = 18;
    break;
  case ' ':
    if(!gameEnd){
      game->drop();
      game->tick();
    }
    break;
  case GDK_Left:
    if(!gameEnd)
      game->moveLeft();break;
  case GDK_Right:
    if(!gameEnd)
      game->moveRight();break;
  case GDK_Up:
    if(!gameEnd)
      game->rotateCCW();break;
  case GDK_Down:
    if(!gameEnd)
      game->rotateCW();break;
  }
}

bool Viewer::on_button_press_event(GdkEventButton* e){
  mBaseX = e->x;
  mx = mBaseX;
  mbx = mBaseX;
  mDown = true;
  angleAxisXActive = angleAxisX;
  angleAxisYActive = angleAxisY;
  angleAxisZActive = angleAxisZ;
  scale = baseScale;
  if(e->state & GDK_SHIFT_MASK){
    trans = Viewer::scl;
  }else{
    switch(e->button){
      case 1:
        trans = Viewer::xrot;break;
      case 2:
        trans = Viewer::yrot;break;
      case 3:
        trans = Viewer::zrot;break;
    }
  }
  vX = 0;
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* e){
  mDown = false;
  angleAxisX = angleAxisXActive;
  angleAxisY = angleAxisYActive;
  angleAxisZ = angleAxisZActive;
  baseScale = scale;
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* e){
  mx = e->x;
  return true;
}

void Viewer::heXYZ(float x, float y, float z){
  glBegin(GL_LINE_LOOP);
  float h = 1.4;
  float v = 0.4;
  glNormal3f(0,brt*scale,0);
  glVertex3f(x-h,y,z);
  glVertex3f(x-v,y,z-h);
  glVertex3f(x+v,y,z-h);
  glVertex3f(x+h,y,z);
  glVertex3f(x+v,y,z+h);
  glVertex3f(x-v,y,z+h);
  glEnd();
}

//gotta fix faces to have weighted normals
void Viewer::polyFace(pt3f p1, pt3f p2, pt3f p3, pt3f p4, pt3f p5, pt3f p6, pt3f p7, pt3f p8){
  float mx = ((p1.n.x + p2.n.x + p3.n.x + p4.n.x + p5.n.x + p6.n.x + p7.n.x + p8.n.x)/8)*faceWeight;
  float my = ((p1.n.y + p2.n.y + p3.n.y + p4.n.y + p5.n.y + p6.n.y + p7.n.y + p8.n.y)/8)*faceWeight;
  float mz = ((p1.n.z + p2.n.z + p3.n.z + p4.n.z + p5.n.z + p6.n.z + p7.n.z + p8.n.z)/8)*faceWeight;
  glBegin(GL_POLYGON);
  glNormal3f((p1.n.x+mx)*brt*scale,(p1.n.y+my)*brt*scale,(p1.n.z+mz)*brt*scale);
  glVertex3f(p1.x,p1.y,p1.z);
  glNormal3f((p2.n.x+mx)*brt*scale,(p2.n.y+my)*brt*scale,(p2.n.z+mz)*brt*scale);
  glVertex3f(p2.x,p2.y,p2.z);
  glNormal3f((p3.n.x+mx)*brt*scale,(p3.n.y+my)*brt*scale,(p3.n.z+mz)*brt*scale);
  glVertex3f(p3.x,p3.y,p3.z);
  glNormal3f((p4.n.x+mx)*brt*scale,(p4.n.y+my)*brt*scale,(p4.n.z+mz)*brt*scale);
  glVertex3f(p4.x,p4.y,p4.z);
  glNormal3f((p5.n.x+mx)*brt*scale,(p5.n.y+my)*brt*scale,(p5.n.z+mz)*brt*scale);
  glVertex3f(p5.x,p5.y,p5.z);
  glNormal3f((p6.n.x+mx)*brt*scale,(p6.n.y+my)*brt*scale,(p6.n.z+mz)*brt*scale);
  glVertex3f(p6.x,p6.y,p6.z);
  glNormal3f((p7.n.x+mx)*brt*scale,(p7.n.y+my)*brt*scale,(p7.n.z+mz)*brt*scale);
  glVertex3f(p7.x,p7.y,p7.z);
  glNormal3f((p8.n.x+mx)*brt*scale,(p8.n.y+my)*brt*scale,(p8.n.z+mz)*brt*scale);
  glVertex3f(p8.x,p8.y,p8.z);
  glEnd();
}

void Viewer::polyWire(pt3f p1, pt3f p2, pt3f p3, pt3f p4, pt3f p5, pt3f p6, pt3f p7, pt3f p8){
  float mx = ((p1.n.x + p2.n.x + p3.n.x + p4.n.x + p5.n.x + p6.n.x + p7.n.x + p8.n.x)/8)*faceWeight;
  float my = ((p1.n.y + p2.n.y + p3.n.y + p4.n.y + p5.n.y + p6.n.y + p7.n.y + p8.n.y)/8)*faceWeight;
  float mz = ((p1.n.z + p2.n.z + p3.n.z + p4.n.z + p5.n.z + p6.n.z + p7.n.z + p8.n.z)/8)*faceWeight;
  glBegin(GL_LINE_LOOP);
  glNormal3f((p1.n.x+mx)*brt*scale,(p1.n.y+my)*brt*scale,(p1.n.z+mz)*brt*scale);
  glVertex3f(p1.x,p1.y,p1.z);
  glNormal3f((p2.n.x+mx)*brt*scale,(p2.n.y+my)*brt*scale,(p2.n.z+mz)*brt*scale);
  glVertex3f(p2.x,p2.y,p2.z);
  glNormal3f((p3.n.x+mx)*brt*scale,(p3.n.y+my)*brt*scale,(p3.n.z+mz)*brt*scale);
  glVertex3f(p3.x,p3.y,p3.z);
  glNormal3f((p4.n.x+mx)*brt*scale,(p4.n.y+my)*brt*scale,(p4.n.z+mz)*brt*scale);
  glVertex3f(p4.x,p4.y,p4.z);
  glNormal3f((p5.n.x+mx)*brt*scale,(p5.n.y+my)*brt*scale,(p5.n.z+mz)*brt*scale);
  glVertex3f(p5.x,p5.y,p5.z);
  glNormal3f((p6.n.x+mx)*brt*scale,(p6.n.y+my)*brt*scale,(p6.n.z+mz)*brt*scale);
  glVertex3f(p6.x,p6.y,p6.z);
  glNormal3f((p7.n.x+mx)*brt*scale,(p7.n.y+my)*brt*scale,(p7.n.z+mz)*brt*scale);
  glVertex3f(p7.x,p7.y,p7.z);
  glNormal3f((p8.n.x+mx)*brt*scale,(p8.n.y+my)*brt*scale,(p8.n.z+mz)*brt*scale);
  glVertex3f(p8.x,p8.y,p8.z);
  glEnd();
}

void Viewer::triFace(pt3f p1, pt3f p2, pt3f p3){
  float mx = ((p1.n.x + p2.n.x + p3.n.x)/3)*faceWeight;
  float my = ((p1.n.y + p2.n.y + p3.n.y)/3)*faceWeight;
  float mz = ((p1.n.z + p2.n.z + p3.n.z)/3)*faceWeight;
  glNormal3f((p1.n.x+mx)*brt*scale,(p1.n.y+my)*brt*scale,(p1.n.z+mz)*brt*scale);
  glVertex3f(p1.x,p1.y,p1.z);
  glNormal3f((p2.n.x+mx)*brt*scale,(p2.n.y+my)*brt*scale,(p2.n.z+mz)*brt*scale);
  glVertex3f(p2.x,p2.y,p2.z);
  glNormal3f((p3.n.x+mx)*brt*scale,(p3.n.y+my)*brt*scale,(p3.n.z+mz)*brt*scale);
  glVertex3f(p3.x,p3.y,p3.z);
}

void Viewer::triWire(pt3f p1, pt3f p2, pt3f p3){
  float mx = ((p1.n.x + p2.n.x + p3.n.x)/3)*faceWeight;
  float my = ((p1.n.y + p2.n.y + p3.n.y)/3)*faceWeight;
  float mz = ((p1.n.z + p2.n.z + p3.n.z)/3)*faceWeight;
  glBegin(GL_LINE_LOOP);
  glNormal3f((p1.n.x+mx)*brt*scale,(p1.n.y+my)*brt*scale,(p1.n.z+mz)*brt*scale);
  glVertex3f(p1.x,p1.y,p1.z);
  glNormal3f((p2.n.x+mx)*brt*scale,(p2.n.y+my)*brt*scale,(p2.n.z+mz)*brt*scale);
  glVertex3f(p2.x,p2.y,p2.z);
  glNormal3f((p3.n.x+mx)*brt*scale,(p3.n.y+my)*brt*scale,(p3.n.z+mz)*brt*scale);
  glVertex3f(p3.x,p3.y,p3.z);
  glEnd();
}

void Viewer::shine(pt3f p1, pt3f p2){
  glNormal3f(0,5,0);
  glColor4f(1,1,1,0);
  glVertex3f(p1.x,p1.y,p1.z);
  glVertex3f(p2.x,p2.y,p2.z);
  glColor4f(1,1,1,0.3);
  glVertex3f(p2.x,p2.y,p2.z+20);
  glVertex3f(p1.x,p1.y,p1.z+20);
  glVertex3f(p1.x,p1.y,p1.z+20);
  glVertex3f(p2.x,p2.y,p2.z+20);
  glColor4f(1,1,1,0);
  glVertex3f(p2.x,p2.y,p2.z+60);
  glVertex3f(p1.x,p1.y,p1.z+60);
}

void Viewer::unitShine(float x, float y, float z, Viewer::cubeType type){
  Viewer::pt3f btrz; btrz.x = x+edge; btrz.y = y+edge; btrz.z = z-edge+coff; btrz.n = nbtrz;
  Viewer::pt3f btlz; btlz.x = x-edge; btlz.y = y+edge; btlz.z = z-edge+coff; btlz.n = nbtlz;
  Viewer::pt3f bbrz; bbrz.x = x+edge; bbrz.y = y-edge; bbrz.z = z-edge+coff; bbrz.n = nbbrz;
  Viewer::pt3f bblz; bblz.x = x-edge; bblz.y = y-edge; bblz.z = z-edge+coff; bblz.n = nbblz;

  //send in naive normal and calculate weighted normal in the actual draw
  switch(type){
    case Viewer::wire:
    break;
    case Viewer::face:
      glBegin(GL_QUADS);
      shine(btlz,bblz);
      shine(btlz,btrz);
      shine(bbrz,btrz);
      shine(bbrz,bblz);
      glEnd();
    break;
    case Viewer::multi:
    break;
  }
}

void Viewer::unitCube(float x, float y, float z, Viewer::cubeType type){
  Viewer::pt3f ftrx; ftrx.x = x+edge-coff; ftrx.y = y+edge; ftrx.z = z+edge; ftrx.n = nftrx;
  Viewer::pt3f ftry; ftry.x = x+edge; ftry.y = y+edge-coff; ftry.z = z+edge; ftry.n = nftry;
  Viewer::pt3f ftrz; ftrz.x = x+edge; ftrz.y = y+edge; ftrz.z = z+edge-coff; ftrz.n = nftrz;
  Viewer::pt3f ftlx; ftlx.x = x-edge+coff; ftlx.y = y+edge; ftlx.z = z+edge; ftlx.n = nftlx;
  Viewer::pt3f ftly; ftly.x = x-edge; ftly.y = y+edge-coff; ftly.z = z+edge; ftly.n = nftly;
  Viewer::pt3f ftlz; ftlz.x = x-edge; ftlz.y = y+edge; ftlz.z = z+edge-coff; ftlz.n = nftlz;
  Viewer::pt3f fbrx; fbrx.x = x+edge-coff; fbrx.y = y-edge; fbrx.z = z+edge; fbrx.n = nfbrx;
  Viewer::pt3f fbry; fbry.x = x+edge; fbry.y = y-edge+coff; fbry.z = z+edge; fbry.n = nfbry;
  Viewer::pt3f fbrz; fbrz.x = x+edge; fbrz.y = y-edge; fbrz.z = z+edge-coff; fbrz.n = nfbrz;
  Viewer::pt3f fblx; fblx.x = x-edge+coff; fblx.y = y-edge; fblx.z = z+edge; fblx.n = nfblx;
  Viewer::pt3f fbly; fbly.x = x-edge; fbly.y = y-edge+coff; fbly.z = z+edge; fbly.n = nfbly;
  Viewer::pt3f fblz; fblz.x = x-edge; fblz.y = y-edge; fblz.z = z+edge-coff; fblz.n = nfblz;
  Viewer::pt3f btrx; btrx.x = x+edge-coff; btrx.y = y+edge; btrx.z = z-edge; btrx.n = nbtrx;
  Viewer::pt3f btry; btry.x = x+edge; btry.y = y+edge-coff; btry.z = z-edge; btry.n = nbtry;
  Viewer::pt3f btrz; btrz.x = x+edge; btrz.y = y+edge; btrz.z = z-edge+coff; btrz.n = nbtrz;
  Viewer::pt3f btlx; btlx.x = x-edge+coff; btlx.y = y+edge; btlx.z = z-edge; btlx.n = nbtlx;
  Viewer::pt3f btly; btly.x = x-edge; btly.y = y+edge-coff; btly.z = z-edge; btly.n = nbtly;
  Viewer::pt3f btlz; btlz.x = x-edge; btlz.y = y+edge; btlz.z = z-edge+coff; btlz.n = nbtlz;
  Viewer::pt3f bbrx; bbrx.x = x+edge-coff; bbrx.y = y-edge; bbrx.z = z-edge; bbrx.n = nbbrx;
  Viewer::pt3f bbry; bbry.x = x+edge; bbry.y = y-edge+coff; bbry.z = z-edge; bbry.n = nbbry;
  Viewer::pt3f bbrz; bbrz.x = x+edge; bbrz.y = y-edge; bbrz.z = z-edge+coff; bbrz.n = nbbrz;
  Viewer::pt3f bblx; bblx.x = x-edge+coff; bblx.y = y-edge; bblx.z = z-edge; bblx.n = nbblx;
  Viewer::pt3f bbly; bbly.x = x-edge; bbly.y = y-edge+coff; bbly.z = z-edge; bbly.n = nbbly;
  Viewer::pt3f bblz; bblz.x = x-edge; bblz.y = y-edge; bblz.z = z-edge+coff; bblz.n = nbblz;

  //send in naive normal and calculate weighted normal in the actual draw
  switch(type){
    case Viewer::wire:
      glLineWidth(1);
      polyWire(ftly,ftlx,ftrx,ftry,fbry,fbrx,fblx,fbly);
      polyWire(btly,btlx,btrx,btry,bbry,bbrx,bblx,bbly);
      polyWire(btlz,btlx,btrx,btrz,ftrz,ftrx,ftlx,ftlz);
      polyWire(bblz,bblx,bbrx,bbrz,fbrz,fbrx,fblx,fblz);
      polyWire(ftry,ftrz,btrz,btry,bbry,bbrz,fbrz,fbry);
      polyWire(ftly,ftlz,btlz,btly,bbly,bblz,fblz,fbly);

      triWire(ftrx,ftry,ftrz);
      triWire(ftlx,ftly,ftlz);
      triWire(fbrx,fbry,fbrz);
      triWire(fblx,fbly,fblz);
      triWire(btrx,btry,btrz);
      triWire(btlx,btly,btlz);
      triWire(bbrx,bbry,bbrz);
      triWire(bblx,bbly,bblz);
    break;
    case Viewer::face:
      polyFace(ftly,ftlx,ftrx,ftry,fbry,fbrx,fblx,fbly);
      polyFace(btly,btlx,btrx,btry,bbry,bbrx,bblx,bbly);
      polyFace(btlz,btlx,btrx,btrz,ftrz,ftrx,ftlx,ftlz);
      polyFace(bblz,bblx,bbrx,bbrz,fbrz,fbrx,fblx,fblz);
      polyFace(ftry,ftrz,btrz,btry,bbry,bbrz,fbrz,fbry);
      polyFace(ftly,ftlz,btlz,btly,bbly,bblz,fblz,fbly);

      glBegin(GL_TRIANGLES);
      triFace(ftrx,ftry,ftrz);
      triFace(ftlx,ftly,ftlz);
      triFace(fbrx,fbry,fbrz);
      triFace(fblx,fbly,fblz);
      triFace(btrx,btry,btrz);
      triFace(btlx,btly,btlz);
      triFace(bbrx,bbry,bbrz);
      triFace(bblx,bbly,bblz);
      glEnd();
    break;
    case Viewer::multi:
      glColor4f(mainColor,mainColor,subColor,1);
      polyFace(ftly,ftlx,ftrx,ftry,fbry,fbrx,fblx,fbly);
      glColor4f(mainColor,subColor,mainColor,1);
      polyFace(btly,btlx,btrx,btry,bbry,bbrx,bblx,bbly);
      glColor4f(mainColor,subColor,subColor,1);
      polyFace(btlz,btlx,btrx,btrz,ftrz,ftrx,ftlx,ftlz);
      glColor4f(subColor,mainColor,mainColor,1);
      polyFace(bblz,bblx,bbrx,bbrz,fbrz,fbrx,fblx,fblz);
      glColor4f(subColor,mainColor,subColor,1);
      polyFace(ftry,ftrz,btrz,btry,bbry,bbrz,fbrz,fbry);
      glColor4f(subColor,subColor,mainColor,1);
      polyFace(ftly,ftlz,btlz,btly,bbly,bblz,fblz,fbly);

      glBegin(GL_TRIANGLES);
      glColor4f(mainColor,mainColor,subColor,1);
      triFace(ftrx,ftry,ftrz);
      triFace(ftlx,ftly,ftlz);
      triFace(fbrx,fbry,fbrz);
      triFace(fblx,fbly,fblz);
      glColor4f(mainColor,subColor,mainColor,1);
      triFace(btrx,btry,btrz);
      triFace(btlx,btly,btlz);
      triFace(bbrx,bbry,bbrz);
      triFace(bblx,bbly,bblz);
      glEnd();
    break;
  }
}
