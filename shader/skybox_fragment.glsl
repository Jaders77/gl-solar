#version 450 core

layout (binding = 0) uniform samplerCube tex_cube;

in VS_OUT
{
   vec3 tc;
} fs_in;

out vec4 color;

void main(void)
{
   color = texture(tex_cube, fs_in.tc);
   //color = vec4((fs_in.tc + 1.0) / 2.0, 1.0);
}