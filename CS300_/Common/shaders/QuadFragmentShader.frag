#version 410 core

// Input from the rasterizer
// Conceptually equivalent to
// "WritePixel( x, y, rasterColor )"

// Output data
out vec4 color;

void main()
{
	color = vec4(0.564f, 0.0f, 0.125f,1.0f);
}
