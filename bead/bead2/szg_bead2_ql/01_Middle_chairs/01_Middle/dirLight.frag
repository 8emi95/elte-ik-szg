#version 130

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

// kimen� �rt�k - a fragment sz�ne
out vec4 fs_out_col;

//
// uniform v�ltoz�k
uniform float t;
uniform float forward;
uniform float right;
uniform float up;
uniform bool on;
// sz�nt�r tulajdons�gok
uniform vec3 eye_pos = vec3(0, 15, 15);

// f�nytulajdons�gok
vec3 light_pos = vec3(right,up,forward);
uniform vec4 La = vec4(0.2f, 0.2f, 0.2f, 1);
uniform vec4 Ld = vec4(1, 1, 1, 1);
uniform vec4 Ls = vec4(1, 1, 1, 1);

// anyagtulajdons�gok
uniform vec4 Ka = vec4(1, 1, 1, 1);
uniform vec4 Kd = vec4(0.75f, 0.25f, 0.125f, 1);
uniform vec4 Ks = vec4(0, 1, 0, 1);
uniform float specular_power = 10;
uniform sampler2D texImage;
uniform sampler2D texImage2;

void main()
{
	//
	// ambiens sz�n sz�m�t�sa
	//
	vec3 light_dir = vs_out_pos - light_pos;
	vec4 ambient = La * Ka;

	//
	// diff�z sz�n sz�m�t�sa
	//

	/* seg�ts�g:
		- normaliz�l�s: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skal�ris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	vec3 normal = normalize( vs_out_normal );
	vec3 toLight = - light_dir;
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );
	vec4 diffuse = Ld*Kd*di;

	//
	// f�nyfoltk�pz� sz�n
	//

	/* seg�ts�g:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	vec4 specular = vec4(0);

	if ( di > 0 )
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		vec3 r = normalize(reflect( light_dir, normal ));
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );
		specular = Ls*Ks*si;
	}

	if (on)
	{
		vec4 texColor1 = texture(texImage, vs_out_tex0.st);
		vec4 texColor2 = texture(texImage2, vs_out_tex0.st);
		fs_out_col = (ambient + diffuse + specular ) * mix(texColor1, texColor2, t);
	}
	else
	{
		fs_out_col = (ambient + diffuse + specular ) * texture(texImage, vs_out_tex0.st);
	}
}