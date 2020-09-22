#version 410 core

// Input from the rasterizer
// Conceptually equivalent to
// "WritePixel( x, y, rasterColor )"

// Output data
out vec3 color;

void main()
{
	color = vec3(0.564f, 0.0f, 0.125f);
}
