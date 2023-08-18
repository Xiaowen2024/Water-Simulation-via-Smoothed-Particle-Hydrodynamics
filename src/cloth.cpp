#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
    double offset_x = width / (num_width_points - 1);
    double offset_y = height / (num_height_points - 1);
    
    //Create the masses at the correct positions and add them to the list
    for (int j = 0; j < num_height_points; j++) {
        for (int i = 0; i < num_width_points; i++) {
            Vector3D pos;
            if (orientation == HORIZONTAL) {
                pos = Vector3D(offset_x * i, 1.0, offset_y * j);
            }
            else {
                pos = Vector3D(offset_x * i, offset_y * j, ((double)(rand() % 2001 - 1000)) / 1000000.0);
            }
            
            PointMass m = PointMass(pos, false);
            point_masses.push_back(m);
        }
    }
    
    for (int j = 0; j < num_height_points; j++) {
        for (int i = 0; i < num_width_points; i++) {
            PointMass *m = &(point_masses[j * num_width_points + i]);
            
            //Create the structural springs
            if (i < num_width_points - 1) {
                PointMass *m_right = &(point_masses[j * num_width_points + (i + 1)]);
                Spring s_right = Spring(m, m_right, STRUCTURAL);
                springs.push_back(s_right);
            }
            if (j < num_height_points - 1) {
                PointMass *m_top = &(point_masses[(j + 1)* num_width_points + i]);
                Spring s_top = Spring(m, m_top, STRUCTURAL);
                springs.push_back(s_top);
            }
            
            //Create the shearing constraints
            if (i < num_width_points - 1 && j < num_height_points - 1) {
                PointMass *m_diag = &(point_masses[(j + 1)* num_width_points + (i + 1)]);
                Spring s_shear = Spring(m, m_diag, SHEARING);
                springs.push_back(s_shear);
            }
            if (i < num_width_points - 1 && j > 0) {
                PointMass *m_diag = &(point_masses[(j - 1)* num_width_points + (i + 1)]);
                Spring s_shear = Spring(m, m_diag, SHEARING);
                springs.push_back(s_shear);
            }
            
            //Create the bending constraints
            if (i + 2 < num_width_points) {
                PointMass *m_right2 = &(point_masses[j* num_width_points + i + 2]);
                Spring s_bend = Spring(m, m_right2, BENDING);
                springs.push_back(s_bend);
            }
            if (j + 2 < num_height_points) {
                PointMass *m_top2 = &(point_masses[(j + 2)* num_width_points + i]);
                Spring s_bend = Spring(m, m_top2, BENDING);
                springs.push_back(s_bend);
            }
            
        }
    }
    
    for (int j = 0; j < pinned.size(); j++) {
        for (int i: pinned[j]) {
            point_masses[j* num_width_points + i].pinned = true;
        }
    }
    
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.
    for (PointMass &m: point_masses) {
        m.forces = Vector3D();
        for (Vector3D accel: external_accelerations) {
            m.forces += mass * accel;
        }
    }
    
    
    for (Spring &s: springs) {
        if (!cp->enable_bending_constraints && s.spring_type == BENDING) {
            continue;
        }
        if (!cp->enable_shearing_constraints && s.spring_type == SHEARING) {
            continue;
        }
        if (!cp->enable_structural_constraints && s.spring_type == STRUCTURAL) {
            continue;
        }
        Vector3D dist = s.pm_b->position - s.pm_a->position;
        double deformation = dist.norm() - s.rest_length;
        
        Vector3D f_ab = cp->ks * dist.unit() * deformation;
        if (s.spring_type == BENDING) {
            f_ab *= 0.2;
        }
        
        
        s.pm_a->forces += f_ab;
        s.pm_b->forces -= f_ab;
        //std::cout << s.pm_a->forces<<"\n" << std::flush;
    }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
    for (PointMass &m: point_masses) {
        //std::cout << m.forces<<"\n" << std::flush;
        if (m.pinned) {
            continue;
        }
        Vector3D newPos = m.position + (1.0 - ((double)cp->damping) / 100.0) * (m.position - m.last_position) + m.forces / mass * delta_t * delta_t;
        m.last_position = m.position;
        m.position = newPos;
    }

  // TODO (Part 4): Handle self-collisions.
    build_spatial_map();
    for (PointMass &m: point_masses) {
        self_collide(m, simulation_steps);
    }

  // TODO (Part 3): Handle collisions with other primitives.
    for (PointMass &m: point_masses) {
        for (CollisionObject* obj: *collision_objects) {
            obj->collide(m);
        }
    }


  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
    for (Spring &s: springs) {
        PointMass &m2 = *s.pm_b;
        PointMass &m1 = *s.pm_a;
        Vector3D dist = m2.position - m1.position;
        if (dist.norm() > 1.1 * s.rest_length) {
            if (m2.pinned && m1.pinned) {
                continue;
            }
            else if (m1.pinned) {
                m2.position -= dist / dist.norm() * (dist.norm() - 1.1 * s.rest_length);
            }
            else if (m2.pinned) {
                m1.position += dist / dist.norm() * (dist.norm() - 1.1 * s.rest_length);
            }
            else {
                m1.position += dist / dist.norm() * (dist.norm() - 1.1 * s.rest_length) / 2.0;
                m2.position -= dist / dist.norm() * (dist.norm() - 1.1 * s.rest_length) / 2.0;
            }
        }
    }
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
    
    for (PointMass &m: point_masses) {
        float hash = hash_position(m.position);
        
        if (map.find(hash) == map.end()) {
            map[hash] = new vector<PointMass *>;
        }
        
        map[hash]->push_back(&m);
    }
    
    /*
    int total = 0;
    for (const auto &entry : map) {
        total += entry.second->size();
        std::cout << entry.first << " " << entry.second->size() << "\n" << std::flush;
    }
    std::cout << total <<" POINTS ARE IN THE SPATIAL MAP\n" << std::flush;
     */
    //std::cout <<" SPATIAL MAP BUILT\n" << std::flush;
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
    float hash = hash_position(pm.position);
    Vector3D totalCorrection = Vector3D();
    int count = 0;
    
    for (PointMass *m: *map[hash]) {
        if (m == &pm) {
            continue;
        }
        
        Vector3D distVect = m->position - pm.position;
        double distance = distVect.norm();
        
        if (distance < 2 * thickness) {
            totalCorrection += (distance - 2 * thickness) * distVect / distVect.norm();
            count++;
        }
    }
    
    if (count == 0) {
        return;
    }
    
    Vector3D correctionVec = totalCorrection / ((double) count) / simulation_steps;
    //std::cout << correctionVec << "\n" << std::flush;
    
    pm.position += correctionVec;

}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
    double w = 3.0 * width / num_width_points;
    double h = 3.0 * height / num_height_points;
    double t;
    if (w > h) {
        t = w;
    }
    else {
        t = h;
    }
    
    int x = (int)(pos.x / w);
    int y = (int)(pos.y / h);
    int z = (int)(pos.z / t);
    
  return 100.0 * x  + 10.0 * y + 1.0 * z;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
