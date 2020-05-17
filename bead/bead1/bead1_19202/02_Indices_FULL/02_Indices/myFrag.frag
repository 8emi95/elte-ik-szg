#version 130

in vec3 vs_out_col;
in vec3 vs_out_pos;
out vec4 fs_out_col;

uniform float t=1;
uniform vec3 target_col=vec3 (0,0,1);

void main()
{
	//fs_out_col = vec4(vs_out_col, 1);

	vec3 color = (1-t) * vs_out_col + t * target_col; // t = 0 eredeti sz�n, t = 1 �j sz�n
    fs_out_col = vec4(color, 1);

}

// 1. feladat: rajzoljuk ki feh�rrel a t�glalapot!

// 2. feladat: uniform v�ltoz�k - az alkalmaz�s �ll�tsa be, hogy milyen sz�nnel t�lts�nk ki!

// 3. feladat: rajzoljuk ki az orig� k�z�ppont�, 1 sugar� k�rt! Mit kell tenni, ha nem a
//    k�rlapot, hanem csak a k�rvonalat akarjuk? Eml.: discard() eldobja a fragmentet

// 4. feladat: komplex sz�mok....
