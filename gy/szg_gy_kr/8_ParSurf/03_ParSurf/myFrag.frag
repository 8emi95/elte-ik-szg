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

// 1. feladat: rajzoljuk ki fehérrel a téglalapot!

// 2. feladat: uniform változók - az alkalmazás állítsa be, hogy milyen színnel töltsünk ki!

// 3. feladat: rajzoljuk ki az origó középpontú, 1 sugarú kört! Mit kell tenni, ha nem a
//    körlapot, hanem csak a körvonalat akarjuk? Eml.: discard() eldobja a fragmentet

// 4. feladat: komplex számok....
