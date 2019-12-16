#version 130

// bemeneti változó - most a vertex shader-bõl (vagyis ottani out)
in vec4 vs_out_col;
in vec4 vs_out_pos;
uniform vec3 transform = vec3(0, 0, 1); //tx ty s
// kimeneti változó - a fragment színe
out vec4 fs_out_col;

void main()
{
	int iterationNumber = 1000;
	//pos.x --> Re
	//pos.y --> Im

	vec2 t_pos = vs_out_pos.xy*transform.zz + transform.xy;
	vec2 pos = t_pos;
	int i = 0;
	for(; i < iterationNumber; i++)
	{
		vec3 tmp = pos.xyx * pos.xyy;
		if((tmp.x + tmp.y) > 2) break;
		pos.x = tmp.x - tmp.y + t_pos.x;
		pos.y =  2 * tmp.z + t_pos.y;
	}
	float col = float(i % 100)/ 100.0 ;
	fs_out_col = vec4(col, col, col, 1);
}