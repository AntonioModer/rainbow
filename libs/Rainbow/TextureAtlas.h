/// Custom texture atlas implemented in OpenGL ES 1.1.

/// GL drawing order: (1,1) -> (0,1) -> (1,0) -> (0,0)
///
/// Copyright 2010 Ninja Unicorn. All rights reserved.
/// \author Tommy Nguyen

#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include <cstring>

#include <Rainbow/Texture.h>
#include <Rainbow/Types.h>
#include <Rainbow/Vector.h>

class Sprite;

class TextureAtlas
{
public:
	TextureAtlas(const char *filename, const int texture_count, const int sprite_count, const int mipmap = 0);
	~TextureAtlas();

	/// Creates a sprite.
	/// \return A sprite object
	Sprite* create_sprite(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);

	/// Defines a texture from the map.
	/// \param x       x-component of texture location
	/// \param y       y-component of texture location
	/// \param width   Width of the texture
	/// \param height  Height of the texture
	/// \return An id unique to the texture
	unsigned int define_texture(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);

	/// Draws all sprites created from the texture atlas.
	//void draw();

	/// Returns the texture name this atlas holds.
	inline GLuint get_name() { return this->texture.name; }

	/// Gets named texture.
	/// \param id        The id of the texture to get
	/// \param vertices  Interleaved vertex data array (vertex and texture coordinates)
	void get_texture(const unsigned int id, SpriteVertex *vertices);

private:
	Texture texture;           ///< Loaded texture
	Vector<GLfloat> textures;  ///< Stores all textures
	Vector<Sprite *> sprites;  ///< Stores all sprites
};

#endif