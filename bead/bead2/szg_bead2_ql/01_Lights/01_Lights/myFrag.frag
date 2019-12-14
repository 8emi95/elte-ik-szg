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

// sz�nt�r tulajdons�gok
uniform vec3 eye;
uniform float plus;
uniform float forward;
uniform float right;
uniform float up;

// f�nytulajdons�gok
vec3 lightDir = vec3(-1,-1,-1);
vec3 lightPos = vec3(right,up,forward);
vec4 La = vec4(0.5,0.5,0.5,1.0);
vec4 Ld = vec4(plus,plus,plus,1.0);
uniform vec4 Ls = vec4(0.1,1.5,0.1,1.0);

// anyagtulajdons�gok
vec4 K = vec4(1.0,1.0,1.0,1.0);

uniform sampler2D texture;

void main()
{
	vec3 lightDir = vs_out_pos - lightPos;
	//
	// ambiens sz�n sz�m�t�sa
	//
	vec4 ambient = La*K;

	//
	// diff�z sz�n sz�m�t�sa
	//
	float di = dot(normalize(vs_out_normal), normalize(-lightDir));
	vec4 diffuse = di*Ld*K;

	/* seg�ts�g:
		- normaliz�l�s: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skal�ris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	
	//
	// f�nyfoltk�pz� sz�n
	//
	vec3 e = normalize(eye - vs_out_pos);
	vec3 r = normalize(reflect(lightDir, vs_out_normal));
	float si = pow(clamp(dot(e,r),0,1),10);
	vec4 specular = si*Ls*K;

	/* seg�ts�g:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	
	//
	// a fragment v�gs� sz�n�nek meghat�roz�sa
	//
	fs_out_col = (ambient+diffuse+specular)*texture2D(texture, vs_out_tex0.st);
}