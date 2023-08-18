# Water Simulation via Smoothed-Particle Hydrodynamics
Nithin Tatikonda, Shreyas Patel, Xiaowen Yuan, Samantha Huang

## Abstract

Our final project was a simulation of liquids done using Smoothed Particle Hydrodynamics. SPH uses particles as discrete representations of regions of a fluid, and uses these to store properties and make it easier to demonstrate the changes and forces that regions experience. For water, the particles tracked the density and pressure of their region. The forces that were exerted on them were pressure forces, viscosity, and surface tension. After tweaking the parameters of the system, the particles were able to accurately replicate the behavior of water.

## Technical Approach

### Smoothed Particle Hydrodynamics

SPH is a computational method that can be used to simulate any continuous media, liquids being one such example. A decomposition of the method’s name gives a great breakdown on how it works. The fluid that is being simulated is represented by a discrete set of particles, where each particle in the fluid represents a specific volume and stores its properties. For liquids in particular, these properties are the density and pressure of the region, as well as the net force that is being applied. These properties have to be recalculated with each time step to reflect any changes that the region experiences.

The term “smoothed” in SPH comes from how all of these properties are calculated. Properties of a particular particle are determined by the properties of other particles nearby, but particles further away contribute less then ones that are close by. To help determine the contribution of each neighboring particle, a smoothing kernel is used. Our method of calculation ends up being defined as such:


### Neighbor Search

A lot of calculation in this simulation relies on searching for nearby particles. To make this portion of the simulation more efficient, the positions of the particles are hashed and mapped. This way, particles can be easily searched for by hashing a position and using the map to store particles that are all in the same 3D region. Our method of hashing was spatial hashing.

### Updating Positions

To update the positions of the particles in our system we used Sympelectic Euler integration. For this, the first step is to determine the net acceleration for the particle in the current timestep. All of the forces were calculated in terms of force per unit volume and need to be simply divided by density.

## Result 
With our implementation, we were able to render our fluid in Blender via object files and in real time through our Project 4 Renderer.

Below is a video of us interacting with the particles in real time by adjusting gravity.
