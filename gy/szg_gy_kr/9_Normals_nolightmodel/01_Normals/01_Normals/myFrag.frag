#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// irány fényforrás: fény iránya
uniform vec3 light_dir = vec3(-1,-1,-1);

// fénytulajdonságok: ambiens, diffúz, ...
uniform vec3 La = vec3(0.1, 0.1, 0.1);
uniform vec3 Ld = vec3(0.4, 0.4, 0.4);

// anyagtulajdonságok: ambiens, diffúz, ...
uniform vec3 Ka = vec3(0, 0.1, 0.6);
uniform vec3 Kd = vec3(0, 0.1, 0.6);

void main()
{	
	//
	// ambiens szín számítása
	//
	vec3 ambient = La*Ka;

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

	vec3 diffuse = cosa*Ld*Kd;
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
	fs_out_col = 2*vec4(ambient + diffuse, 1);
}

// Feladatok

// pont fényforrás

// spekuláris modell 
