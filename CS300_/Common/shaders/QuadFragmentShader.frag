#version 410 core

// Input from the rasterizer
// Conceptually equivalent to
// "WritePixel( x, y, rasterColor )"
in vec4 fcolor;

// Output data
out vec4 color;

void main()
{
	color = fcolor;
}
