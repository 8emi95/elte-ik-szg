#version 130

// kirajzolando geometria pozicio- es szinattributumai
vec4 positions[4] = vec4[4](
	vec4(-1, -1, 0, 1),
	vec4( 1, -1, 0, 1),
	vec4(-1,  1, 0, 1),
	vec4( 1,  1, 0, 1)
);

vec4 colors[4] = vec4[4](
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1),
	vec4(1, 1, 1, 1)
);

// kimeneti változók: a csúcspont színe és pozíciója
out vec4 vs_out_col;
out vec4 vs_out_pos;

void main()
{
	// gl_VertexID: beépített kimeneti változó, a vertex-shader legfõbb feladata ezt kitölteni
	//mint mondtuk, itt midnenfele traf. vegrehajthato, szamitasok. valamilyen pozicioadatot szeretnenk, amit tovabbadunk, hogy primitivek lehessenek beloluk
	//az adott id-ju vertex poziciojanak lekerdezese a positions tombbol

	//valszeg ez olyasmi, amit a tobbi shader is lat, ezert nem kell kulon kimenetkent atadni --> a gl_Position egy vertexhez rendelt pozicio, es neki ad erteket a vertex shader. tehat akkor gondolom, mas shader is lathatja
	gl_Position = positions[gl_VertexID]; 

	// saját kimeneti változók kitöltése (további információk továbbadására)
	vs_out_col	= colors[gl_VertexID];
	vs_out_pos = gl_Position;
}