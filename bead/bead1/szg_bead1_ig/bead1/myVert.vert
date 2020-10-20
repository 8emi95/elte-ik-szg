#version 130

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_col;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_col;

// shader külsõ paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

uniform float szorzo = 0;
uniform int szinID = 0;
vec3 ujszin = vec3(0,0,0);

void main()
{
	gl_Position = proj * view * world * vec4( vs_in_pos, 1 );
	if(szinID == 1)
		ujszin = vec3(1,0,0);
	if(szinID == 2)
		ujszin = vec3(0,1,0);
	if(szinID == 3)
		ujszin = vec3(0,0,3);

	vs_out_col = vs_in_col*(1-szorzo)+ujszin*(szorzo);
	//vs_out_col = vs_in_col*0+vec3(col_r,col_g,col_b);//vec3(0,0,0);//vs_in_col;
	//vs_out_col = vs_in_col*0.5+vec3(col_r,col_g,col_b)*0.5;
	/*if(col_r == 0. && col_g == 0. && col_b == 0.) {
		vs_out_col = vs_in_col;
	} else {
		vs_out_col = vec3(col_r,col_g,col_b);
	}*/
}