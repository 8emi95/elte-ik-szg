#version 330 core

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_norm;
in vec2 vs_in_tex;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;

// shader külsõ paraméterei
uniform mat4 MVP;
uniform mat4 world;
uniform mat4 worldIT;
uniform float time;
uniform float m;
uniform float h;

void main()
{
	vec3 pos = vs_in_pos;
	if(!(pos.x == 0 || pos.x == m || pos.z == 0 || pos.z == -h)) {
		pos.y = pos.y + (sin(time / 1000 + pos.x) + cos(time / 1000+ pos.z)) * 0.3; 
		//pos.y = pos.y + sin(time / 1000); 	
	}
	gl_Position = MVP * vec4(pos, 1 );
	
	vs_out_pos = (world * vec4(pos, 1)).xyz;
	vs_out_norm = (worldIT * vec4(vs_in_norm, 0)).xyz;
	vs_out_tex = vs_in_tex;
}