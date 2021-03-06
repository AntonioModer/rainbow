// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Sprite.h"

#include "Graphics/SpriteBatch.h"
#include "Math/Transform.h"

namespace
{
    constexpr unsigned int kStaleBuffer     = 1u << 0;
    constexpr unsigned int kStalePosition   = 1u << 1;
    constexpr unsigned int kStaleTexture    = 1u << 2;
    constexpr unsigned int kStaleNormalMap  = 1u << 3;
    constexpr unsigned int kStaleMask       = 0xffffu;
    constexpr unsigned int kIsHidden        = 1u << 16;
    constexpr unsigned int kIsFlipped       = 1u << 17;
    constexpr unsigned int kIsMirrored      = 1u << 18;

    const unsigned int kFlipTable[]{
        0, 1, 2, 3,   // Normal
        3, 2, 1, 0,   // Flipped
        1, 0, 3, 2,   // Mirrored
        2, 3, 0, 1};  // Flipped + mirrored
    constexpr unsigned int kFlipVertically = 4;
    constexpr unsigned int kFlipHorizontally = 8;

    static_assert((kIsFlipped >> 0xf) == kFlipVertically, "");
    static_assert((kIsMirrored >> 0xf) == kFlipHorizontally, "");

    unsigned int flip_index(unsigned int state)
    {
        return ((state & kIsFlipped) >> 0xf) + ((state & kIsMirrored) >> 0xf);
    }
}

Sprite& SpriteRef::operator*() const
{
    return batch_->sprites()[i_];
}

Sprite* SpriteRef::operator->() const
{
    return &batch_->sprites()[i_];
}

Sprite::Sprite(Sprite&& s)
    : state_(s.state_ | kStaleMask), center_(s.center_), position_(s.position_),
      texture_(s.texture_), color_(s.color_), width_(s.width_),
      height_(s.height_), angle_(s.angle_), pivot_(s.pivot_), scale_(s.scale_),
      normal_map_(s.normal_map_), id_(s.id_), vertex_array_(s.vertex_array_)
{
    s.id_ = kNoId;
    s.vertex_array_ = nullptr;
}

auto Sprite::is_flipped() const -> bool
{
    return (state_ & kIsFlipped) == kIsFlipped;
}

auto Sprite::is_hidden() const -> bool
{
    return (state_ & kIsHidden) == kIsHidden;
}

auto Sprite::is_mirrored() const -> bool
{
    return (state_ & kIsMirrored) == kIsMirrored;
}

void Sprite::set_color(Colorb c)
{
    state_ |= kStaleTexture;
    color_ = c;
}

void Sprite::set_normal(unsigned int id)
{
    state_ |= kStaleNormalMap;
    normal_map_ = id;
}

void Sprite::set_pivot(const Vec2f& pivot)
{
    R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f &&
             pivot.y >= 0.0f && pivot.y <= 1.0f,
             "Invalid pivot point");

    Vec2f diff = pivot;
    diff -= pivot_;
    if (diff.is_zero())
        return;

    diff.x *= width_ * scale_.x;
    diff.y *= height_ * scale_.y;
    center_ += diff;
    position_ += diff;
    pivot_ = pivot;
}

void Sprite::set_position(const Vec2f& position)
{
    state_ |= kStalePosition;
    position_ = position;
}

void Sprite::set_rotation(float r)
{
    state_ |= kStaleBuffer;
    angle_ = r;
}

void Sprite::set_scale(const Vec2f& f)
{
    R_ASSERT(f.x > 0.0f && f.y > 0.0f,
             "Can't scale with a factor of zero or less");

    state_ |= kStaleBuffer;
    scale_ = f;
}

void Sprite::set_texture(unsigned int id)
{
    state_ |= kStaleTexture;
    texture_ = id;
}

void Sprite::flip()
{
    state_ ^= kIsFlipped;
    state_ |= kStaleTexture;
}

void Sprite::hide()
{
    if (is_hidden())
        return;

    state_ |= kIsHidden | kStaleMask;
}

void Sprite::mirror()
{
    state_ ^= kIsMirrored;
    state_ |= kStaleTexture;
}

void Sprite::move(const Vec2f& delta)
{
    state_ |= kStalePosition;
    position_ += delta;
}

void Sprite::rotate(float r)
{
    state_ |= kStaleBuffer;
    angle_ += r;
}

void Sprite::show()
{
    if (!is_hidden())
        return;

    state_ &= ~kIsHidden;
    state_ |= kStaleMask;
}

auto Sprite::update(ArraySpan<SpriteVertex> vertex_array,
                    const TextureAtlas& texture) -> bool
{
    if ((state_ & kStaleMask) == 0)
        return false;

    if (is_hidden())
    {
        vertex_array[0].position = Vec2f::Zero;
        vertex_array[1].position = Vec2f::Zero;
        vertex_array[2].position = Vec2f::Zero;
        vertex_array[3].position = Vec2f::Zero;
        state_ &= ~kStaleMask;
        return true;
    }

    if (state_ & kStaleBuffer)
    {
        if (state_ & kStalePosition)
            center_ = position_;

        rainbow::transform(*this, vertex_array);
    }
    else if (state_ & kStalePosition)
    {
        position_ -= center_;
        vertex_array[0].position += position_;
        vertex_array[1].position += position_;
        vertex_array[2].position += position_;
        vertex_array[3].position += position_;
        center_ += position_;
        position_ = center_;
    }

    if (state_ & kStaleTexture)
    {
        const unsigned int f = flip_index(state_);
        const auto& tx = texture[texture_];
        for (unsigned int i = 0; i < 4; ++i)
        {
            vertex_array[kFlipTable[f + i]].color = color_;
            vertex_array[kFlipTable[f + i]].texcoord = tx.vx[i];
        }
    }

    state_ &= ~kStaleMask;
    vertex_array_ = vertex_array.data();
    return true;
}

auto Sprite::update(ArraySpan<Vec2f> normal_array, const TextureAtlas& normal)
    -> bool
{
    if ((state_ & kStaleNormalMap) == 0)
        return false;

    state_ ^= kStaleNormalMap;

    const unsigned int f = flip_index(state_);
    const auto& tx = normal[normal_map_];
    for (unsigned int i = 0; i < 4; ++i)
        normal_array[kFlipTable[f + i]] = tx.vx[i];
    return true;
}

Sprite& Sprite::operator=(Sprite&& s)
{
    state_ = s.state_ | kStaleMask;
    center_ = s.center_;
    position_ = s.position_;
    texture_ = s.texture_;
    color_ = s.color_;
    width_ = s.width_;
    height_ = s.height_;
    angle_ = s.angle_;
    pivot_ = s.pivot_;
    scale_ = s.scale_;
    normal_map_ = s.normal_map_;
    id_ = s.id_;
    vertex_array_ = s.vertex_array_;

    s.id_ = kNoId;
    s.vertex_array_ = nullptr;

    return *this;
}
