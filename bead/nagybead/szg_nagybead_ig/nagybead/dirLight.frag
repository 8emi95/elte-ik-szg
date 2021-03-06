#version 130

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

// kimen� �rt�k - a fragment sz�ne
out vec4 fs_out_col;

//
// uniform v�ltoz�k
//


uniform bool is_sun = false; // a "nap" kirajzol�s van-e


// sz�nt�r tulajdons�gok
uniform vec3 eye_pos = vec3(0, 15, 15);
vec4 La = vec4(0.2f, 0.2f, 0.2f, 1);

// f�nytulajdons�gok
vec3 light_pos = vec3( 0, 5, 0 ); //hold f�nye.
vec3 sunLight_pos = vec3( 0, 0, 0 ); //a nap az orig�ban van
vec4 Ld = vec4(0.7f, 0.7f, 0.7f, 1);
vec4 Ls = vec4(1, 1, 0.2, 1);

// anyagtulajdons�gok
vec4 Ka = vec4(1, 1, 1, 1);
vec4 Kd = vec4(1, 1, 1, 1);
vec4 Ks = vec4(0.5, 0.5, 0.5, 1);
float specular_power = 64;


uniform vec4 sziklaSzin = vec4(1, 1, 1, 1);
uniform vec4 fenySzin = vec4(1, 1, 1, 1);


uniform sampler2D texImage;
uniform sampler2D texNormal;


// Seg�df�ggv�ny: felt�ve hogy a �s b vektorok nem esnek egy egyenesbe,
// mi az az R forgat�m�trix, hogy b = R*a?
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

// Seg�df�ggv�ny a norm�lvektor m�dos�t�s�hoz a normalmap szerint
// alap�tlet: a normalmap azt mutatja, hogyan kell v�ltoztatni egy norm�lison, ha az a (0,0,1) ir�nyba mutatna
// viszont a norm�lvektoraink ugye nem �gy �llnak, hanem vs_out_normal ir�nyba mutatnak
// teh�t: legyen R az a transzform�ci�, ami a (0,0,1)-et vs_out_normal-ba forgatja,
// �s ekkor az �j norm�lvektor val�j�ban R*normal_from_normalmap lesz

vec3 normalTransform(vec3 normal_original, vec3 normal_from_normalmap){

	vec3 n_orig = normalize(normal_original);
	vec3 n_map = normalize(normal_from_normalmap);
	mat3 R = AToB(vec3(0,0,1), n_orig);
	return R*n_map;

}

void main()
{

	Ld = fenySzin;	
	
	// A normalmapet tartalmaz� k�p RGB �rt�kei 0-t�l 1-ig terjed� sz�mok 
	// A norm�lisok XYZ �rt�kei viszont -1-t�l 1-ig terjed� sz�mok
	// sz�ks�ges transzform�ci�: normal = 2*color-1
	vec3 normalFromMap = 2*( (texture(texNormal, vs_out_tex0.st)).xyz ) - 1;

	vec3 normal = normalTransform(vs_out_normal, normalFromMap);


	//
	// ambiens sz�n sz�m�t�sa
	//
	vec4 ambient = La * Ka;

	//
	// diff�z sz�n sz�m�t�sa
	//

	/* seg�ts�g:
		- normaliz�l�s: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skal�ris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/

	vec3 toLight = normalize(light_pos - vs_out_pos);
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );

	vec3 toLightSun = normalize(sunLight_pos - vs_out_pos);
	float diSun = clamp( dot( toLightSun, normal), 0.0f, 1.0f );

	vec4 diffuse = Ld*Kd*di+Ld*Kd*diSun;

	//
	// f�nyfoltk�pz� sz�n
	//

	/* seg�ts�g:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	vec4 specular = vec4(0);

	if ( di > 0 ) // spekul�ris komponens csak akkor van, ha diff�z is
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		vec3 r = reflect( -toLight, normal );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );

		r = reflect( -toLightSun, normal );
		float siSun = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );

		specular = Ls*Ks*si+Ls*Ks*siSun;
	}

	// ha nem haszn�lunk f�nyeket, akkor minden (a "nap" kiv�tel�vel) feh�r
	vec4 light =
				ambient +
				diffuse +
				specular;


	if (is_sun){
		//fs_out_col = vec4(1, 1, 0.2, 1); //s�rga sz�n volt
		fs_out_col = texture(texImage, vs_out_tex0.st)  * fenySzin; //a nap textur�j�t sz�nezz�k a f�nysz�nnel
	} else {
		fs_out_col = light * texture(texImage, vs_out_tex0.st);
	}

	fs_out_col *= sziklaSzin;

}