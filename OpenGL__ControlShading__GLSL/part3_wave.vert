

uniform float t;
varying float y;

void main()
{
	vec4 vertex;
	float R = sqrt( (gl_Vertex.x * gl_Vertex.x) + (gl_Vertex.z * gl_Vertex.z) );
	y = ( sin(R * t) * cos(R * t) ) / R;

	vertex.x = gl_Vertex.x;
	vertex.y = y;
	vertex.z = gl_Vertex.z;
	vertex.w = 1.0;
	
	gl_Position = gl_ModelViewProjectionMatrix * vertex;	
} 
