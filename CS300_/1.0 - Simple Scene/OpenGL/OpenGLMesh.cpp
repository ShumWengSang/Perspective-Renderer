//
// Created by roland on 9/30/20.
//
#include "../stdafx.h"
#include "OpenGLMesh.h"
#include "../Renderer/Buffer.h"

Mesh &OpenGLMesh::SetIndexBuffer(SharedPtr<VertexArray> &vaoOwner) {
    // Add index array
    if(vaoOwner->GetType() == VertexArray::PrimitiveType::Triangles) {
        SharedPtr<IndexBuffer> meshIB = IndexBuffer::Create(
                (unsigned *) this->Faces.data(),
                (this->Faces.size() * 3));
        vaoOwner->SetIndexBuffer(meshIB);
    }
    else if(vaoOwner->GetType() == VertexArray::PrimitiveType::Lines)
    {
        SharedPtr<IndexBuffer> meshIB = IndexBuffer::Create(
                (unsigned *) this->Index.data(),
                (this->Index.size()));
        vaoOwner->SetIndexBuffer(meshIB);
    }
    return *this;
}

Mesh &OpenGLMesh::SetVerticeBuffer(SharedPtr<VertexArray> &vaoOwner) {
    SharedPtr<VertexBuffer> meshVB = VertexBuffer::Create(
            (float*)this->Vertices.data(),
            sizeof(this->Vertices[0]) * this->Vertices.size());
    meshVB->SetLayout({
                              { ShaderDataType::Float4, "vPosition"}
                      });
    vaoOwner->AddVertexBuffer(meshVB);
    return *this;
}

Mesh &OpenGLMesh::GenerateRandomColors(SharedPtr<VertexArray> &vaoOwner) {
    std::vector<float> colors(4 * this->Vertices.size());
    for(unsigned i = 0; i < colors.size(); ++i)
    {
        colors[i] = rand() % 255;
        colors[i] /= 255;
        //colors[i] = 0.5f;
    }
    for(unsigned i = 1; i < colors.size(); i +=4)
    {
        //colors[i] = 1;
    }
    for(unsigned  i = 3; i < colors.size(); i += 4)
    {
        colors[i] = 1.0f;
    }
    SharedPtr<VertexBuffer> colorVB = VertexBuffer::Create(
            colors.data(),
            sizeof(colors[0]) * colors.size());
    colorVB->SetLayout({
                               { ShaderDataType::Float4, "vColor"}
                       });
    vaoOwner->AddVertexBuffer(colorVB);
    return *this;
}

Mesh &OpenGLMesh::CalcVertexNormal(SharedPtr<VertexArray> &vaoOwner) {

    for(unsigned i = 0; i < Vertices.size();++i)
    {
        // For each vertex, we find all the faces that it is in.
        std::vector<unsigned> FaceIndex;
        for(unsigned j = 0; j < Faces.size(); ++j)
        {
            // If the face contains the vertex, we store it.
            if(Faces[j].Contains(i))
            {
                FaceIndex.emplace_back(j);
            }
        }

        glm::vec3 vertexNormal = glm::vec3(0.0f);
        // Now from all the faces that contains this vertex, calculate the average normal.
        // Ignore all parallel
        Vec3Set set;
        for(unsigned j = 0; j < FaceIndex.size(); ++j)
        {
            // Get the vertices for this face.
            glm::vec3 FaceNormal = this->faceNormals[FaceIndex[j]];

            // If we can't find it.
            if(set.find(FaceNormal) == set.end())
            {
                // Insert it and add.
                // Sum up the normals
                set.insert(FaceNormal);
                vertexNormal += FaceNormal;
            }
        }

        // And divide to average it out.
        vertexNormal /= set.size();
        VertexNormals.emplace_back(glm::vec4(vertexNormal, 0.0f));
#ifdef DEBUG_OUT
        std::cout << "Vertex Normal for "<< i << " is: " << glm::to_string(this->VertexNormals.back()) <<
                  " and this is affeced by " << set.size() << " number of faces" << std::endl;
#endif
    }

    return *this;

}

Mesh &OpenGLMesh::SetVertexNormal(SharedPtr<VertexArray> &vaoOwner) {
    // Now that we have calculated all the vertex normals, we can send it over to the GPU.
    SharedPtr<VertexBuffer> vertexNormalVB = VertexBuffer::Create(
            (float*)VertexNormals.data(),
            sizeof(VertexNormals[0]) * VertexNormals.size());
    vertexNormalVB->SetLayout({
                                      { ShaderDataType::Float4, "vvertexNormal"}
                              });
    vaoOwner->AddVertexBuffer(vertexNormalVB);
    return *this;
}

Mesh &OpenGLMesh::GenerateFaceNormalLines(SharedPtr<VertexArray> &vaoOwner) {
    std::vector<Mesh::Edge> positions;
    std::vector<unsigned> indexArr;
    // For every face
    for(unsigned i = 0, j = 0;i < this->Faces.size(); ++i, j += 2)
    {
        unsigned index1 = Faces[i].index1;
        unsigned index2 = Faces[i].index2;
        unsigned index3 = Faces[i].index3;

        // Get the points that make up this face
        glm::vec4 a = this->Vertices[index1];
        glm::vec4 b = this->Vertices[index2];
        glm::vec4 c = this->Vertices[index3];
        glm::vec4 normal = glm::vec4(this->faceNormals[i], 0.0f);

        // Find the center of those points
        Mesh::Edge newEdge;
        newEdge.point[0] = (a + b + c) / 3;
        // Point + Dir
        newEdge.point[1] = newEdge.point[0] + normal;
        positions.emplace_back(newEdge);
        // Insert into index array.
        indexArr.emplace_back(j);
        indexArr.emplace_back(j + 1);
    }

    // Now all the edges are set. We create a Buffer and attach it.
    SharedPtr<VertexBuffer> faceNormalLines = VertexBuffer::Create(
            (float*)positions.data(),
            sizeof(positions[0]) * positions.size());
    faceNormalLines->SetLayout({
                                      { ShaderDataType::Float4, "vPosition"}
                              });
    vaoOwner->AddVertexBuffer(faceNormalLines);

    // Create index buffers for it
    SharedPtr<IndexBuffer> meshIB = IndexBuffer::Create(
            (unsigned*)indexArr.data(),
            (indexArr.size()));
    vaoOwner->SetIndexBuffer(meshIB);
    return *this;
}

Mesh &OpenGLMesh::GenerateVertexNormalLines(SharedPtr<VertexArray> &vaoOwner) {
    std::vector<Mesh::Edge> positions;
    std::vector<unsigned> indexArr;

    // For every vertex we generate a point on it, and one on the side its pointing towards
    for(unsigned i = 0, j = 0;i < this->Vertices.size(); ++i, j += 2)
    {
        glm::vec4 pointA = Vertices[i];
        glm::vec4 normal = this->VertexNormals[i];
        glm::vec4 pointB = pointA + normal;

        // Find the center of those points
        Mesh::Edge newEdge;
        newEdge.point[0] = pointA;
        // Point + Dir
        newEdge.point[1] = pointB;
        positions.emplace_back(newEdge);
        // Insert into index array.
        indexArr.emplace_back(j);
        indexArr.emplace_back(j + 1);
    }

    // Now all the edges are set. We create a Buffer and attach it.
    SharedPtr<VertexBuffer> vertexNormalLines = VertexBuffer::Create(
            (float*)positions.data(),
            sizeof(positions[0]) * positions.size());
    vertexNormalLines->SetLayout({
                                       { ShaderDataType::Float4, "vPosition"}
                               });
    vaoOwner->AddVertexBuffer(vertexNormalLines);

    // Create index buffers for it
    SharedPtr<IndexBuffer> meshIB = IndexBuffer::Create(
            (unsigned*)indexArr.data(),
            (indexArr.size()));
    vaoOwner->SetIndexBuffer(meshIB);
    return *this;
}
