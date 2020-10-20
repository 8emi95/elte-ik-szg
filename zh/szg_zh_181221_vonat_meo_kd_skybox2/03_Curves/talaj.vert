#version 130

// VBO-b�l �rkez� v�ltoz�k
in vec3 vs_in_pos;
in vec3 vs_in_normal;
in vec2 vs_in_tex0;

// a pipeline-ban tov�bb adand� �rt�kek
out vec3 vs_out_pos;
out vec3 vs_out_normal;
out vec2 vs_out_tex0;


// shader k�ls� param�terei - most a h�rom transzform�ci�s m�trixot k�l�n-k�l�n vessz�k �t
uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 MVP;

uniform vec3 temp;

void main()
{

	if((sqrt(pow(vs_in_pos.x, 2) + pow(vs_in_pos.z, 2)) < 8.2 && sqrt(pow(vs_in_pos.x, 2) + pow(vs_in_pos.z, 2)) > 7.8)
		|| (sqrt(pow(vs_in_pos.x, 2) + pow(vs_in_pos.z, 2)) < 9.2 && sqrt(pow(vs_in_pos.x, 2) + pow(vs_in_pos.z, 2)) > 8.8)){
		gl_Position = MVP * vec4( vs_in_pos.x, vs_in_pos.y + 1, vs_in_pos.z, 1 );
	}else{
		gl_Position = MVP * vec4( vs_in_pos, 1 );
	}

	

	vs_out_pos = (world * vec4( vs_in_pos, 1 )).xyz;
	vs_out_normal  = (worldIT * vec4(vs_in_normal, 0)).xyz;
	vs_out_tex0 = vs_in_tex0;
}