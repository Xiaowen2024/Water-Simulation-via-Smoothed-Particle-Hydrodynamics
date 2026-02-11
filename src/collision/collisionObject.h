#ifndef COLLISIONOBJECT
#define COLLISIONOBJECT

#include <nanogui/nanogui.h>

#include "CGL/vector3D.h"
#include "particle.h"

using namespace CGL;
using namespace std;
using namespace nanogui;

class CollisionObject {
public:
  virtual void render(GLShader &shader) = 0;
  virtual void collide(Particle &p) = 0;

private:
  double friction;
};

#endif /* COLLISIONOBJECT */
