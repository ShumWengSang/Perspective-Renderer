//
// Created by roland on 9/14/20.
//


#include "stdafx.h"
#include "ObjLoader.h"
#include "Renderer/Buffer.h"
#include "OpenGL/OpenGLMesh.h"

static float Remap(float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

#define VecMapShortcut(a) result.a = Remap(value.a, low1.a, high1.a, low2.a, high2.a);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
static glm::vec4 Remap(glm::vec4 const & value,
                       glm::vec4 const &  low1, glm::vec4 const & high1,
                       glm::vec4 const &  low2, glm::vec4 const &  high2)
{
    glm::vec4 result;
    VecMapShortcut(x);
    VecMapShortcut(y);
    VecMapShortcut(z);
    VecMapShortcut(w);
    return result;
}
#pragma clang diagnostic pop

static glm::vec4 Remap(glm::vec4 const & value,
                       glm::vec2 const & inMinMax,
                       glm::vec2 const & outMinMax)
{
    return outMinMax.x + (value - inMinMax.x) * (outMinMax.y - outMinMax.x) / (inMinMax.y - inMinMax.x);
}
#undef VecMapShortcut

static glm::vec3 CalcNormal(glm::vec4 const & a, glm::vec4 const & b, glm::vec4 const & c)
{
    glm::vec3 ba = b - a;
    glm::vec3 ca = c - a;
    glm::vec3 cross = glm::cross(ba, ca);
    cross = glm::normalize(cross);
    return cross;
}

std::optional<MeshUniquePtr> ObjLoader::LoadObj(const char *fileLocation) {
    if (fileLocation == nullptr) {
        std::cout << "Missing file location" << std::endl;
        DEBUG_BREAKPOINT();
        return {};
    }

    // Attempt to open file in read mode
    FILE *file = fopen(fileLocation, "r");
    if (file == NULL) {
        std::cout << "Impossible to open the file !" << std::endl;
        DEBUG_BREAKPOINT();
        return {};
    }

    // File is loaded. Now lets get some temp variables to record
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec4> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    struct UsefulInformation
    {
        float MaxX, MaxY, MaxZ;
        float MinX, MinY, MinZ;
        UsefulInformation()
        {
            MaxX = MaxY = MaxZ = std::numeric_limits<float>::min();
            MinX = MinY = MinZ = std::numeric_limits<float>::max();
        }

        void Record(glm::vec4 const & currentVertex)
        {
            MaxX = std::max(currentVertex.x, MaxX);
            MaxY = std::max(currentVertex.y, MaxY);
            MaxZ = std::max(currentVertex.z, MaxZ);

            MinX = std::min(currentVertex.x, MinX);
            MinY = std::min(currentVertex.y, MinY);
            MinZ = std::min(currentVertex.z, MinZ);
        }

        glm::vec3 CalcCenter()
        {
            glm::vec3 center;
            center.x = (MaxX + MinX) / 2;
            center.y = (MaxY + MinY) / 2;
            center.z = (MaxZ + MinZ) / 2;
            return center;
        }
    };
    UsefulInformation usefulStuff;

    // Now we can to process this so opengl is happy with it.
    MeshUniquePtr newMesh = CreateUniquePtr<OpenGLMesh>();

    while (true) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File.

        // If first thing is V.
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec4 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
            usefulStuff.Record(vertex);

        } else if (strcmp(lineHeader, "v") == 0) {
            glm::vec4 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "f") == 0) {
            // Ok lets read the entire line
            char buffer[1000];
            if(fgets(buffer, sizeof(buffer) * sizeof(char), file))
            {
                unsigned int vertexIndex = -1;
                char * token = strtok(buffer, " ");
                unsigned int count = 0;
                unsigned startIndex = -1;
                // Now we read %d by %d until the end.
                while(token != nullptr) {
                    sscanf(token, "%d", &vertexIndex);
                    token = strtok(nullptr, " ");
                    // When we a triangle, we calc normal
                    if(count < 3)
                    {
                        if(count == 0)
                            startIndex = vertexIndex;
                        // NOOP
                        vertexIndices.push_back(vertexIndex);
                    }
                    // For more points, just set to what the calculated normal is.
                    else if(count >= 3)
                    {
                            // If odd count, use clockwise
                        vertexIndices.push_back(startIndex);
                        vertexIndices.push_back(vertexIndices[vertexIndices.size() - 2]);
                        vertexIndices.push_back(vertexIndex);

                    }
                    else
                    {
                        // Shouldn't reach here.
                        std::cout << "Error! This means there were less then 3 vertices for this point!" << std::endl;
                        DEBUG_BREAKPOINT();
                    }
                    ++count;
                }
            }

        } else {
            // Don't know
            char buffer[1000];
            fgets(buffer, sizeof(buffer) * sizeof(char), file);
            std::cout << "Obj loader not sure what this is: " << buffer << std::endl;
        }
    }

    const float minRange = std::min(usefulStuff.MinX, std::min(usefulStuff.MinY, usefulStuff.MinZ));
    const float maxRange = std::max(usefulStuff.MaxX, std::max(usefulStuff.MinY, usefulStuff.MaxZ));
    const glm::vec2 inMinMax = {minRange, maxRange};
    const glm::vec2 outMinMax = {-1.0f, 1.0f};

    for(unsigned int i = 0; i < temp_vertices.size(); ++i)
    {
        glm::vec4 vertex = temp_vertices[i];
        // Modify the vertex so its in [-1, 1] range and centered
        vertex = Remap(vertex, inMinMax, outMinMax);
        // These are points
        vertex.w = 1;
        newMesh->Vertices.push_back(vertex);

#ifdef DEBUG_OUT
      std::cout << "Loaded Vertices: " << glm::to_string(vertex) << std::endl;
#endif
    }

    // Assign Faces and Normal
    for(unsigned i = 0; i < vertexIndices.size();)
    {
        Mesh::Face face;
        for(int j = 0; j < 3; ++i, ++j) {
            // Account for index starts from one
            face.point[j] = vertexIndices[i] - 1;
        }
        newMesh->Faces.push_back(face);
#ifdef DEBUG_OUT
        std::cout << "Loaded Faces: " << face << std::endl;
#endif

        // Generate point normals
        glm::vec4& a = newMesh->Vertices[face.index1];
        glm::vec4& b = newMesh->Vertices[face.index2];
        glm::vec4& c = newMesh->Vertices[face.index3];
        auto normal = CalcNormal(a, b, c);
        // Each face will have the same normal three times.
        newMesh->faceNormals.emplace_back(glm::round(normal));
#ifdef DEBUG_OUT
        std::cout << "Normal for Face is: " << glm::to_string(newMesh->faceNormals.back()) << std::endl;
#endif
    }

    fclose(file);
    return newMesh;
}

std::optional<MeshUniquePtr> ObjLoader::CreateSphere(float radius, unsigned numDivisions) {
    MeshUniquePtr sphereMesh = CreateUniquePtr<OpenGLMesh>();

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float sectorStep = 2 * PI / numDivisions;
    float stackStep = PI / numDivisions;
    float sectorAngle, stackAngle;

    for(unsigned i = 0; i <= numDivisions; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(unsigned j = 0; j <= numDivisions; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            glm::vec4 vertex = {x,y,z,1.0f};
#ifdef DEBUG_OUT
            std::cout << "Sphere Vertex generated: " << glm::to_string(vertex) << std::endl;
#endif
            sphereMesh->Vertices.emplace_back(vertex);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            glm::vec4 vertexNormal = {nx,ny,nz,0.0f};
            sphereMesh->VertexNormals.emplace_back(vertexNormal);

        }
    }

    // Once all the sphere points are generated, create triangles from adjacent points
    // generate CCW index list of sphere triangles
    unsigned k1, k2;
    for (unsigned i = 0; i < numDivisions; ++i)
    {
        k1 = i * (numDivisions + 1);     // beginning of current stack
        k2 = k1 + numDivisions + 1;      // beginning of next stack

        for (unsigned j = 0; j < numDivisions; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                Mesh::Face face = {k1, k2, k1 + 1};
                sphereMesh->Faces.emplace_back(face);
                //Calc Normal
                glm::vec4 normal = sphereMesh->Vertices[face.index1];
                normal += sphereMesh->Vertices[face.index2];
                normal += sphereMesh->Vertices[face.index3];
                sphereMesh->faceNormals.emplace_back(glm::vec3(normal / 3));

            }

            // k1+1 => k2 => k2+1
            if (i != (numDivisions - 1))
            {
                Mesh::Face face = {k1 + 1, k2, k2 + 1};
                sphereMesh->Faces.emplace_back(face);
                //Calc Normal
                glm::vec4 normal = sphereMesh->Vertices[face.index1];
                normal += sphereMesh->Vertices[face.index2];
                normal += sphereMesh->Vertices[face.index3];
                sphereMesh->faceNormals.emplace_back(glm::vec3(normal / 3));
            }
        }
    }
    return sphereMesh;
}

std::ostream &operator<<(std::ostream &os, const Mesh::Face &face) {
    os << face.point[0] << '-' << face.point[1] << '-' << face.point[2];
    return os;
}

std::ostream &operator<<(std::ostream &os, const Mesh::Edge &edge) {
    os << '-' << glm::to_string(edge.point[0]) << '-' << glm::to_string(edge.point[0]);
    return os;
}


bool Mesh::Face::Contains(unsigned index) const {
    return index1 == index || index2 == index || index3 == index;
}
