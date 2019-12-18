#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex0;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// irány fényforrás: fény iránya
uniform vec3 light_dir = vec3(-1,-1,-1);

// fénytulajdonságok: ambiens, diffúz, ...
uniform vec3 La = vec3(0.1, 0.1, 0.1);
uniform vec3 Ld = vec3(0.4, 0.4, 0.4);

// anyagtulajdonságok: ambiens, diffúz, ...
// Nincs szükség rájuk, elég a textúra
//uniform vec3 Ka = vec3(0.8, 0.9, 0.7);
//uniform vec3 Kd = vec3(0.8, 0.9, 0.7);

uniform sampler2D texImage;
uniform sampler2D texImage2;

uniform float t;

void main()
{
	//
	// ambiens szín számítása
	//
	vec3 ambient = La;

	//
	// diffúz szín számítása
	vec3 normal = normalize(vs_out_norm);
	vec3 to_light = normalize(-light_dir);
	
	float cosa = clamp(dot(normal, to_light), 0, 1);

	vec3 diffuse = cosa*Ld;
	
	// a fragment végsõ színének meghatározása
	//
	// FELADAT: váltás a két textúra között folyamatos átmenettel (pl. mix())

	vec4 texcolor = mix(texture(texImage, vs_out_tex0.xy), texture(texImage2, vs_out_tex0.xy), t);
	if(vs_out_tex0.y > 0.9)
		texcolor = vec4(1);

	if(abs(vs_out_tex0.y - 0.5) < 0.01)
		texcolor = vec4(1, 0, 0, 1);

	if(vs_out_tex0.x < 0.02)
		texcolor = vec4(0, 1, 0, 1);

	fs_out_col = 2*vec4(ambient + diffuse, 1) * texcolor;
}

// procedurális textúra...
