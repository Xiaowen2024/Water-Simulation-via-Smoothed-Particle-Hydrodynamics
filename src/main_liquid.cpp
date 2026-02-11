#include <iostream>
#include <fstream>
#include <nanogui/nanogui.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include "misc/getopt.h"
#else
#include <getopt.h>
#include <unistd.h>
#endif
#include <unordered_set>
#include <stdlib.h>

#include "CGL/CGL.h"
#include "collision/plane.h"
#include "collision/sphere.h"
#include "liquid.h"
#include "liquidSimulator.h"
#include "json.hpp"
#include "misc/file_utils.h"

typedef uint32_t gid_t;

using namespace std;
using namespace nanogui;

using json = nlohmann::json;

#define msg(s) cerr << "[ClothSim] " << s << endl;

const string SPHERE = "sphere";
const string PLANE = "plane";
const string LIQUID = "liquid";

const unordered_set<string> VALID_KEYS = {SPHERE, PLANE, LIQUID};

LiquidSimulator *app = nullptr;
GLFWwindow *window = nullptr;
Screen *screen = nullptr;

void error_callback(int error, const char* description) {
  puts(description);
}

void createGLContexts() {
  if (!glfwInit()) {
    return;
  }

  glfwSetTime(0);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  window = glfwCreateWindow(800, 800, "Liquid Simulator", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Could not initialize GLAD!");
  }
  glGetError();

  glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  screen = new Screen();
  screen->initialize(window, true);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glfwSwapInterval(1);
  glfwSwapBuffers(window);
}

void setGLFWCallbacks() {
  glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
    if (!screen->cursorPosCallbackEvent(x, y)) {
      app->cursorPosCallbackEvent(x / screen->pixelRatio(),
                                  y / screen->pixelRatio());
    }
  });

  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *, int button, int action, int modifiers) {
        if (!screen->mouseButtonCallbackEvent(button, action, modifiers) ||
            action == GLFW_RELEASE) {
          app->mouseButtonCallbackEvent(button, action, modifiers);
        }
      });

  glfwSetKeyCallback(
      window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
        if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
          app->keyCallbackEvent(key, scancode, action, mods);
        }
      });

  glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback(window,
                      [](GLFWwindow *, int count, const char **filenames) {
                        screen->dropCallbackEvent(count, filenames);
                        app->dropCallbackEvent(count, filenames);
                      });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    if (!screen->scrollCallbackEvent(x, y)) {
      app->scrollCallbackEvent(x, y);
    }
  });

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *, int width, int height) {
                                   screen->resizeCallbackEvent(width, height);
                                   app->resizeCallbackEvent(width, height);
                                 });
}

void usageError(const char *binaryName) {
  printf("Usage: %s [options]\n", binaryName);
  printf("Required program options:\n");
  printf("  -f     <STRING>    Filename of scene\n");
  printf("  -r     <STRING>    Project root.\n");
  printf("                     Should contain \"shaders/Default.vert\".\n");
  printf("                     Automatically searched for by default.\n");
  printf("  -a     <INT>       Sphere vertices latitude direction.\n");
  printf("  -o     <INT>       Sphere vertices longitude direction.\n");
  printf("\n");
  exit(-1);
}

void incompleteObjectError(const char *object, const char *attribute) {
  cout << "Incomplete " << object << " definition, missing " << attribute << endl;
  exit(-1);
}

bool loadObjectsFromFile(string filename, Liquid *liquid, LiquidParameters *lp,
                         vector<CollisionObject *>* objects,
                         int sphere_num_lat, int sphere_num_lon) {
  ifstream i(filename);
  if (!i.good()) {
    return false;
  }
  json j;
  i >> j;

  bool found_liquid = false;

  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    string key = it.key();

    if (VALID_KEYS.find(key) == VALID_KEYS.end()) {
      cout << "Invalid scene object found: " << key << endl;
      exit(-1);
    }

    json object = it.value();

    if (key == LIQUID) {
      found_liquid = true;

      auto it_start_formation = object.find("start_formation");
      if (it_start_formation != object.end()) {
        liquid->start_formation = *it_start_formation;
      } else {
        incompleteObjectError("liquid", "start_formation");
      }

      auto it_l = object.find("l");
      if (it_l != object.end()) {
        liquid->l = *it_l;
      } else {
        incompleteObjectError("liquid", "l");
      }
      auto it_sigma = object.find("sigma");
      if (it_sigma != object.end()) {
        liquid->sigma = *it_sigma;
      } else {
        incompleteObjectError("liquid", "sigma");
      }
      auto it_s = object.find("s");
      if (it_s != object.end()) {
        liquid->s = *it_s;
      } else {
        incompleteObjectError("liquid", "s");
      }
      auto it_mass = object.find("mass");
      if (it_mass != object.end()) {
        liquid->mass = *it_mass;
      } else {
        incompleteObjectError("liquid", "mass");
      }
      auto it_rest = object.find("rest_density");
      if (it_rest != object.end()) {
        liquid->rest_density = *it_rest;
      } else {
        incompleteObjectError("liquid", "rest_density");
      }
      auto it_H = object.find("H");
      if (it_H != object.end()) {
        liquid->H = *it_H;
      } else {
        incompleteObjectError("liquid", "H");
      }
      auto it_rad = object.find("radius");
      if (it_rad != object.end()) {
        liquid->radius = *it_rad;
      } else {
        incompleteObjectError("liquid", "radius");
      }
      auto it_bx = object.find("BOX_X");
      if (it_bx != object.end()) {
        liquid->BOX_X = *it_bx;
      } else {
        incompleteObjectError("liquid", "BOX_X");
      }
      auto it_by = object.find("BOX_Y");
      if (it_by != object.end()) {
        liquid->BOX_Y = *it_by;
      } else {
        incompleteObjectError("liquid", "BOX_Y");
      }
      auto it_bz = object.find("BOX_Z");
      if (it_bz != object.end()) {
        liquid->BOX_Z = *it_bz;
      } else {
        incompleteObjectError("liquid", "BOX_X");
      }
      auto it_gl = object.find("grid_len");
      if (it_gl != object.end()) {
        liquid->grid_len = *it_gl;
      } else {
        incompleteObjectError("liquid", "grid_len");
      }
      auto it_k = object.find("k_const");
      if (it_k != object.end()) {
        lp->k_const = *it_k;
      } else {
        incompleteObjectError("liquid", "k_const");
      }
      auto it_g = object.find("g_const");
      if (it_g != object.end()) {
        lp->g_const = *it_g;
      } else {
        incompleteObjectError("liquid", "g_const");
      }
      auto it_mu = object.find("mu_const");
      if (it_mu != object.end()) {
        lp->mu_const = *it_mu;
      } else {
        incompleteObjectError("liquid", "mu_const");
      }
      auto it_wf = object.find("wall_reflection");
      if (it_wf != object.end()) {
        lp->wall_reflection = *it_wf;
      } else {
        incompleteObjectError("liquid", "wall_reflection");
      }
    } else if (key == SPHERE) {
      Vector3D origin;
      double radius, friction;

      auto it_origin = object.find("origin");
      if (it_origin != object.end()) {
        vector<double> vec_origin = *it_origin;
        origin = Vector3D(vec_origin[0], vec_origin[1], vec_origin[2]);
      } else {
        incompleteObjectError("sphere", "origin");
      }

      auto it_radius = object.find("radius");
      if (it_radius != object.end()) {
        radius = *it_radius;
      } else {
        incompleteObjectError("sphere", "radius");
      }

      auto it_friction = object.find("friction");
      if (it_friction != object.end()) {
        friction = *it_friction;
      } else {
        incompleteObjectError("sphere", "friction");
      }

      Sphere *s = new Sphere(origin, radius, friction, sphere_num_lat, sphere_num_lon);
      objects->push_back(s);
    } else if (key == PLANE) {
      Vector3D point, normal;
      double friction;

      auto it_point = object.find("point");
      if (it_point != object.end()) {
        vector<double> vec_point = *it_point;
        point = Vector3D(vec_point[0], vec_point[1], vec_point[2]);
      } else {
        incompleteObjectError("plane", "point");
      }

      auto it_normal = object.find("normal");
      if (it_normal != object.end()) {
        vector<double> vec_normal = *it_normal;
        normal = Vector3D(vec_normal[0], vec_normal[1], vec_normal[2]);
      } else {
        incompleteObjectError("plane", "normal");
      }

      auto it_friction = object.find("friction");
      if (it_friction != object.end()) {
        friction = *it_friction;
      } else {
        incompleteObjectError("plane", "friction");
      }

      Plane *p = new Plane(point, normal, friction);
      objects->push_back(p);
    }
  }

  i.close();

  return found_liquid;
}

bool is_valid_project_root(const std::string& search_path) {
  std::stringstream ss;
  ss << search_path;
  ss << "/";
  ss << "shaders/Default.vert";

  return FileUtils::file_exists(ss.str());
}

bool find_project_root(const std::vector<std::string>& search_paths, std::string& retval) {
  for (std::string search_path : search_paths) {
    if (is_valid_project_root(search_path)) {
      retval = search_path;
      return true;
    }
  }
  return false;
}

int main(int argc, char **argv) {
  std::vector<std::string> search_paths = {
    ".",
    "..",
    "../..",
    "../../.."
  };
  std::string project_root;
  bool found_project_root = find_project_root(search_paths, project_root);

  Liquid liquid;
  LiquidParameters lp;
  vector<CollisionObject *> objects;

  int c;

  int sphere_num_lat = 40;
  int sphere_num_lon = 40;

  std::string file_to_load_from;
  bool file_specified = false;

  while ((c = getopt (argc, argv, "f:r:a:o:")) != -1) {
    switch (c) {
      case 'f': {
        file_to_load_from = optarg;
        file_specified = true;
        break;
      }
      case 'r': {
        project_root = optarg;
        if (!is_valid_project_root(project_root)) {
          std::cout << "Warn: Could not find required file \"shaders/Default.vert\" in specified project root: " << project_root << std::endl;
        }
        found_project_root = true;
        break;
      }
      case 'a': {
        int arg_int = atoi(optarg);
        if (arg_int < 1) {
          arg_int = 1;
        }
        sphere_num_lat = arg_int;
        break;
      }
      case 'o': {
        int arg_int = atoi(optarg);
        if (arg_int < 1) {
          arg_int = 1;
        }
        sphere_num_lon = arg_int;
        break;
      }
      default: {
        usageError(argv[0]);
        break;
      }
    }
  }

  if (!found_project_root) {
    std::cout << "Error: Could not find required file \"shaders/Default.vert\" anywhere!" << std::endl;
    return -1;
  } else {
    std::cout << "Loading files starting from: " << project_root << std::endl;
  }

  if (!file_specified) {
    std::stringstream def_fname;
    def_fname << project_root;
    def_fname << "/scene/liquid.json";
    file_to_load_from = def_fname.str();
  }

  bool success = loadObjectsFromFile(file_to_load_from, &liquid, &lp, &objects, sphere_num_lat, sphere_num_lon);
  if (!success) {
    std::cout << "Warn: Unable to load liquid scene from file: " << file_to_load_from << std::endl;
    return -1;
  }

  glfwSetErrorCallback(error_callback);

  createGLContexts();

  liquid.buildGrid();

  app = new LiquidSimulator(project_root, screen);
  app->loadLiquid(&liquid);
  app->loadLiquidParameters(&lp);
  app->loadCollisionObjects(&objects);
  app->init();

  screen->setVisible(true);
  screen->performLayout();

  setGLFWCallbacks();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    app->drawContents();

    screen->drawContents();
    screen->drawWidgets();

    glfwSwapBuffers(window);

    if (!app->isAlive()) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  return 0;
}
