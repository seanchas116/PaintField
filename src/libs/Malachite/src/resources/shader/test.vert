#version 120

varying vec2 ml_Position;

void main(void)
{
	ml_Position = (gl_Vertex.xy + vec2(1)) * vec2(0.5);
	gl_Position = gl_Vertex;
}
