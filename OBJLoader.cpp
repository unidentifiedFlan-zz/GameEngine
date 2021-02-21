#define TINYOBJLOADER_IMPLEMENTATION

#include "OBJLoader.h"

OBJLoader::OBJLoader(TextureLoader &textureLoader) : _textureLoader(textureLoader)
{
}

std::shared_ptr<RenderObject> OBJLoader::load(const std::string &path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	directory = path.substr(0, path.find_last_of('/'));

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), directory.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	return processNode(attrib, shapes, materials);
}

std::shared_ptr<RenderObject> OBJLoader::processNode(tinyobj::attrib_t attrib,
	std::vector<tinyobj::shape_t> shapes,
	std::vector<tinyobj::material_t> materials)
{
	if (shapes.size() == 1) {
		return processMesh(attrib, shapes[0], materials);
	}

	std::shared_ptr<RenderObject> scene( new RenderObject() );
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		auto mesh = processMesh(attrib, shapes[s], materials);
		scene->addChild(mesh);
	}

	return scene;
}

std::shared_ptr<RenderObject> OBJLoader::processMesh(tinyobj::attrib_t attrib,
	tinyobj::shape_t mesh,
	std::vector<tinyobj::material_t> materials)
{
	std::vector<Vertex>  vertices;
	std::vector<unsigned int> indices;

	Texture specularTexture, diffuseTexture, normalTexture;

	size_t index_offset = 0;
	for (size_t f = 0; f < mesh.mesh.num_face_vertices.size(); f++) {
		int fv = mesh.mesh.num_face_vertices[f];
		for (size_t v = 0; v < fv; v++) {
			Vertex vertex;
			glm::vec3 position, normal;
			tinyobj::index_t idx = mesh.mesh.indices[index_offset + v];
			indices.push_back(index_offset + v);

			position.x = attrib.vertices[3 * idx.vertex_index + 0];
			position.y = attrib.vertices[3 * idx.vertex_index + 1];
			position.z = attrib.vertices[3 * idx.vertex_index + 2];
			normal.x = attrib.normals[3 * idx.normal_index + 0];
			normal.y = attrib.normals[3 * idx.normal_index + 1];
			normal.z = attrib.normals[3 * idx.normal_index + 2];
			glm::vec2 textureCoords;
			textureCoords.x = attrib.texcoords[2 * idx.texcoord_index + 0];
			textureCoords.y = attrib.texcoords[2 * idx.texcoord_index + 1];

			vertex.Position = position;
			vertex.Normal = normal;
			vertex.TexCoords = textureCoords;
			vertices.push_back(vertex);
		}
		index_offset += fv;

		// per-face material
		tinyobj::material_t material = materials[mesh.mesh.material_ids[f]];
		std::string prepath = directory + "/";
		std::string path = prepath + material.diffuse_texname;
		if (path != prepath) {
			diffuseTexture = _textureLoader.loadMaterialTexture(path, Texture::Type::DIFFUSE);
		}
		path = prepath + material.specular_texname;
		if (path != prepath) {
			specularTexture = _textureLoader.loadMaterialTexture(path, Texture::Type::SPECULAR);
		}
		path = prepath + material.bump_texname;
		if (path != prepath) {
			normalTexture = _textureLoader.loadMaterialTexture(path, Texture::Type::NORMAL);
		}
	}
;
	std::shared_ptr<RenderObject> obj( new RenderObject() );
	std::shared_ptr<Mesh> mesh_ptr( new Mesh(vertices, indices) );
	obj->setMesh( mesh_ptr );
	obj->name = mesh.name;
	obj->material.diffuseMap = diffuseTexture;
	obj->material.specularMap = specularTexture;
	obj->material.normalMap = normalTexture;

	return obj;
}
