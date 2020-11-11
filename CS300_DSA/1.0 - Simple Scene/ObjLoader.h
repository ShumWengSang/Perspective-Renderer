//
// Created by roland on 9/14/20.
//

#ifndef SIMPLE_SCENE_OBJLOADER_H
#define SIMPLE_SCENE_OBJLOADER_H

class Vertex;

class Mesh
{
public:
    virtual ~Mesh() = default;
    union Face
    {
        unsigned point[3];
        struct
        {
            unsigned index1, index2, index3;
        };
        bool Contains(int unsigned) const ;
    };
    struct Edge
    {
        glm::vec4 point[2];
    };
    
    // Vertex data
    std::vector<glm::vec4> Vertices;
    std::vector<glm::vec4> VertexNormals;
    std::vector<glm::vec3> faceNormals;
    std::vector<glm::vec3> VertexNormalsFace;
    std::vector<glm::vec3> Color;
    std::vector<glm::vec2> UV;
    std::vector<glm::vec2> TexCoords;

    // This is also the index array.
    std::vector<Face> Faces;
    // Alternative indexer (for non-triangles)
    std::vector<uint32_t> Index;

    // Lighting Data
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float shininess;

    Mesh & GenerateVertexNormals();
    Mesh & GenerateColors();
    Mesh & GenerateColors(glm::vec3 colors);
    Mesh &GenerateTexCoords(int mode, int toggleNormal);
    static std::vector<Vertex> CreateVertexFromMesh(Mesh const & mesh);

};

using MeshUniquePtr = UniquePtr<Mesh>;

class ObjLoader
{
public:
    static std::optional<MeshUniquePtr> LoadObj(std::string_view fileLocation);
    static std::optional<MeshUniquePtr> CreateSphere(float radius, unsigned numDivisions);
    static std::optional<MeshUniquePtr> CreateCircularLine(float radius, unsigned divisions);
};


std::ostream& operator<< (std::ostream& os, Mesh::Face const & face);
std::ostream& operator<< (std::ostream& os, Mesh::Edge const & face);
#endif //SIMPLE_SCENE_OBJLOADER_H
