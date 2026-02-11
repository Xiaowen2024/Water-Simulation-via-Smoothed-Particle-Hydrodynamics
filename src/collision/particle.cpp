//
//  particle.cpp
//  clothsim
//
//  Created by Nithin Tatikonda on 4/30/23.
//

#include <stdio.h>
#include <nanogui/nanogui.h>

#include "CGL/vector3D.h"
#include "../misc/sphere_drawing.h"
#include "particle.h"

using namespace nanogui;
using namespace CGL;

void Particle::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
    if (draw) {
        m_sphere_mesh.draw_sphere(shader, position, radius * 0.92);
    }
}
