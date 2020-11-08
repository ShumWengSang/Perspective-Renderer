#version 450 core
in gl_PerVertex
{
    vec4 gl_Position;
} gl_in[];

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

// 0 = no show, 1 = faceNormal, 2 = vertexNormal
layout(location = 0) uniform int showNormals;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
};

in VS_GS_VERTEX
{
    vec3 fragNormal;
    vec4 color;
} vsIn[];

out gl_PerVertex { vec4 gl_Position; };

layout(location = 0) out vec4 fragColor;

const float MAGNITUDE = 1.0;

void GenerateLine(int index)
{
    gl_Position = Projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = Projection * (gl_in[index].gl_Position +
                                vec4(vsIn[index].fragNormal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

vec3 CalcNormal(vec4 a, vec4 b, vec4 c)
{
    vec4 ba = b - a;
    vec4 ca = c - a;
    return normalize(cross(ba.xyz, ca.xyz));
}

void main()
{
    if(showNormals == 0)
    {
        // Discard everything.
    }
    // Vertex Normals
    else if(showNormals == 2)
    {
        for (int i = 0; i < vsIn.length(); i++)
        {
            fragColor = vsIn[i].color;
            GenerateLine(i);
        }
    }
    // Face normals
    else if(showNormals == 1)
    {
        // Add all three points
        vec4 point = vec4(0);
        for(int i = 0; i < vsIn.length();i++)
        {
            point += gl_in[i].gl_Position;
        }
        point /= vsIn.length();

        vec3 normal = CalcNormal(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

        gl_Position = Projection * point;
        EmitVertex();
        gl_Position = Projection * (point +
        vec4(normal, 0.0) * MAGNITUDE);
        EmitVertex();
        EndPrimitive();
    }

}