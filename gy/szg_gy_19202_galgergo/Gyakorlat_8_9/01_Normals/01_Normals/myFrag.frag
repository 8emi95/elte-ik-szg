#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// irány fényforrás: fény iránya
uniform vec3 light_dir = vec3(-1,-1,-1);

// fénytulajdonságok: ambiens, diffúz, ...
uniform vec3 La = vec3(0.4, 0.4, 0.4);
uniform vec3 Ld = vec3(0.4, 0.6, 0.6);
uniform vec3 Ls = vec3(1.0, 1.0, 1.0);

// anyagtulajdonságok: ambiens, diffúz, ...
uniform vec3 Ka = vec3(0.2, 0.4, 0.6);
uniform vec3 Kd = vec3(1.0, 1.0, 1.0);
uniform vec3 Ks = vec3(1.0, 1.0, 1.0);

uniform vec3 eye_pos;

uniform sampler2D texImage;

void main()
{	
	//
	// ambiens szín számítása
	//

	vec3 ambient = La * Ka;

	//
	// diffúz szín számítása
	//	
	/* segítség:
	    - normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/

	// fragmenthez tartozó normális
	vec3 normal = normalize(vs_out_norm);
	// fragmentből a fényforrás felé mutató vektor
	vec3 toLight = -normalize(light_dir);
	// diffúz együttható
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );

	vec3 diffuse = di * Ld * Kd;

	//
	// fényfoltképzõ szín
	//
	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/

	// kamera felé mutató vektor
	vec3 e = normalize(eye_pos - vs_out_pos);
	// a felületről tökéletesen visszatükröződő fénysugár
	vec3 r = reflect(-toLight, normal);
	// spekuláris együttható
	float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), 32);

	vec3 specular = si * Ls * Ks;

	
	//
	// a fragment végsõ színének meghatározása
	//

	//fs_out_col = vec4(ambient + diffuse + specular, 1);

	// felületi normális
	//fs_out_col = vec4(vs_out_norm, 1);



	// textúrával
	vec4 textureColor = texture(texImage, vs_out_tex);
	// egyenlítő
	if(abs(vs_out_tex.y - 0.5) < 0.01){
		textureColor = vec4(1,0,0,1);
	}
	// jégsapka
	if(vs_out_tex.y > 1 - 0.2){
		textureColor = vec4(1,1,1,1);
	}

	fs_out_col = vec4(ambient + diffuse + specular, 1) * textureColor;
}

// Feladatok

// 1) Fényszámítás
// - ambiens
// - diffúz
// - spekuláris

// 2) Textúra
