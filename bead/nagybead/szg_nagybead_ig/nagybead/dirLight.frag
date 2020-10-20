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


uniform bool is_sun = false; // a "nap" kirajzolás van-e


// színtér tulajdonságok
uniform vec3 eye_pos = vec3(0, 15, 15);
vec4 La = vec4(0.2f, 0.2f, 0.2f, 1);

// fénytulajdonságok
vec3 light_pos = vec3( 0, 5, 0 ); //hold fénye.
vec3 sunLight_pos = vec3( 0, 0, 0 ); //a nap az origóban van
vec4 Ld = vec4(0.7f, 0.7f, 0.7f, 1);
vec4 Ls = vec4(1, 1, 0.2, 1);

// anyagtulajdonságok
vec4 Ka = vec4(1, 1, 1, 1);
vec4 Kd = vec4(1, 1, 1, 1);
vec4 Ks = vec4(0.5, 0.5, 0.5, 1);
float specular_power = 64;


uniform vec4 sziklaSzin = vec4(1, 1, 1, 1);
uniform vec4 fenySzin = vec4(1, 1, 1, 1);


uniform sampler2D texImage;
uniform sampler2D texNormal;


// Segédfüggvény: feltéve hogy a és b vektorok nem esnek egy egyenesbe,
// mi az az R forgatómátrix, hogy b = R*a?
// Math: http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d/897677#897677
mat3 AToB(vec3 a, vec3 b){

	a = normalize(a);
	b = normalize(b);

	vec3 v = cross(a, b);
	float s = length(v);
	float c = dot(a, b);

	mat3 V = mat3(
		vec3(0,     v.z, -v.y),
		vec3(-v.z,  0,    v.x),
		vec3( v.y, -v.x,  0  )
	);

	mat3 R = mat3(1.0f) + V + V * V * ((1 - c) / (s*s));
	return R;

}

// Segédfüggvény a normálvektor módosításához a normalmap szerint
// alapötlet: a normalmap azt mutatja, hogyan kell változtatni egy normálison, ha az a (0,0,1) irányba mutatna
// viszont a normálvektoraink ugye nem így állnak, hanem vs_out_normal irányba mutatnak
// tehát: legyen R az a transzformáció, ami a (0,0,1)-et vs_out_normal-ba forgatja,
// és ekkor az új normálvektor valójában R*normal_from_normalmap lesz

vec3 normalTransform(vec3 normal_original, vec3 normal_from_normalmap){

	vec3 n_orig = normalize(normal_original);
	vec3 n_map = normalize(normal_from_normalmap);
	mat3 R = AToB(vec3(0,0,1), n_orig);
	return R*n_map;

}

void main()
{

	Ld = fenySzin;	
	
	// A normalmapet tartalmazó kép RGB értékei 0-tól 1-ig terjedõ számok 
	// A normálisok XYZ értékei viszont -1-tõl 1-ig terjedõ számok
	// szükséges transzformáció: normal = 2*color-1
	vec3 normalFromMap = 2*( (texture(texNormal, vs_out_tex0.st)).xyz ) - 1;

	vec3 normal = normalTransform(vs_out_normal, normalFromMap);


	//
	// ambiens szín számítása
	//
	vec4 ambient = La * Ka;

	//
	// diffúz szín számítása
	//

	/* segítség:
		- normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/

	vec3 toLight = normalize(light_pos - vs_out_pos);
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );

	vec3 toLightSun = normalize(sunLight_pos - vs_out_pos);
	float diSun = clamp( dot( toLightSun, normal), 0.0f, 1.0f );

	vec4 diffuse = Ld*Kd*di+Ld*Kd*diSun;

	//
	// fényfoltképzõ szín
	//

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	vec4 specular = vec4(0);

	if ( di > 0 ) // spekuláris komponens csak akkor van, ha diffúz is
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		vec3 r = reflect( -toLight, normal );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );

		r = reflect( -toLightSun, normal );
		float siSun = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );

		specular = Ls*Ks*si+Ls*Ks*siSun;
	}

	// ha nem használunk fényeket, akkor minden (a "nap" kivételével) fehér
	vec4 light =
				ambient +
				diffuse +
				specular;


	if (is_sun){
		//fs_out_col = vec4(1, 1, 0.2, 1); //sárga szín volt
		fs_out_col = texture(texImage, vs_out_tex0.st)  * fenySzin; //a nap texturáját színezzük a fényszínnel
	} else {
		fs_out_col = light * texture(texImage, vs_out_tex0.st);
	}

	fs_out_col *= sziklaSzin;

}