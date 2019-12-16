#version 130

// bemeneti változó - most a vertex shader-bõl (vagyis ottani out)
in vec4 vs_out_col;
in vec4 vs_out_pos; 

// kimeneti változó - a fragment színe
out vec4 fs_out_col;

void main()
{
	float r = sqrt(vs_out_pos.x*vs_out_pos.x + vs_out_pos.y*vs_out_pos.y); // a koordináta távolsága az origótól
	if (r < 0.5 ) //körlap: kirajzoljuk a fragmentet lilával, ha a koordináta távolsága az origótól a két megadott érték közé esik
	//if (r < 0.5 && r > 0.45) //körvonal: kirajzoljuk a fragmentet lilával, ha a koordináta távolsága az origótól a két megadott érték közé esik
		fs_out_col = vec4(1, 0, 1, 1);
	else
		discard; //eldobjuk a fragmentet, nem rajzolunk ki semmit
}