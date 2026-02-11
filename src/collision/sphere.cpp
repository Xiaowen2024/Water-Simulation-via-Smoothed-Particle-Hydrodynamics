#include <nanogui/nanogui.h>

#include "CGL/vector3D.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(Particle &p) {
  double x = p.position.x;
  double y = p.position.y;
  double z = p.position.z;
  double cx = this->origin.x;
  double cy = this->origin.y;
  double cz = this->origin.z;

  if ((x - cx) * (x - cx) + (y - cy) * (y - cy) + (z - cz) * (z - cz) >
      this->radius * this->radius) {
    return;
  }

  Vector3D pointOnSphere = this->origin +
                           (p.position - this->origin) /
                               (p.position - this->origin).norm() * this->radius;
  Vector3D correctionVec = pointOnSphere - p.old_position;

  p.position = p.old_position + (1 - friction) * correctionVec;
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}
