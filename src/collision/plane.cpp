#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "plane.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Plane::collide(PointMass &pm) {
  // TODO (Part 3): Handle collisions with planes.
    //Check if last position and current position are on different sides
    if (dot((pm.position - this->point), this->normal) < 0 && dot((pm.last_position - this->point), this->normal) < 0) {
        return;
    }
    if (dot((pm.position - this->point), this->normal) > 0 && dot((pm.last_position - this->point), this->normal) > 0) {
        return;
    }
    
    Vector3D d = this->normal;
    Vector3D pointOfIntersect = dot((this->point - pm.position), this->normal) / dot(d, this->normal) * d + pm.position;
    if (dot(this->normal, (pm.last_position - this->point)) >= 0) {
        pointOfIntersect += (this->normal / this->normal.norm()) * SURFACE_OFFSET;
    }
    else {
        pointOfIntersect -= (this->normal / this->normal.norm()) * SURFACE_OFFSET;
    }
    
    Vector3D correctionVec = pointOfIntersect - pm.last_position;
    
    pm.position = pm.last_position + correctionVec * (1 - friction);

}

void Plane::render(GLShader &shader) {
  nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);

  Vector3f sPoint(point.x, point.y, point.z);
  Vector3f sNormal(normal.x, normal.y, normal.z);
  Vector3f sParallel(normal.y - normal.z, normal.z - normal.x,
                     normal.x - normal.y);
  sParallel.normalize();
  Vector3f sCross = sNormal.cross(sParallel);

  MatrixXf positions(3, 4);
  MatrixXf normals(3, 4);

  positions.col(0) << sPoint + 2 * (sCross + sParallel);
  positions.col(1) << sPoint + 2 * (sCross - sParallel);
  positions.col(2) << sPoint + 2 * (-sCross + sParallel);
  positions.col(3) << sPoint + 2 * (-sCross - sParallel);

  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;

  if (shader.uniform("u_color", false) != -1) {
    shader.setUniform("u_color", color);
  }
  shader.uploadAttrib("in_position", positions);
  if (shader.attrib("in_normal", false) != -1) {
    shader.uploadAttrib("in_normal", normals);
  }

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
}
