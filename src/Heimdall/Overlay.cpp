// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/Overlay.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Style.h"

namespace Heimdall
{
	Overlay::~Overlay()
	{
		if (this->texture)
			TextureManager::Instance->remove(this->texture);
	}

	void Overlay::setup(const Vec2i &screen)
	{
		const unsigned char white[4096] = { 0xff };
		this->texture = TextureManager::Instance->create(
				GL_LUMINANCE, 64, 64, GL_LUMINANCE, white);

		this->vertices[0].color       = Color::Overlay();
		this->vertices[0].texcoord    = Vec2f(0.5f, 0.5f);
		this->vertices[1].color       = this->vertices[0].color;
		this->vertices[1].texcoord    = this->vertices[0].texcoord;
		this->vertices[1].position.x  = screen.width;
		this->vertices[2].color       = this->vertices[0].color;
		this->vertices[2].texcoord    = this->vertices[0].texcoord;
		this->vertices[2].position    = Vec2f(screen.width, screen.height);
		this->vertices[3].color       = this->vertices[0].color;
		this->vertices[3].texcoord    = this->vertices[0].texcoord;
		this->vertices[3].position.y  = screen.height;
	}

	void Overlay::draw_impl()
	{
		TextureManager::Instance->bind(this->texture);
		Renderer::draw_elements(this->vertices, 6);
	}

	void Overlay::update_impl() { }
}

#endif
