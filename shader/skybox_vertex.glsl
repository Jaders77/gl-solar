#version 450 core

uniform mat4 look_matrix;
uniform mat4 proj_matrix;

layout (location = 0) in vec4 position;

out VS_OUT
{
   vec3 tc;
} vs_out;

void main(void)
{
   vec4 pos_vs = look_matrix * position;
   vs_out.tc = normalize(position).xyz;
   gl_Position = proj_matrix * pos_vs;
}