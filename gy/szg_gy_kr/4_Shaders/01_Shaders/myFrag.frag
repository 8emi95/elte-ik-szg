#version 130

// bemeneti változók - most a vertex shader-bõl (vagyis ottani out)
in vec4 vs_out_col;	// bemeneti szín (a csúcspontokból interpolálva)
in vec4 vs_out_pos;

// kimeneti változó - a fragment színe
// a fragment-shader célja, hogy ezt meghatározza
//RGB-alfa, ezert vec4 a kimenet
out vec4 fs_out_col;



void main()
{

	float x = 10*vs_out_pos.x;
	float y = 10*vs_out_pos.y;

	// kör
	fs_out_col = (x*x + y*y < 1) ? vec4(1, 0, 0, 1) : vec4(0, 1, 0, 1);
	

	// implicit függvénybõl adódó minta
	// további szép példa: sin(x + 2*sin(y)) - cos(y + 3*cos(x))
	/*
	if(x*cos(y)+y*cos(x) < 0){
		fs_out_col = vec4(1 - vs_out_col.xyz, 1); // // a kapott szín "inverze"
	} else {
		fs_out_col = vs_out_col;
	}
	*/

	// minden fehér
	//				  R, G, B, A
	//fs_out_col = vec4(1, 1, 1, 1);
}