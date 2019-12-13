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
uniform sampler2D texImage2; // MEO ------------------------------------------------------------------------------------------------------------------------

uniform float ice_level; // MEO ------------------------------------------------------------------------------------------------------------------------
uniform float mix_texture; // MEO ------------------------------------------------------------------------------------------------------------------------

void main()
{
	//
	// ambiens szín számítása
	//
	vec3 ambient = La;

	//
	// diffúz szín számítása
	//	
	/* segítség:
	    - normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	vec3 normal = normalize(vs_out_norm);
	vec3 to_light = normalize(-light_dir);
	
	float cosa = clamp(dot(normal, to_light), 0, 1);

	vec3 diffuse = cosa*Ld;
	//
	// fényfoltképzõ szín
	//

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	//...

	//
	// a fragment végsõ színének meghatározása
	//
	// FELADAT: váltás a két textúra között folyamatos átmenettel (pl. mix())
	
	/*
	fs_out_col = vec4(ambient + diffuse, 1) * texture(texImage, vs_out_tex0);
	*/

	// MEO ------------------------------------------------------------------------------------------------------------------------

	vec4 tex_color = mix(texture2D(texImage, vs_out_tex0), texture2D(texImage2, vs_out_tex0), mix_texture);
	
	if(vs_out_tex0.y > ice_level)
		tex_color = vec4(1,1,1,1);

	fs_out_col = vec4(ambient + diffuse, 1) * tex_color;

	// MEO ------------------------------------------------------------------------------------------------------------------------
}

// procedurális textúra...
