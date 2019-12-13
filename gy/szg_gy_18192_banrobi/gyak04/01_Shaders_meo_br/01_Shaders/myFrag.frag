#version 130

// bemeneti változó - most a vertex shader-bõl (vagyis ottani out)
in vec4 vs_out_col;
in vec2 vs_out_pos; // MEO ------------------------------------------------------------------------------------------------------------------------

// kimeneti változó - a fragment színe
out vec4 fs_out_col;

// MEO ------------------------------------------------------------------------------------------------------------------------

bool sziv(float x, float y){
	return pow(x*x + y*y - 1, 3)-x*x*y*y*y < 0;
}

vec2 negyzet(vec2 a){
	return vec2(a.x*a.x-a.y*a.y, 2*a.x*a.y);
}

// MEO ------------------------------------------------------------------------------------------------------------------------

void main()
{
	//				  R, G, B, A
	// színátmenet
	//fs_out_col = vs_out_col;

	//				  R, G, B, A
	// tiszta fehér
	//fs_out_col = vec4(1, 1, 1, 1);

	// MEO ------------------------------------------------------------------------------------------------------------------------

	// kör
	/*
	float x = vs_out_pos.x, y = vs_out_pos.y;
	float r = 0.5;
	if(abs(sqrt(x*x+y*y) - r) < 0.01){
		fs_out_col = vec4(1, 0, 0, 1);
	}else{
		//fs_out_col = vec4(252/255.0, 15/255.0, 192/255.0, 1);
		fs_out_col = vec4(0,0,0	, 1);
	}
	*/

	// sziv
	/*
	float x = vs_out_pos.x, y = vs_out_pos.y;
	x *= 2;
	y *= 2;
	if( sziv(x, y) || sziv(x + 1, y) ){
		fs_out_col = vec4(1, 0, 0, 1);
	}else{
		fs_out_col = vec4(0,0,0	, 1);
	}
	*/

	// Mandelbrot
	
	vec2 c = vs_out_pos*2;
	vec2 xn = c;
	for(int i = 0; i < 100; ++i){
		xn = negyzet(xn) + c;
		if( length(xn) > 2 ) break;
	}
	if(length(xn) > 2) {
		fs_out_col = vec4(1,0,0,1);
	}
	else{
		fs_out_col = vec4(0,0,0,1);
	}
	
	// MEO ------------------------------------------------------------------------------------------------------------------------

}	