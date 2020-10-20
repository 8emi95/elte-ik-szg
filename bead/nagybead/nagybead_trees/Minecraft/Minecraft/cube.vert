#version 330 core

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_norm;
in vec2 vs_in_tex;
in vec2 vs_in_atlas_offset;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;
out vec3 vs_out_view_pos;

// shader külsõ paraméterei
uniform mat4 MVP;
uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 view;
uniform int atlas_row_count;


void main()
{
	gl_Position = MVP * vec4( vs_in_pos, 1 );

	vec4 worldPos = world * vec4( vs_in_pos, 1 );
	
	vs_out_pos = (world * vec4(vs_in_pos, 1)).xyz;
	vs_out_norm = (worldIT * vec4(vs_in_norm, 0)).xyz;
	
	vec2 offset = vs_in_atlas_offset / atlas_row_count;

	vs_out_tex  = (vs_in_tex / atlas_row_count) + offset;

	vs_out_view_pos = (inverse(view) * vec4(vec3(0.0), 1.0)).xyz - worldPos.xyz;
}