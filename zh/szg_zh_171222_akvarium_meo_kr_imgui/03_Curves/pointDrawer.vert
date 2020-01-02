#version 130

in vec3 vs_in_pos;
in vec3 vs_in_normal;
in vec2 vs_in_tex0;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vs_in_pos, 1) ;
}