#version 130

in vec3 vs_out_col;
in vec3 vs_out_pos;
out vec4 fs_out_col;

void main()
{
	float x = vs_out_pos.x;
	float y = vs_out_pos.y;
	fs_out_col = vec4(0, sin((16*x) - y)*0.5, sin((16*x)-y)*0.5, 1);
	
}

// 1. feladat: rajzoljuk ki feh�rrel a t�glalapot!

// 2. feladat: uniform v�ltoz�k - az alkalmaz�s �ll�tsa be, hogy milyen sz�nnel t�lts�nk ki!

// 3. feladat: rajzoljuk ki az orig� k�z�ppont�, 1 sugar� k�rt! Mit kell tenni, ha nem a
//    k�rlapot, hanem csak a k�rvonalat akarjuk? Eml.: discard() eldobja a fragmentet

// 4. feladat: komplex sz�mok....
