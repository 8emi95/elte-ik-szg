#version 330 core

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 color;

out vec4 fs_out_col;

void main()
{
	fs_out_col = vec4(color, 1);
}