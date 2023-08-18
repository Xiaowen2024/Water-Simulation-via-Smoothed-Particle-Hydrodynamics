#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
  // TODO (Part 3): Handle collisions with spheres.
    //Check collission
    double x = pm.position.x;
    double y = pm.position.y;
    double z = pm.position.z;
    double cx = this->origin.x;
    double cy = this->origin.y;
    double cz = this->origin.z;
    
    if ((x - cx)*(x - cx) + (y - cy)*(y - cy) + (z - cz)*(z - cz) > this->radius * this->radius) {
        return;
    }
    
    
    Vector3D pointOnSphere = this->origin + (pm.position - this->origin) / (pm.position - this->origin).norm() * this->radius;
    Vector3D correctionVec = pointOnSphere - pm.last_position;
    
    pm.position = pm.last_position + (1 - friction) * correctionVec;

}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}
