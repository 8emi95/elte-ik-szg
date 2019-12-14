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
uniform vec3 eye;
uniform float plus;
uniform float forward;
uniform float right;
uniform float up;

// fénytulajdonságok
vec3 lightDir = vec3(-1,-1,-1);
vec3 lightPos = vec3(right,up,forward);
vec4 La = vec4(0.5,0.5,0.5,1.0);
vec4 Ld = vec4(plus,plus,plus,1.0);
uniform vec4 Ls = vec4(0.1,1.5,0.1,1.0);

// anyagtulajdonságok
vec4 K = vec4(1.0,1.0,1.0,1.0);

uniform sampler2D texture;

void main()
{
	vec3 lightDir = vs_out_pos - lightPos;
	//
	// ambiens szín számítása
	//
	vec4 ambient = La*K;

	//
	// diffúz szín számítása
	//
	float di = dot(normalize(vs_out_normal), normalize(-lightDir));
	vec4 diffuse = di*Ld*K;

	/* segítség:
		- normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	
	//
	// fényfoltképzõ szín
	//
	vec3 e = normalize(eye - vs_out_pos);
	vec3 r = normalize(reflect(lightDir, vs_out_normal));
	float si = pow(clamp(dot(e,r),0,1),10);
	vec4 specular = si*Ls*K;

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	
	//
	// a fragment végsõ színének meghatározása
	//
	fs_out_col = (ambient+diffuse+specular)*texture2D(texture, vs_out_tex0.st);
}