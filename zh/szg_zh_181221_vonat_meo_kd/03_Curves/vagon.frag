#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;

out vec4 fs_out_col;

void main()
{
	//fs_out_col = vec4(vs_out_pos, 1);
					// R G B A
					//sin(sin(16x) - y)  *  0.5
	fs_out_col = vec4(0.f, sin(sin(16.f * vs_out_pos.x) - vs_out_pos.y) * 0.5f, sin(sin(16.f * vs_out_pos.x) - vs_out_pos.y) * 0.5f, 1);

}