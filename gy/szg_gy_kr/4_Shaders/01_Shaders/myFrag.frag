#version 130

// bemeneti v�ltoz�k - most a vertex shader-b�l (vagyis ottani out)
in vec4 vs_out_col;	// bemeneti sz�n (a cs�cspontokb�l interpol�lva)
in vec4 vs_out_pos;

// kimeneti v�ltoz� - a fragment sz�ne
// a fragment-shader c�lja, hogy ezt meghat�rozza
//RGB-alfa, ezert vec4 a kimenet
out vec4 fs_out_col;



void main()
{

	float x = 10*vs_out_pos.x;
	float y = 10*vs_out_pos.y;

	// k�r
	fs_out_col = (x*x + y*y < 1) ? vec4(1, 0, 0, 1) : vec4(0, 1, 0, 1);
	

	// implicit f�ggv�nyb�l ad�d� minta
	// tov�bbi sz�p p�lda: sin(x + 2*sin(y)) - cos(y + 3*cos(x))
	/*
	if(x*cos(y)+y*cos(x) < 0){
		fs_out_col = vec4(1 - vs_out_col.xyz, 1); // // a kapott sz�n "inverze"
	} else {
		fs_out_col = vs_out_col;
	}
	*/

	// minden feh�r
	//				  R, G, B, A
	//fs_out_col = vec4(1, 1, 1, 1);
}