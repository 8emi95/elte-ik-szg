#version 330 core

// a pipeline-ban tovább adandó értékek
out vec3 color;

// shader külsõ paraméterei
uniform mat4 MVP;

vec3[] pos = vec3[6](
	vec3(0,0,0), vec3(1,0,0),
	vec3(0,0,0), vec3(0,1,0),
	vec3(0,0,0), vec3(0,0,1)
);

vec3[] col = vec3[3](
	vec3(1,0,0),
	vec3(0,1,0),
	vec3(0,0,1)
);

void main()
{
	gl_Position = MVP * vec4(pos[gl_VertexID], 1 );
	
	color = col[gl_VertexID / 2];
}