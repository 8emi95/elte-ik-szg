#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

out vec4 fs_out_col;

uniform sampler2D tex;

void main()
{
	fs_out_col = vec4(1,1,1, 1)*texture(tex, vs_out_tex);
}