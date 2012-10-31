// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Algorithm.h"
#include "Graphics/ImageLoader.h"
#include "Graphics/OpenGL.h"
#include "Graphics/TextureAtlas.h"

TextureAtlas::TextureAtlas(const Data &img) : name(0), width(0), height(0)
{
	R_ASSERT(img, "No data provided");

	void *data = nullptr;
	ImageInfo info;
	if (!ImageLoader::load(data, info, img))
		return;

#ifndef NDEBUG
	// Ensure texture dimension is supported by the hardware
	int max_texture_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	R_ASSERT(info.width <= static_cast<unsigned int>(max_texture_size)
	         && info.height <= static_cast<unsigned int>(max_texture_size),
	         "Texture dimension exceeds max texture size supported by hardware");
	//R_ASSERT(this->is_valid(info.width) && this->is_valid(info.height),
	//         "Texture dimension must be divisible by 4 and greater than 64");
#endif

	this->width = info.width;
	this->height = info.height;

	GLint format = -1, internal = -1;
	switch (info.compressed)
	{
	#ifdef GL_OES_compressed_ETC1_RGB8_texture
		case ImageLoader::ETC1:
			this->name = TextureManager::Instance().create_compressed(GL_ETC1_RGB8_OES, info.width, info.height, info.size, data);
			break;
	#endif // ETC1
	#ifdef GL_IMG_texture_compression_pvrtc
		case ImageLoader::PVRTC:
			R_ASSERT(info.depth == 2 || info.depth == 4, "Invalid colour depth");
			R_ASSERT(info.channels == 3 || info.channels == 4, "Invalid number of colour channels");
			if (info.channels == 3)
				internal = (info.depth == 2) ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			else
				internal = (info.depth == 2) ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			this->name = TextureManager::Instance().create_compressed(internal, info.width, info.height, info.size, data);
			break;
	#endif // PVRTC
		default:
			switch (info.channels)
			{
				case 1:
					R_ASSERT(info.depth == 8, "Invalid colour depth");
					format = GL_LUMINANCE;
					internal = GL_LUMINANCE;
					break;
				case 2:
					R_ASSERT(info.depth == 16, "Invalid colour depth");
					format = GL_LUMINANCE_ALPHA;
					internal = GL_LUMINANCE_ALPHA;
					break;
				case 3:
					R_ASSERT(info.depth == 16 || info.depth == 24, "Invalid colour depth");
					format = GL_RGB;
					internal = (info.depth == 16) ? GL_RGB565 : GL_RGB8;
					break;
				case 4:
					R_ASSERT(info.depth == 16 || info.depth == 32, "Invalid colour depth");
					format = GL_RGBA;
					internal = (info.depth == 16) ? GL_RGBA4 : GL_RGBA8;
					break;
			}
			this->name = TextureManager::Instance().create(internal, this->width, this->height, format, data);
			break;
	}
	ImageLoader::release(data);
}

unsigned int TextureAtlas::define(const int x, const int y, const int w, const int h)
{
	R_ASSERT(x >= 0 && (x + w) <= this->width && y >= 0 && (y + h) <= this->height,
	         "Invalid dimensions");

	const float x0 = x / static_cast<float>(this->width);
	const float x1 = (x + w) / static_cast<float>(this->width);
	const float y0 = y / static_cast<float>(this->height);
	const float y1 = (y + h) / static_cast<float>(this->height);

	const size_t i = this->textures.size();
	this->textures.push_back(Texture(x0, y0, x1, y1));
	this->textures[i].atlas = this->name;
	return i;
}

bool TextureAtlas::is_valid(const unsigned int i)
{
	return (i < 64) ? false : (i & 0x03) == 0;
}
