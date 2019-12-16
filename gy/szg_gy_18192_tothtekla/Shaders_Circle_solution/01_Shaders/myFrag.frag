#version 130

// bemeneti v�ltoz� - most a vertex shader-b�l (vagyis ottani out)
in vec4 vs_out_col;
in vec4 vs_out_pos; 

// kimeneti v�ltoz� - a fragment sz�ne
out vec4 fs_out_col;

void main()
{
	float r = sqrt(vs_out_pos.x*vs_out_pos.x + vs_out_pos.y*vs_out_pos.y); // a koordin�ta t�vols�ga az orig�t�l
	if (r < 0.5 ) //k�rlap: kirajzoljuk a fragmentet lil�val, ha a koordin�ta t�vols�ga az orig�t�l a k�t megadott �rt�k k�z� esik
	//if (r < 0.5 && r > 0.45) //k�rvonal: kirajzoljuk a fragmentet lil�val, ha a koordin�ta t�vols�ga az orig�t�l a k�t megadott �rt�k k�z� esik
		fs_out_col = vec4(1, 0, 1, 1);
	else
		discard; //eldobjuk a fragmentet, nem rajzolunk ki semmit
}