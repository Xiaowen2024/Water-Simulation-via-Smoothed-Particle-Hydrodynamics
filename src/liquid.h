
#ifndef liquid_h
#define liquid_h

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/vector3D.h"
#include "CGL/misc.h"
#include "CGL/CGL.h"
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
  double k_const; // pressure stiffness constant
  double mu_const; // viscosity constant
  double g_const; // gravity constant
  double wall_reflection; // bounce factor for collisions with walls (planes)
    
};

class Liquid {
public:
  Liquid() {}
  Liquid(int grid_len);
  ~Liquid();

  void buildGrid();
  void build_grid_original();
  void build_grid_centered();
  void build_grid_corner();
  const vector<Particle*>& get_particle_list(Vector3D pos);
  void simulate(double frames_per_sec, double simulation_steps, const LiquidParameters &lp,
                vector<Vector3D> external_accelerations,
                vector<CollisionObject *> &collision_objects);

  void reset();
    
  double W_poly6(double r, double h);
  double grad_W_spiky(double r, double h);
  double grad_2_W_viscosity(double r, double h);
  double grad_W_poly6(double r, double h);
  double grad_2_W_poly6(double r, double h);

  void build_spatial_map();
  void build_neighbor_map();
  void find_neighbors(const Particle &p, float r, vector<Particle *> &out);
  int64_t hash_position(const Vector3D &pos) const;

  int grid_len;
  int num_particles;
  double BOX_X;
  double BOX_Y;
  double BOX_Z;
  double radius;
  double rest_density;
  double mass;
  double s; // scaling factor for radius
  double sigma;
  double l; // threshold for surface tension forces
  int start_formation;
  double H; // kernel radius

  vector<Particle> particles;

  // Spatial hashing
  unordered_map<int64_t, vector<Particle*>> spatial_map;
  unordered_map<Particle*, vector<Particle*>> neighbor_map;
};


#endif /* liquid_h */
