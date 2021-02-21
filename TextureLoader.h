#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Texture.h"
#include <glad/glad.h>
#include <stb/stb_image.h>

class TextureLoader
{
public:
	std::vector<Texture> loaded_textures;

	Texture loadMaterialTexture(std::string path, Texture::Type type, std::string wrapType = "DEFAULT")
	{
		Texture texture;

		if (path.size() == 0) return texture;

		bool skip = false;
		for (unsigned int j = 0; j < loaded_textures.size(); ++j) {
			if (loaded_textures[j].path == path) {
				texture = loaded_textures[j];
				skip = true;
				break;
			}
		}

		if (!skip) {
			texture.id = textureFromFile(path, wrapType);
			texture.type = type;
			texture.path = path;
			loaded_textures.push_back(texture);
		}

		return texture;
	}

	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrComponents;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

private:
	unsigned int textureFromFile(std::string path, std::string wrapType = "DEFAULT")
	{

		unsigned int textureID;
		glGenTextures(1, &textureID);

		stbi_set_flip_vertically_on_load(true);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			GLuint wrap = GL_REPEAT;
			if (wrapType == "CLAMP")
				wrap = GL_CLAMP_TO_EDGE;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
};