#version 330 core
// bemeneti v�ltoz� - most a vertex shader-b�l (vagyis ottani out)
in vec4 vs_out_col;

out vec4 fs_out_col;

void main()
{

	fs_out_col = vs_out_col;
}