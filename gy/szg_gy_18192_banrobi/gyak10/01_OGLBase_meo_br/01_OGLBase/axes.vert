// MEO ------------------------------------------------------------------------------------------------------------------------

#version 330 core

// a pipeline-ban tov�bb adand� �rt�kek
out vec3 vs_out_col;

// shader k�ls� param�terei - most a h�rom transzform�ci�s m�trixot k�l�n-k�l�n vessz�k �t
uniform mat4 MVP;

vec3 positions[6] = vec3[6](
	vec3(0,0,0), vec3(1,0,0),
	vec3(0,0,0), vec3(0,1,0),
	vec3(0,0,0), vec3(0,0,1)
);

vec3 colors[3] = vec3[3](
	vec3(1,0,0),
	vec3(0,1,0),
	vec3(0,0,1)
);


void main()
{
	vec3 pos = positions[gl_VertexID];
	vec3 col = colors[gl_VertexID/2];

	gl_Position = MVP * vec4( pos, 1 );

	vs_out_col = col;
}