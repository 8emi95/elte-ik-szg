#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec2 vs_out_tex0;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

uniform sampler2D texImage;
uniform sampler2D texImage2;

uniform float t;
uniform int kockaE;

void main()
{
	vec4 fs_out_col1 = texture(texImage, vs_out_tex0*2);
	vec4 fs_out_col2 = texture(texImage2, vs_out_tex0*2);

	fs_out_col = (1-t)*fs_out_col1+t*fs_out_col2;

	if(length(vs_out_tex0 -vec2(0.5)) > 0.5)
	{
		fs_out_col = vec4(0,0,1,1);
	}

	if(abs(vs_out_tex0.x - 0.5) < 0.2 && abs(vs_out_tex0.y - 0.5) < 0.2)
	{
		fs_out_col = vec4(1,0,0,1);
	}

	if(abs(vs_out_tex0.x - 0.5) < 0.05)
	{
		fs_out_col = vec4(0,1,0,1);
	}

	if(abs(vs_out_tex0.y - 0.5) < 0.05)
	{
		fs_out_col = vec4(0,1,0,1);
	}
}

// procedurális textúra...
