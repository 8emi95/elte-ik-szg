#version 130

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_col;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_col;

// shader külsõ paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 MVP;
uniform vec3 intp_end;
uniform float intp_val;

void main()
{
	gl_Position = MVP * vec4( vs_in_pos, 1 );
	vs_out_col = mix(vs_in_col, intp_end, intp_val);
}