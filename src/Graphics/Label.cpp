// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"

#include <cstring>

#include "Common/Algorithm.h"
#include "Common/UTF8.h"

using rainbow::is_equal;

namespace
{
	const float kAlignmentFactor[]{0.0f, 1.0f, 0.5f};
}

Label::Label()
    : size_(0), scale_(1.0f), alignment_(TextAlignment::Left), angle_(0.0f),
      count_(0), stale_(0), width_(0)
{
	array_.reconfigure([this] { buffer_.bind(); });
}

void Label::set_alignment(const TextAlignment a)
{
	alignment_ = a;
	set_needs_update(kStaleBuffer);
}

void Label::set_color(const Colorb c)
{
	color_ = c;
	set_needs_update(kStaleColor);
}

void Label::set_font(SharedPtr<FontAtlas> f)
{
	font_ = std::move(f);
	set_needs_update(kStaleBuffer);
}

void Label::set_position(const Vec2f &position)
{
	position_.x = std::round(position.x);
	position_.y = std::round(position.y);
	set_needs_update(kStaleBuffer);
}

void Label::set_rotation(const float r)
{
	if (is_equal(r, angle_))
		return;

	angle_ = r;
	set_needs_update(kStaleBuffer);
}

void Label::set_scale(const float f)
{
	if (is_equal(f, scale_))
		return;

	scale_ = rainbow::clamp(f, 0.01f, 1.0f);
	set_needs_update(kStaleBuffer);
}

void Label::set_text(const char *text)
{
	const size_t len = strlen(text);
	if (len > size_)
	{
		text_.reset(new char[len + 1]);
		size_ = len;
		set_needs_update(kStaleBufferSize);
	}
	std::copy_n(text, len, text_.get());
	text_[len] = '\0';
	set_needs_update(kStaleBuffer);
}

void Label::move(const Vec2f &delta)
{
	position_ += delta;
	set_needs_update(kStaleBuffer);
}

void Label::update()
{
	if (stale_)
	{
		update_internal();
		upload();
		clear_state();
	}
}

void Label::update_internal()
{
	// Note: This algorithm currently does not support font kerning.
	if (stale_ & kStaleBuffer)
	{
		if (stale_ & kStaleBufferSize)
			vertices_.reset(new SpriteVertex[size_ * 4]);
		width_ = 0;
		unsigned int start = 0;
		unsigned int count = 0;
		const bool is_rotated = !is_equal(angle_, 0.0f);
		const Vec2f R = is_rotated ? Vec2f(cos(-angle_), sin(-angle_))
		                           : Vec2f(1.0f, 0.0f);
		const bool needs_alignment =
		    alignment_ != TextAlignment::Left || is_rotated;
		Vec2f pen = needs_alignment ? Vec2f(0.0f, 0.0f) : position_;
		const float origin_x = pen.x;
		SpriteVertex *vx = vertices_.get();
		const auto *text = reinterpret_cast<unsigned char*>(text_.get());
		while (*text)
		{
			if (*text == '\n')
			{
				save(start, count, pen.x - origin_x, R, needs_alignment);
				pen.x = origin_x;
				start = count;
				pen.y -= font_->height() * scale_;
				++text;
				continue;
			}

			const auto &c = rainbow::utf8_decode(text);
			if (c.bytes == 0)
				break;
			text += c.bytes;

			const FontGlyph *glyph = font_->get_glyph(c);
			if (!glyph)
				continue;

			pen.x += glyph->left * scale_;

			for (size_t i = 0; i < 4; ++i)
			{
				vx->color = color_;
				vx->texcoord = glyph->quad[i].texcoord;
				vx->position = glyph->quad[i].position;
				vx->position *= scale_;
				vx->position += pen;
				++vx;
			}

			pen.x += (glyph->advance - glyph->left) * scale_;
			++count;
		}
		count_ = count * 4;
		save(start, count, pen.x - origin_x, R, needs_alignment);
	}
	else if (stale_ & kStaleColor)
	{
		const Colorb &color = color_;
		std::for_each(vertices_.get(),
		              vertices_.get() + count_,
		              [color](SpriteVertex &v) {
			v.color = color;
		});
	}
}

void Label::upload() const
{
	buffer_.upload(vertices_.get(), count_ * sizeof(vertices_[0]));
}

void Label::save(const unsigned int start,
                 const unsigned int end,
                 const float width,
                 const Vec2f &R,
                 const bool needs_alignment)
{
	if (needs_alignment)
	{
		const float offset =
		    width * kAlignmentFactor[static_cast<int>(alignment_)];
		const auto &translate = position_;
		std::for_each(vertices_.get() + start * 4,
		              vertices_.get() + end * 4,
		              [offset, &R, &translate](SpriteVertex &v) {
			auto &p = v.position;
			p = Vec2f(R.x * (p.x - offset) - R.y * p.y + translate.x,
			          R.y * (p.x - offset) + R.x * p.y + translate.y);
		});
	}
	if (width > width_)
		width_ = width;
}
