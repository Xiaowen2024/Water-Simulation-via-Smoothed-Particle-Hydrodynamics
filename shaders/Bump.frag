#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  
  return length(texture(u_texture_2, uv).xyz);
}

void main() {
  // YOUR CODE HERE
  vec3 b = normalize(cross(v_normal.xyz, v_tangent.xyz));
  mat3 TBN = mat3(normalize(v_tangent.xyz), b, normalize(v_normal.xyz));
  vec2 first = vec2(v_uv.x + 1 / u_texture_2_size.x, v_uv.y);
  vec2 second = vec2(v_uv.x, v_uv.y + 1 / u_texture_2_size.y);
  float dU = (h(first) - h(v_uv)) * u_height_scaling * u_normal_scaling;
  float dV = (h(second) - h(v_uv)) * u_height_scaling * u_normal_scaling;
  vec3 n0 = vec3(-dU, -dV, 1.0);
  vec3 nd = normalize(TBN * n0);

  float ka = 0.1;
    vec3 Ia = vec3(1.0,1.0,1.0);
    float ks = 0.8;
    float p = 100.0;
    vec3 l = normalize(u_light_pos - v_position.xyz);
    vec3 v = normalize(u_cam_pos - v_position.xyz);
    vec3 h = (l + v) / length(l + v);

    vec3 I_by_r_squared = u_light_intensity / (length(u_light_pos - v_position.xyz) * length(u_light_pos - v_position.xyz));
    out_color = vec4(I_by_r_squared * max(0.0, dot(nd, l)) + ks * I_by_r_squared * pow(max(0.0, dot(nd, h)), p)+ ka * Ia, 1.0);
    out_color = vec4(out_color.xyz * u_color.xyz, 1.0);


  // (Placeholder code. You will want to replace it.)
  //out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  //out_color.a = 1;
}

