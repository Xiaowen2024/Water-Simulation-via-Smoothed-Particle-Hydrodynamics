#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "liquid.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Liquid::Liquid(int grid_len) {
    this->grid_len = grid_len;
    this->H = 1.0;
  buildGrid();
}

Liquid::~Liquid() {
  particles.clear();
}

void Liquid::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
    //Create the masses at the correct positions and add them to the list
    if (this->start_formation == 0) {
        build_grid_original();
    }
    else if (this->start_formation == 1) {
        build_grid_centered();
    }
    else if (this->start_formation == 2) {
        build_grid_corner();
    }
}

const vector<Particle*>& Liquid::get_particle_list(Vector3D pos) {
    static const vector<Particle*> empty;
    int64_t hash = hash_position(pos);
    auto it = spatial_map.find(hash);
    if (it == spatial_map.end()) return empty;
    return it->second;
}

void Liquid::build_grid_centered() {
    double width = (double) BOX_X;
    double height = (double) BOX_Y;
    double length = (double) BOX_Z;
    Vector3D offset = Vector3D(1.0);
    for (int j = 0; j < grid_len; j++) {
        for (int i = 0; i < grid_len; i++) {
            //((double)(rand() % 2001 - 1000)) / 1000000.0
            for (int k = 0; k < grid_len; k++) {
                Vector3D pos;
                double rand1 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                double rand2 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                
                double x = width / 2 + ((double)(i) - ((double)grid_len) / 2) * 1.5 *this->H + rand1;
                double y = height / 2 + ((double)(j) - ((double)grid_len) / 2) *  1.5 * this->H;
                double z = length / 2 + ((double)(k) - ((double)grid_len) / 2) * 1.5 * this->H + rand2;
                pos = Vector3D(x,y,z);
                Particle p = Particle(pos, this->radius);
                particles.push_back(p);
            }
        }
    }
}

void Liquid::build_grid_corner() {
    double width = (double) BOX_X;
    double height = (double) BOX_Y;
    double length = (double) BOX_Z;
    double rad = this->s * pow(3 * this->mass / 4 / M_PI/ this->rest_density, 1.0/3.0);
    
    Vector3D offset = Vector3D(1.0);
    for (int j = 0; j < grid_len; j++) {
        for (int i = 0; i < grid_len; i++) {
            //((double)(rand() % 2001 - 1000)) / 1000000.0
            for (int k = 0; k < grid_len; k++) {
                Vector3D pos;
                double rand1 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                double rand2 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                
                double x = ((double)(i)) * 2.0 * rad + rad;
                double y = ((double)(j)) *  2.0 * rad + rad;
                double z = ((double)(k)) * 2.0 * rad + rad;
                pos = Vector3D(x,y,z);
                Particle p = Particle(pos, rad);
                particles.push_back(p);
            }
        }
    }
}

void Liquid::build_grid_original() {
    double width = (double) BOX_X;
    double height = (double) BOX_Y;
    double length = (double) BOX_Z;
    Vector3D offset = Vector3D(1.0);
    for (int j = 0; j < grid_len; j++) {
        for (int i = 0; i < grid_len; i++) {
            //((double)(rand() % 2001 - 1000)) / 1000000.0
            for (int k = 0; k < grid_len; k++) {
                double rand1 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                double rand2 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                double rand3 = ((double)(rand() % 2001 - 1000)) / 1000000.0;
                Vector3D pos;
                //pos = Vector3D(2 * radius * i + rand1, 2 * radius * j + rand2, 2 * radius * k + rand3);
                double x = width / 2 + ((double)(i) - ((double)grid_len) / 2) * 5 * radius + rand1;
                double y = height / 2 + ((double)(j) - ((double)grid_len) / 2) * 5 * radius + rand2 + height/2;
                double z = length / 2 + ((double)(k) - ((double)grid_len) / 2) * 5 * radius + rand3;
                pos = Vector3D(x,y,z);
                Particle p = Particle(pos, this->radius);
                particles.push_back(p);
            }
        }
    }
}

void Liquid::build_spatial_map() {
    spatial_map.clear();
    spatial_map.reserve(particles.size());

    for (Particle &p: particles) {
        int64_t hash = hash_position(p.position);
        spatial_map[hash].push_back(&p);
    }
}

void Liquid::find_neighbors(const Particle &p, float r, vector<Particle *> &out) {
    out.clear();

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                Vector3D pos = p.position + Vector3D(dx, dy, dz) * this->H;
                int64_t hash = hash_position(pos);
                auto it = spatial_map.find(hash);
                if (it == spatial_map.end()) {
                    continue;
                }
                for (Particle *neighbor_p: it->second) {
                    if ((neighbor_p->position - p.position).norm() <= r) {
                        out.push_back(neighbor_p);
                    }
                }
            }
        }
    }
}

double Liquid::W_poly6(double r, double h) {
    // r is the distance between the two particles, h is the kernel radius
    if (r > h) {
        return 0.0;
    }
    return 315.0 / 64.0 / M_PI / pow(h, 9.0) * pow((h * h - r * r), 3.0);
}

double Liquid::grad_W_poly6(double r, double h) {
    // r is the distance between the two particles, h is the kernel radius
    if (r > h) {
        return 0.0;
    }

    return -945.0 / 32.0 / M_PI / pow(h, 9.0) * pow((h * h - r * r), 2.0);
}

double Liquid::grad_2_W_poly6(double r, double h) {
    // r is the distance between the two particles, h is the kernel radius
    if (r > h) {
        return 0.0;
    }

    return -945.0 / 32.0 / M_PI / pow(h, 9.0) * (h * h - r * r) * (3.0 * h*h - 7.0 * r * r);
}



double Liquid::grad_W_spiky(double r, double h) {
    if (r > h) {
        return 0.0;
    }
    return -45.0 / M_PI / pow(h, 6.0) * pow(h - r, 2.0);
}


double Liquid::grad_2_W_viscosity(double r, double h) {
    if (r > h) {
        return 0.0;
    }
    return 45.0 / M_PI / pow(h, 6.0) * (h - r);
}

void Liquid::simulate(double frames_per_sec, double simulation_steps, const LiquidParameters &lp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> &collision_objects) {
  double delta_t = 1.0f / frames_per_sec / simulation_steps;
    double rest_density = this->rest_density;
    double wall_offset = 0.0001;
    int count = 0;
    
    
    build_spatial_map();
    
    //Compute density and pressure
    for (Particle &p: particles) {
        double density = 0.0;

        for (Particle *neighbor_p: neighbor_map[&p]) {

            double r_val = (neighbor_p->position - p.position).norm();
            if (r_val > this->H) {
                continue;
            }
            
            density += this->mass * W_poly6(r_val, this->H);
            
        }
        

        p.density = density;
        p.pressure = lp.k_const * (density - rest_density);
    }
    
    
    //Compute forces
    for (Particle &p: particles) {
        //External_forces
        Vector3D total_external_forces = Vector3D(0.0);
        
        for (Vector3D acceleration: external_accelerations) {
            total_external_forces += acceleration * p.density;
        }
        
        //Pressure forces and Viscosity forces
        Vector3D total_pressure_forces = Vector3D(0.0);
        Vector3D total_viscosity_forces = Vector3D(0.0);
        
        for (Particle *neighbor_p: neighbor_map[&p]) {
            double r_val = (neighbor_p->position - p.position).norm();
            
            Vector3D diff_vec = p.position - neighbor_p->position;

            if (r_val == 0 || r_val > this->H) {
                continue;
            }
            total_pressure_forces += -grad_W_spiky(r_val, this->H) * this->mass * (p.pressure + neighbor_p->pressure) / 2 / neighbor_p->density * (diff_vec) / diff_vec.norm();

            diff_vec = neighbor_p->velocity - p.velocity;
            total_viscosity_forces += grad_2_W_viscosity(r_val, this->H) * this->mass / neighbor_p->density * lp.mu_const * diff_vec;
        }
        
        //Surface tension forces
        
        Vector3D total_surface_tension_forces = Vector3D(0.0);
        double c = 0.0;
        Vector3D n = Vector3D(0.0);
        double grad_2_c = 0.0;
        for (Particle *neighbor_p: neighbor_map[&p]) {
            double r_val = (neighbor_p->position - p.position).norm();
            Vector3D diff_vec = p.position - neighbor_p->position;
            
            if (r_val == 0.0 || r_val > this->H) {
                continue;
            }
            
            c += this->mass / neighbor_p->density * W_poly6(r_val, this->H);
            n += this->mass / neighbor_p->density * grad_W_poly6(r_val, this->H) * diff_vec;
            grad_2_c += this->mass / neighbor_p->density * grad_2_W_poly6(r_val, this->H);
        }
        if (n.norm() >= l) {
            total_surface_tension_forces = -sigma * n / n.norm() * grad_2_c;
            p.norm_color_gradient = n.norm();
        }
        else {
            p.norm_color_gradient = 0.0;
        }
        
        p.forces = total_external_forces + total_pressure_forces + total_viscosity_forces + total_surface_tension_forces;
        
        //std::cout <<  total_pressure_forces << " " << total_viscosity_forces << " \n" << std::flush;
        //std::cout << total_external_forces << " " << total_pressure_forces.norm() << " " << total_viscosity_forces  << " " << total_surface_tension_forces << " " << p.forces << p.forces / p.density << " \n";
        
    }
    //std::cout << "------------------------------------------------------------------\n";
    
    //compute new positions and velocities
    for (Particle &p: particles) {
        
        //p.position += delta_t * p.velocity; + 1.0/2.0 * p.old_acceleration * delta_t * delta_t;
        //p.velocity += delta_t  * ((p.forces / p.density) + p.old_acceleration) / 2.0;
        
        p.velocity += delta_t * (p.forces / p.density);
        p.position += delta_t * p.velocity;
        
        p.old_acceleration = (p.forces / p.density);
        
        if (p.position.x < 0) {
            p.position.x = 0;
            p.velocity.x *= lp.wall_reflection;
        }
        if (p.position.y < 0) {
            p.position.y = 0;
            p.velocity.y *= lp.wall_reflection;
        }
        if (p.position.z < 0) {
            p.position.z = 0;
            p.velocity.z *= lp.wall_reflection;
        }
        
        if (p.position.x > this->BOX_X) {
            p.position.x = this->BOX_X;
            p.velocity.x *= lp.wall_reflection;
        }/*
        if (p.position.y > this->BOX_Y) {
            p.position.y = this->BOX_Y;
            p.velocity.y *= lp.wall_reflection;
        }*/
        if (p.position.z > this->BOX_Z) {
            p.position.z = this->BOX_Z;
            p.velocity.z *= lp.wall_reflection;
        }
        
        
        
        p.radius = s * pow(3 * this->mass / 4 / M_PI/ p.density, 1.0/3.0);
        p.radius2 = p.radius * p.radius; 
        
        /*
        if (p.norm_color_gradient > this->l) {
            p.radius = 4 * s * pow(3 * this->mass / 4 / M_PI/ p.density, 1.0/3.0);
            p.radius2 = p.radius * p.radius;
            p.draw = false;
            count++;
        }
        else {
            p.draw = true;
        }
         */
         
    }
    
    
    /*
    build_spatial_map();
    build_neighbor_map();
    for (Particle &p: particles) {
        //std::cout << neighbor_map[&p]->size() << "\n";
        if (neighbor_map[&p]->size() < 0) {
            p.draw = false;
        }
        else {
            p.draw = true;
        }
    }
     */
    
    //std::cout <<"--------\n"<< std::flush;
}

int64_t Liquid::hash_position(const Vector3D &pos) const {
    int x = (int)floor(pos.x / this->H);
    int y = (int)floor(pos.y / this->H);
    int z = (int)floor(pos.z / this->H);
    
    return (int64_t)73856093 * x ^ (int64_t)19349663 * y ^ (int64_t)83492791 * z;
}
