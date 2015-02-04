// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

#include "Common/Vec2.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/TextureManager.h"

struct SpriteVertex;

namespace rainbow { class Director; }

class Renderer : public Global<Renderer>
{
	friend rainbow::Director;

public:
	static const size_t kNumSprites = 256;  ///< Hard-coded limit on number of sprites.

	static void clear();

	template<typename T>
	static void draw(const T &);

	template<typename T>
	static void draw_arrays(const T &, const int first, const size_t count);

	static bool has_extension(const char *const extension);
	static int max_texture_size();

	const Vec2i& resolution() const { return view_; }
	const Vec2i& window_size() const { return window_; }

	void set_resolution(const Vec2i &resolution);
	void set_window_size(const Vec2i &size, const float factor = 1.0f);

	void bind_element_array() const;
	Vec2i convert_to_flipped_view(const Vec2i &) const;
	Vec2i convert_to_view(const Vec2i &) const;

private:
	unsigned int index_buffer_;
	Vec2f scale_;
	Vec2i origin_;
	Vec2i view_;
	Vec2i window_;
	TextureManager texture_manager_;
	ShaderManager shader_manager_;

	Renderer();
	~Renderer();

	bool init();
};

template<typename T>
void Renderer::draw(const T &obj)
{
	obj.vertex_array().bind();
	obj.bind_textures();
	glDrawElements(GL_TRIANGLES, obj.count(), GL_UNSIGNED_SHORT, nullptr);
}

template<typename T>
void Renderer::draw_arrays(const T &obj, const int first, const size_t count)
{
	obj.vertex_array().bind();
	obj.bind_textures();
	glDrawArrays(GL_TRIANGLES, first, count);
}

#endif
