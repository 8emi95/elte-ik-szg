#version 130

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

//
// uniform változók
//

// színtér tulajdonságok
uniform vec3 eye_pos = vec3(0, 15, 15);

// fénytulajdonságok
//uniform vec3 light_pos = vec3( 5, 5, 5 );
uniform vec3 light_dir = vec3(0, -1, 0);

uniform vec4 La = vec4(0, 0.3, 0.4, 1);
uniform vec4 Ld = vec4(0.6, 0.6, 0.2, 1);
uniform vec4 Ls = vec4(1, 1, 1, 1);

// anyagtulajdonságok
uniform vec4 Ka = vec4(1, 1, 1, 0);
uniform vec4 Kd = vec4(0.75f, 0.25f, 0.125f, 1);
uniform vec4 Ks = vec4(0, 1, 0, 0);
uniform float specular_power = 32;
uniform sampler2D texImage;

void main()
{
	//
	// ambiens szín számítása
	//
	vec4 ambient = La * Ka;

	//
	// diffúz szín számítása
	vec3 normal = normalize( vs_out_normal );
	//vec3 toLight = normalize(light_pos - vs_out_pos); //PONT FENY
	//akarmilyen hosszu lehet a slider miatt
	vec3 toLight = normalize(-light_dir); //IRANY FENY
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );
	vec4 diffuse = vec4(Ld.rgb*Kd.rgb*di, Kd.a);

	//
	// fényfoltképzõ szín
	//
	vec4 specular = vec4(0);

	if ( di > 0 )
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		//mert a lightdir nem megbízható, lehet, hogy nagyobb, mint 1
		vec3 r = reflect( -toLight, normal );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );
		specular = Ls*Ks*si;
	}

	vec4 color = (ambient + diffuse + specular ) * texture(texImage, vs_out_tex0.st);
	//milyen messze vagyunk a kameratol - ezt keri a feladat, hogy ennek a fuggvenyeben valtozzon
	float distToCam = length(eye_pos - vs_out_pos);
	distToCam = distToCam / 50;
	fs_out_col = mix(color, La, distToCam);
}
