
#ifndef liquid_h
#define liquid_h

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "clothMesh.h"
#include "collision/collisionObject.h"
#include "collision/particle.h"

using namespace CGL;
using namespace std;


struct LiquidParameters {
  LiquidParameters() {}
  LiquidParameters( double k_const,
                  double mu_const, double g_const, double wall_reflection)
      : k_const(k_const),
        mu_const(mu_const),
        g_const(g_const),
        wall_reflection(wall_reflection) {}
  ~LiquidParameters() {}

  //parameters
  double k_const;
  double mu_const;
    double g_const;
    double wall_reflection;
    
};

struct Liquid {
  Liquid() {}
  Liquid(int grid_len);
  ~Liquid();

  void buildGrid();
    void build_grid_original();
    void build_grid_centered();
    void build_grid_corner();
    vector<Particle *> * get_particle_list(Vector3D pos);

  void simulate(double frames_per_sec, double simulation_steps, LiquidParameters *lp,
                vector<Vector3D> external_accelerations,
                vector<CollisionObject *> *collision_objects);

  void reset();
    
    double W_poly6(double r, double h);
    double grad_W_spiky(double r, double h);
    double grad_2_W_viscosity(double r, double h);
    double grad_W_poly6(double r, double h);
    double grad_2_W_poly6(double r, double h);

  void build_spatial_map();
    void build_neighbor_map();
  float hash_position(Vector3D pos);

  // Cloth properties
    int grid_len;
    int num_particles;
    double BOX_X;
    double BOX_Y;
    double BOX_Z;
    double radius;
    double rest_density;
    double mass;
    double s;
    double sigma;
    double l;
    int start_formation;
    
    double H;

  // Cloth components
  vector<Particle> particles;

  // Spatial hashing
  unordered_map<float, vector<Particle *> *> map;
    unordered_map<Particle *, vector<Particle *> *> neighbor_map;
};


#endif /* liquid_h */
