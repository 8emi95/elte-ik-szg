#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// irány fényforrás: fény iránya
//initialized to default value, but comes from the c++ program
uniform vec3 light_pos = vec3(0,0,0);

// fénytulajdonságok: ambiens, diffúz
uniform vec3 La = vec3(0.8, 0.8, 0.8);
uniform vec3 Ld = vec3(0.4, 0.4, 0.4);

// anyagtulajdonságok: ambiens, diffúz
uniform vec3 Ka = vec3(0, 0.1, 0.6);
uniform vec3 Kd = vec3(0, 0.5, 3.0);

void main()
{	
	// ambiens szín számítása
	vec3 ambient = La*Ka;

	// diffúz szín számítása
	vec3 normal = normalize(vs_out_norm);
	vec3 to_light = normalize(light_pos-vs_out_pos);
	float intensity = 1.0/(length(vs_out_pos-light_pos)*length(vs_out_pos-light_pos));
		
	float cosa = clamp(dot(normal, to_light), 0, 1);
	vec3 diffuse = intensity*cosa*Ld*Kd;

	//
	// fényfoltképzõ szín
	//

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/

	// a fragment végsõ színének meghatározása
	//
	fs_out_col = vec4(ambient + diffuse, 1);
}

