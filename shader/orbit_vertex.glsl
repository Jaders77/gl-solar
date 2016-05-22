#version 450 core

uniform mat4 look_matrix;
uniform mat4 proj_matrix;
uniform mat4 orbit_matrix;

layout (location = 0) in vec4 position;

void main(void)
{
   vec4 pos_ls = orbit_matrix * position;
   vec4 pos_vs = look_matrix * pos_ls;
   gl_Position = proj_matrix * pos_vs;
}