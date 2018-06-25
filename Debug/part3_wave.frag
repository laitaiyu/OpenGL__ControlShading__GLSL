varying float y;

vec4 computeColor(float y)
{
	vec4 Color;
	float index;
	float max = 0.15;
	float min = -0.15;
	
	index = 4.0 * (y-min)/(max - min);
	if (index >= 4.0 )
	{
		Color = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else if (index >= 3.0)
	{
		Color = vec4(1.0, 4.0 - index, 0.0, 1.0);
	}
	else if ( index >= 2.0)
	{
		Color = vec4(index - 2.0, 1.0, 0.0, 1.0);
	}
	else if ( index >= 1.0 )
	{
		Color = vec4(0.0, 1.0, 2.0- index, 1.0);
	}
	else if ( index >= 0.0 )
	{
		Color = vec4(0.0, index, 1.0, 1.0);
	}
	else
	{
		Color = vec4( 0.0, 0.0, 1.0, 1.0);
	}
	return Color;
}

void main()
{	
	vec4 Color = computeColor(y);
	gl_FragColor = Color;
} 
