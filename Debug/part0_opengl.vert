varying float y;

void main()
{
	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;

    float Wave = sin(gl_Vertex.x) * cos(gl_Vertex.x);
    vec4 tmpVertex = gl_Vertex;
    y = gl_Vertex.x;
    
    tmpVertex[1] = Wave;
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * tmpVertex;
} 
