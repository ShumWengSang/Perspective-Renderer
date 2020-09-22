//
// Created by roland on 9/14/20.
//

#ifndef SIMPLE_SCENE_OBJLOADER_H
#define SIMPLE_SCENE_OBJLOADER_H

struct Mesh
{
    union Face
    {
        unsigned face[3];
        struct
        {
            unsigned index1, index2, index3;
        };
    };
    union Edge
    {
        unsigned point[2];
        struct
        {
            unsigned index1, index2;
        };
    };
    
    // Vertex data
    std::vector<glm::vec4> Vertices;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec4> Color;
    std::vector<glm::vec2> UV;
    std::vector<GLuint> Indexes;

    std::vector<Face> Faces;
    std::vector<Edge> Edges;

    // Lighting Data
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float shininess;

};

using MeshUniquePtr = std::unique_ptr<Mesh>;

class ObjLoader
{
public:
    static std::optional<MeshUniquePtr> LoadObj(const char* fileLocation);
};
#endif //SIMPLE_SCENE_OBJLOADER_H
