#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// irány fényforrás: fény iránya
uniform vec3 light_dir = vec3(-1,-1,-1);

// fénytulajdonságok: ambiens, diffúz, spekuláris
uniform vec3 La = vec3(0.1, 0.1, 0.1);
uniform vec3 Ld = vec3(0.4, 0.4, 0.4);
uniform vec3 Ls = vec3(1, 1, 1);

// anyagtulajdonságok: ambiens, diffúz, spekuláris
uniform vec3 Ka = vec3(0, 0.1, 0.6);
uniform vec3 Kd = vec3(0, 0.1, 0.6);
uniform vec3 Ks = vec3(0.9, 0.9, 0.9);

uniform vec3 eye_pos = vec3(0, 0, 10);


void main()
{	
	// ambiens
	vec3 ambient = La*Ka;

	// diffúz 
	vec3 normal = normalize(vs_out_norm);
	vec3 to_light = normalize(-light_dir);
	
	float cosa = clamp(dot(normal, to_light), 0, 1);
	vec3 diffuse = cosa*Ld*Kd;
	

	// spekuláris
	vec3 e = normalize( eye_pos - vs_out_pos );
	vec3 r = reflect( -to_light, normal );

	float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), 12);
	vec3 specular = Ls*Ks*si;

	// a fragment végső színe
	fs_out_col = vec4(ambient + diffuse + specular, 1);
}