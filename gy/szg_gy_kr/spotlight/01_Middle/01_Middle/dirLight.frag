#version 130

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

uniform bool this_is_cone = false; // éppen a kúpot rajzoljuk-e ki?
uniform bool light_switch = true; // be van-e kapcsolva a spotlámpa?
uniform bool is_sharp = true; // "éles" legyen-e a spotlámpa fényének a határa

//
// uniform változók
//

// színtér tulajdonságok
uniform vec3 eye_pos = vec3(0, 15, 15);
uniform vec3 light_pos = vec3( 0, 5, 0 );
uniform vec3 lightDir = vec3(0, -1, 0);
float spotAngle = 2 * 3.14152f / 16.0f; // ekkora szögtartományban világít a spotlámpánk
uniform vec4 La = vec4(0.15f, 0.15f, 0.15f, 1);

// fénytulajdonságok
uniform vec4 Ld = vec4(0.7f, 0.7f, 0.7f, 1);
uniform vec4 Ls = vec4(1, 1, 1, 1);

// anyagtulajdonságok
uniform vec4 Ka = vec4(1, 1, 1, 1);
uniform vec4 Kd = vec4(1, 1, 1, 1);
uniform vec4 Ks = vec4(1, 1, 1, 1);
uniform float specular_power = 16;
uniform sampler2D texImage;

void main()
{

	// inicializálunk
	vec4 diffuse = vec4(0, 0, 0, 1);
	vec4 specular = vec4(0, 0, 0, 1);

	//
	// ambiens szín számítása
	//
	vec4 ambient = La * Ka;

	// *************************
	// spot lámpa intenzitása
	// *************************

	// Ha egy fregmenst nem egészen olyan szögben éri a fénysugár (-toLight), mint amerre néz a spotlámpa (lightDir),
	// akkor a spotlámpa körén kívül eshet: a rá számolt fény intenzitását csökkentjük
	// ezt az intenzitást számoljuk ki spoti-ban
	float spoti;
	
	// toLight a fregmensbõl a fényforrásba mutató vektor (-toLight pedig a fényforrásból a fregmensbe)
	// a spotlámpánk lightDir irányba néz
	vec3 toLight = normalize(light_pos - vs_out_pos);
	// angle fejezi ki a két vektor szögét radiánban
	float angle = acos(dot(-toLight, lightDir));

	// ha angle túl nagy, akkor spoti 1-nél kisebb értékü -> gyengébb a fény éri a fregmenst!

	if(is_sharp){
		// "éles" fénykör: vagy van fény, vagy nincs
		spoti = angle < spotAngle ? 1 : 0;
	} else {
		// https://en.wikipedia.org/wiki/Sigmoid_function
		// finomabb átmenetet kapunk egy megfelelõen megválasztott szigmoid fügvénnyel:
		// minél nagyobb a szög, annál kisebb értékü spoti, de nem hirtelen ugrik
		float ex = exp(-64 * (angle - spotAngle));
		spoti  = ex / (ex + 1);
	}

	//
	// diffúz szín számítása
	//

	/* segítség:
		- normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
		- skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
		- clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	vec3 normal = normalize( vs_out_normal );
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );
	diffuse = Ld*Kd*di;

	//
	// fényfoltképzõ szín
	//

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/

	if ( di > 0 )
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		vec3 r = reflect( -toLight, normal );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );
		specular = Ls*Ks*si;
	}

	// a kúpot csak textúrázzuk
	if(this_is_cone){
		fs_out_col = texture(texImage, vs_out_tex0.st);
	} else if (light_switch){
		// ha van fényünk, akkor a diffúz és spekuláris fény erõssége spoti-tól függ
		fs_out_col = (ambient + spoti*(diffuse + specular)) * texture(texImage, vs_out_tex0.st);
	} else {
		// egyébként nincs diffúz és spekuláris komponens, csak ambiens
		fs_out_col = ambient * texture(texImage, vs_out_tex0.st);
	}

}