//
// Created by roland on 9/14/20.
//


#include "stdafx.h"
#include "ObjLoader.h"

static float Remap(float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

#define VecMapShortcut(a) result.a = Remap(value.a, low1.a, high1.a, low2.a, high2.a);

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
#undef VecMapShortcut

static glm::vec3 CalcNormal(glm::vec4 const & a, glm::vec4 const & b, glm::vec4 const & c)
{
    glm::vec3 ba = a - b;
    glm::vec3 ca = a - c;
    glm::vec3 cross = glm::cross(ba, ca);
    cross = glm::normalize(cross);
    return cross;
}

std::optional<MeshUniquePtr> ObjLoader::LoadObj(const char *fileLocation) {
    if (fileLocation == nullptr) {
        std::cout << "Missing file location" << std::endl;
        return {};
    }

    // Attempt to open file in read mode
    FILE *file = fopen(fileLocation, "r");
    if (file == NULL) {
        std::cout << "Impossible to open the file !" << std::endl;
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
    MeshUniquePtr newMesh = std::make_unique<Mesh>();

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
                // Now we read %d by %d until the end.
                while(token != nullptr) {
                    sscanf(token, "%d", &vertexIndex);
                    token = strtok(nullptr, " ");
                    // When we a triangle, we calc normal
                    if(count < 3)
                    {
                        // NOOP
                        vertexIndices.push_back(vertexIndex);
                    }
                    // For more points, just set to what the calculated normal is.
                    else if(count >= 3)
                    {
                        // If odd count, use clockwise
                        vertexIndices.push_back(vertexIndices[vertexIndices.size() - 2]);
                        vertexIndices.push_back(vertexIndices[vertexIndices.size() - 2]);
                        vertexIndices.push_back(vertexIndex);
                        // Else if even count, use counter clockwise

                    }
                    else
                    {
                        // Shouldn't reach here.
                        std::cout << "Error! This means there were less then 3 vertices for this face!" << std::endl;
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

    //newMesh->UV.reserve(vertexIndices.size());
    //newMesh->Normals.reserve(vertexIndices.size());

    const glm::vec3 Center = usefulStuff.CalcCenter();
    const glm::vec4 NewMaxRange = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    const glm::vec4 NewMinRange = glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f);
    const glm::vec4 OldMaxRange = glm::vec4(usefulStuff.MaxX, usefulStuff.MaxY, usefulStuff.MaxZ, 0);
    const glm::vec4 OldMinRange = glm::vec4(usefulStuff.MinX, usefulStuff.MinY, usefulStuff.MinZ, 0);

    for(unsigned int i = 0; i < temp_vertices.size(); ++i)
    {
        glm::vec4 vertex = temp_vertices[i];
        // Modify the vertex so its in [-1, 1] range and centered
        vertex = Remap(vertex, OldMinRange, OldMaxRange, NewMinRange, NewMaxRange);
        newMesh->Vertices.push_back(vertex);
    }

    // Assign Faces and Normal
    for(unsigned i = 0; i < vertexIndices.size();)
    {
        Mesh::Face face;
        for(int j = 0; j < 3; ++i, ++j) {
            // Account for index starts from one
            face.face[i] = vertexIndices[i] - 1;
        }
        newMesh->Faces.push_back(face);

        // Generate face normals
        glm::vec4& a = newMesh->Vertices[face.index1];
        glm::vec4& b = newMesh->Vertices[face.index2];
        glm::vec4& c = newMesh->Vertices[face.index3];
        newMesh->Normals.push_back(CalcNormal(a, b, c));
    }

    // Now we scale the model to the range of [-1,1] and center
    // obj at [0,0]

    fclose(file);
    return newMesh;
}

void Mesh::SetupBuffers() {
    // Gen Vertex Array Object
    glGenVertexArrays(1, &VAO_ID);
    glBindVertexArray(VAO_ID);

    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec4),
                 Vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);


}
