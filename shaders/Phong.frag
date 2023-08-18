#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // YOUR CODE HERE
    
    float ka = 0.1;
    vec3 Ia = vec3(1.0,1.0,1.0);
    float ks = 0.8;
    float p = 100.0;
    vec3 l = normalize(u_light_pos - v_position.xyz);
    vec3 v = normalize(u_cam_pos - v_position.xyz);
    vec3 h = (l + v) / length(l + v);

    vec3 I_by_r_squared = u_light_intensity / (length(u_light_pos - v_position.xyz) * length(u_light_pos - v_position.xyz));
    out_color = vec4(I_by_r_squared * max(0.0, dot(v_normal.xyz, l)) + ks * I_by_r_squared * pow(max(0.0, dot(v_normal.xyz, h)), p) +ka * Ia, 1.0);
    out_color = vec4(out_color.xyz * u_color.xyz, 1.0);
  // (Placeholder code. You will want to replace it.)
  //out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  //out_color.a = 1;
}

