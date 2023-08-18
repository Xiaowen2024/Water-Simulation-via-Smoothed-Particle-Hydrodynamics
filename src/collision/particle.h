//

#ifndef particle_h
#define particle_h

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"

using namespace CGL;
using namespace std;

struct Particle {
public:
  Particle(const Vector3D &origin, double radius, int num_lat = 3, int num_lon = 3)
      : position(origin), old_position(origin), start_position(origin), radius(radius), radius2(radius * radius), velocity(Vector3D(0.0)), old_acceleration(Vector3D(0.0)), norm_color_gradient(0.0), draw(true),
         m_sphere_mesh(Misc::SphereMesh(num_lat, num_lon)) {}

  void render(GLShader &shader);

public:
  double radius;
  double radius2;
    bool draw;

    
    double pressure;
    double density;
    double norm_color_gradient;
    Vector3D velocity;
    Vector3D position;
    Vector3D forces;
    Vector3D old_position;
    Vector3D start_position;
    Vector3D old_acceleration;
  
  Misc::SphereMesh m_sphere_mesh;
};


#endif /* particle_h */
