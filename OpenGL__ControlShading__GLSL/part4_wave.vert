
varying float x;
varying float z;

void main()
{

	vec4 Normal;
	Normal = vec4(gl_Vertex);
	x = Normal.x;
	z = Normal.z;
	
	gl_Position = gl_ModelViewProjectionMatrix * Normal;
} 
