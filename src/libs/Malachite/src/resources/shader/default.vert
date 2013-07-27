
varying vec2 pos;

void main(void)
{
	pos = gl_Vertex.xy;
	gl_Position = ftransform();
}
