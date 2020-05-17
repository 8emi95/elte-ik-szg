#pragma once
#include "includes/TextureObject.h"
#include "Includes/Mesh_OGL3.h"
class TexturedMesh
{
public:
	TexturedMesh() {}
	TexturedMesh(Mesh* mesh, Texture2D* texture);
	~TexturedMesh();

	Mesh& getMesh();
	const Texture2D& getTexture();

private:
	Mesh* mesh;
	Texture2D* texture;
};

