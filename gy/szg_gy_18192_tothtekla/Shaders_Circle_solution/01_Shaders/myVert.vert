#version 130

// lokális változók: két tömb
vec4 positions[4] = vec4[4](
	vec4(-1, -1, 0, 1),
	vec4( 1, -1, 0, 1),
	vec4(-1,  1, 0, 1),
	vec4( 1,  1, 0, 1)
);

vec4 colors[4] = vec4[4](
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1),
	vec4(1, 1, 1, 1)
);

// kimeneti változó: a csúcspont színe
out vec4 vs_out_col;
out vec4 vs_out_pos; //pozíció átadása a fragment shaderbe

void main()
{
	// gl_VertexID: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_VertexID.xhtml
	vs_out_pos = positions[gl_VertexID];
	gl_Position = vs_out_pos;
	vs_out_col	= colors[gl_VertexID];
}