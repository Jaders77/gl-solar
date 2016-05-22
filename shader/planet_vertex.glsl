#version 450 core
#define PRECISION 0.000001

uniform mat4 look_matrix;
uniform mat4 proj_matrix;
uniform mat4 planet_matrix;

layout (location = 0) in vec4 position;

out VS_OUT
{
   vec3 tc;
   float diffuse_color;
} vs_out;

void main(void)
{
   vec4 pos_planet = planet_matrix * vec4(0., 0., 0., 1.);
   vec4 pos_ls = planet_matrix * position;
   vec4 pos_vs = look_matrix * pos_ls;
   gl_Position = proj_matrix * pos_vs;
   vec4 normal = pos_ls - pos_planet;
   vs_out.tc = position.xyz;
   float is_sun = step(-PRECISION, pos_planet.x) * (1.0 - step(PRECISION, pos_planet.x)) * step(-PRECISION, pos_planet.y) * (1.0 - step(PRECISION, pos_planet.y)) * step(-PRECISION, pos_planet.z) * (1.0 - step(PRECISION, pos_planet.z));
   vs_out.diffuse_color = max(0., dot(normalize(-normal.xyz), normalize(pos_ls.xyz))) + is_sun;
}