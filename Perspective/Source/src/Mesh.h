#pragma once

#include "ModelSystem.h"

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(
		std::vector<Vertex> const& vertices, std::vector<unsigned int> const& indices
		, std::vector<Texture> const& textures
	);

	void Draw() const;
	void Clear();

private:
	//  render data
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};
