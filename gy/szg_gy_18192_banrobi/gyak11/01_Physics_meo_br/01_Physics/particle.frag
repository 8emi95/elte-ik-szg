#version 130

uniform vec4 color = vec4(1);

in vec3 vs_out_vel;

out vec4 fs_out_color;

void main()
{
	fs_out_color = vec4(vs_out_vel/2+.5, 1);
}

// 1. feladat: színezd a részecskéket a sebességvektoruk nagysága szerint!
// 2. feladat: kódold el a színben a sebességvektor X, Y és Z tengely szerinti nagyságát!