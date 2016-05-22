#version 450 core

layout (binding = 0) uniform sampler2D tex;

in VS_OUT
{
   vec3 tc;
   float diffuse_color;
} fs_in;

out vec4 color;

const float PI = 3.141592653589793238462643383;

void main(void)
{
   vec3 tc_norm = normalize(fs_in.tc);
   vec2 lon_lat = vec2(1.0 - (atan(tc_norm.z, tc_norm.x) / PI + 1.0) * 0.5, (tc_norm.y + 1.0) * 0.5);
   //color = texture(tex, lon_lat) * vec4(fs_in.diffuse_color, fs_in.diffuse_color, fs_in.diffuse_color, 1.0);
   color = texture(tex, lon_lat);
   //color = vec4(fs_in.diffuse_color, fs_in.diffuse_color, fs_in.diffuse_color, 1.0);
}