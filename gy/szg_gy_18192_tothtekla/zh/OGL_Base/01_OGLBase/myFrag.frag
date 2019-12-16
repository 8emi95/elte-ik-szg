#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

out vec4 fs_out_col;

// színtér tulajdonságok
uniform vec3 lightPos = vec3(15, 1, 0);
//uniform vec3 lightDir = vec3(0, -1, 0);
uniform vec3 eye;

uniform float lightRadius = 20;

// fénytulajdonságok
uniform vec3 Fa = vec3(0.1, 0.1, 0.1);
uniform vec3 Fd = vec3(0.5, 0.8, 0.5);
uniform vec3 Fs = vec3(0.8, 0.8, 0.8);

// anyagtulajdonságok
uniform vec3 Ka = vec3(1, 1, 1);
uniform vec3 Kd = vec3(1, 1, 1);
uniform vec3 Ks = vec3(1, 1, 1);

uniform sampler2D myTexture;

void main()
{
	//
	// ambiens szín számítása
	//

	vec3 a = Fa*Ka;

	//
	// diffúz szín számítása
	//

	/* segítség:
		- normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	
	vec3 lightDir = normalize(vs_out_pos-lightPos);
	vec3 d = clamp(dot(vs_out_norm, -lightDir), 0, 1)*Fd*Kd;

	//
	// fényfoltképzõ szín
	//

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	vec3 r = reflect(lightDir, vs_out_norm);
	vec3 e = normalize(vs_out_pos - eye);

	vec3 s = pow(clamp(dot(r, -e), 0, 1), 6)*Fs*Ks;
	
	//
	//float intensity = pow(1-clamp(distance(lightPos, vs_out_pos), 0, lightRadius)/lightRadius, 2);
	float intensity = 1;
	//
	// a fragment végsõ színének meghatározása
	//
	fs_out_col = intensity * vec4(a+d+s, 1) * texture2D(myTexture, vs_out_tex.st);
}