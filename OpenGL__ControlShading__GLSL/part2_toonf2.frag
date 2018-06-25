
varying vec3 Normal;

void main()
{
	float Intensity;
	vec4 Color;
	vec3 N = normalize(Normal);
	Intensity = dot(vec3(gl_LightSource[0].position),N);

	if (Intensity > 0.95)
	{
		Color = vec4(1.0,0.5,0.5,1.0);
	}
	else if (Intensity > 0.5)
	{
		Color = vec4(0.6,0.3,0.3,1.0);
	}
	else if (Intensity > 0.25)
	{
		Color = vec4(0.4,0.2,0.2,1.0);
	}
	else
	{
		Color = vec4(0.2,0.1,0.1,1.0);
	}
	
	gl_FragColor = Color;
}
