#version 330 core

// a pipeline-ban tovább adandó értékek
vec4 positions[6] = vec4[6](
	vec4(10, 0, 0, 1),
	vec4(0, 0, 0, 1),
	vec4( 0, 10, 0, 1),
	vec4(0, 0, 0, 1),
	vec4(0,  0, 10, 1),
	vec4(0, 0, 0, 1)
);

vec4 colors[3] = vec4[3](
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1)
);

// kimeneti változó: a csúcspont színe
out vec4 vs_out_col;


// shader külsõ paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * positions[gl_VertexID];
	vs_out_col	= colors[gl_VertexID/2];

}