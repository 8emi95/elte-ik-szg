#version 330 core
// bemeneti változó - most a vertex shader-bõl (vagyis ottani out)
in vec4 vs_out_col;

out vec4 fs_out_col;

void main()
{

	fs_out_col = vs_out_col;
}