
varying vec4 diffuse, ambientGlobal, ambient;
varying vec3 Normal, lightDir, halfVector;
varying float Dist;

void main()
{
	vec4 ecPos;
	vec3 aux;
	
	Normal = normalize ( gl_NormalMatrix * gl_Normal );

	ecPos = gl_ModelViewMatrix * gl_Vertex;
	aux = vec3 ( gl_LightSource[0].position - ecPos );
	lightDir = normalize ( aux );
	Dist = length ( aux );
	halfVector = normalize ( gl_LightSource[0].halfVector.xyz );

	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	gl_Position = ftransform();

}