#version 130

// VBO-b�l �rkez� v�ltoz�k
in vec3 vs_in_pos;
in vec3 vs_in_col;

in vec2 vs_in_tex0;

// a pipeline-ban tov�bb adand� �rt�kek
out vec3 vs_out_col;
out vec2 vs_out_tex0;

// shader k�ls� param�terei - most a h�rom transzform�ci�s m�trixot k�l�n-k�l�n vessz�k �t
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

uniform float szorzo = 0;
uniform int szinID = 0;
vec3 ujszin = vec3(0,0,0);


void main()
{
	gl_Position = proj * view * world * vec4( vs_in_pos, 1 );
	vs_out_col  = vs_in_col;
	vs_out_tex0 = vs_in_tex0;
}