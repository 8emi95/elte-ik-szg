#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;
in vec3 vs_out_view_pos;

out vec4 fs_out_col;

// irány fényforrás: fény iránya
uniform vec3 light_dir = vec3(-1,-1,-1);
uniform vec3 light_dir2 = vec3(0,0,-1);

// fénytulajdonságok: ambiens, diffúz, ...
uniform vec3 La = vec3(0.4, 0.4, 0.4);

uniform vec3 Ld = vec3(0.6, 0.6, 0.6);
uniform float LsStr = 0.1;

uniform vec3 Ld2 = vec3(0, 0, 0.5);
uniform float LsStr2 = 0.1;

uniform sampler2D texImage;

void main()
{

	vec3 ambient = La;

	vec3 normal = normalize(vs_out_norm);
	vec3 viewDir = normalize(vs_out_view_pos - vs_out_pos);

	//light 1
	vec3 to_light = normalize(-light_dir);	
	float cosa = clamp(dot(normal, to_light), 0, 1);
	vec3 diffuse = cosa*Ld;		

	vec3 reflectDir = reflect(-(light_dir - vs_out_pos), normal); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.9);
	vec3 specular = LsStr * spec * vec3(0.1); 

	//light 2
	vec3 to_light2 = normalize(-light_dir2);	
	float cosa2 = clamp(dot(normal, to_light2), 0, 1);
	vec3 diffuse2 = cosa2*Ld2;
	
	vec3 reflectDir2 = reflect(-(light_dir2 - vs_out_pos), normal); 
	float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 0.9);
	vec3 specular2 = LsStr2 * spec2 * vec3(0.1); 
	
	fs_out_col = vec4(ambient + diffuse + diffuse2 + specular + specular2, 1) * texture(texImage, vs_out_tex);
}