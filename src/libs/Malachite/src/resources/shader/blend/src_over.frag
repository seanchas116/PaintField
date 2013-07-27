#version 120

//layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
//out vec4 ml_FragColor;
uniform sampler2D ml_InImage;
uniform float ml_Opacity;
varying vec2 ml_Position;

void main(void)
{
	vec2 dstPos = vec2(ml_Position.x * 0.5, ml_Position.y);
	vec2 srcPos = dstPos + vec2(0.5, 0);
	
	vec4 dst = texture2D(ml_InImage, dstPos);
	vec4 src = texture2D(ml_InImage, srcPos);
	
	src *= vec4(ml_Opacity);
	
	//vec4 src = vec4(1, 1, 1, 1);
	//vec4 dst = vec4(0, 0, 0, 0.5);
	gl_FragColor = src + vec4(1.0 - src.a) * dst;
	//gl_FragColor = dst;
	
	//gl_FragColor = vec4(1, 0.5, 0.5, 1);
	//gl_FragColor = texture2D(ml_InImage, pos);
}
