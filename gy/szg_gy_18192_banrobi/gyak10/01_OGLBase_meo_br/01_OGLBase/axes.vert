// MEO ------------------------------------------------------------------------------------------------------------------------

#version 330 core

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_col;

// shader külsõ paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
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