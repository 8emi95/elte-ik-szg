#version 130

// bemeneti változó - most a vertex shader-bõl (vagyis ottani out)
in vec4 vs_out_col;

// kimeneti változó - a fragment színe
out vec4 fs_out_col;
in vec2 vs_out_pos;

vec2 sq(vec2 a);

void main()
{
	//				  R, G, B, A
	fs_out_col = vs_out_col;

	//				  R, G, B, A
	//fs_out_col = vec4(1, 1, 1, 1);

	bool ff = false;
	float R = 0.5;
	float eps = 0.03;
	ff = abs(length(vs_out_pos)-R) < eps;

	ff ? fs_out_col = vec4(0,0,1,1) : fs_out_col = vec4(1,1,1,1);

	vec2 c = 2*vs_out_pos;
	vec2 xn = c;
	for(int i = 0; i < 100; ++i)
	{
		xn = sq(xn)+c;
	}

	ff = length(xn) < 2;
	fs_out_col = ff ? vec4(1,0,0,1) : vec4(1,1,1,1);

}

vec2 sq(vec2 a)
{
	return vec2(a.x*a.x-a.y*a.y,2*a.x*a.y);
}