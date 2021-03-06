// Provides algorithms and mathematical methods.
//
// \see http://graphics.stanford.edu/~seander/bithacks.html
//
// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_ALGORITHM_H_
#define COMMON_ALGORITHM_H_

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <utility>

#include "Common/Constants.h"
#include "Common/Constraints.h"
#include "Platform/Macros.h"  // For std::rbegin/rend on Android.

namespace rainbow
{
    template <typename T, size_t N>
    constexpr size_t array_size(const T (&)[N])
    {
        return N;
    }

    /// <summary>Rounds up to the nearest power of 2.</summary>
    /// <remarks>Note that 0 is incorrectly considered a power of 2.</remarks>
    inline unsigned int ceil_pow2(unsigned int i)
    {
        --i;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return ++i;
    }

    template <typename T>
    T clamp(T x, T min_val, T max_val)
    {
        return std::min(std::max(x, min_val), max_val);
    }

    /// <summary>Converts radians to degrees.</summary>
    inline float degrees(float r)
    {
        return r * static_cast<float>(kRadian);
    }

    /// <summary>Fast inverse square root by 0x5f3759df.</summary>
    inline float fast_invsqrt(float x)
    {
        float xhalf = x * 0.5f;
#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
        int i = *reinterpret_cast<int*>(&x);
        i = 0x5f3759df - (i >> 1);
        x = *reinterpret_cast<float*>(&i);
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif
        return x * (1.5f - (xhalf * x * x));
    }

    /// <summary>Rounds down to the nearest power of 2.</summary>
    inline unsigned int floor_pow2(unsigned int i)
    {
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return i - (i >> 1);
    }

    /// <summary>
    ///   Compares two floating point numbers and approximate their equality.
    /// </summary>
    /// <returns><c>true</c> when approximately equal.</returns>
    template <typename T, typename = FloatingPoint<T>>
    bool is_equal(T a, T b)
    {
        return fabs(a - b) <=
               fmax(fabs(a), fabs(b)) * std::numeric_limits<T>::epsilon();
    }

    /// <summary>Determines whether an integer is a power of 2.</summary>
    inline bool is_pow2(unsigned int i)
    {
        return i && !(i & (i - 1));
    }

    /// <summary>Low-pass filter.</summary>
    inline float low_pass(float value, float low_pass)
    {
        return kLowPassAlpha * powf(10.0f, value * kLowPassAlpha) +
               (1.0f - kLowPassAlpha) * low_pass;
    }

    /// <summary>
    ///   Removes the element at <paramref name="pos"/> by swapping it with the
    ///   last element in <paramref name="container"/> and popping it.
    /// </summary>
    template <typename T>
    void quick_erase(T& container, typename T::iterator pos)
    {
        std::swap(*pos, container.back());
        container.pop_back();
    }

    /// <summary>Converts degrees to radians.</summary>
    inline float radians(float d)
    {
        return d * static_cast<float>(kDegree);
    }

    /// <summary>
    ///   Removes the first element equal to <paramref name="val"/>.
    /// </summary>
    template <typename T>
    auto remove(T& container, const typename T::value_type& val)
    {
        auto i = std::find(std::begin(container), std::end(container), val);
        return (i == std::end(container) ? i : container.erase(i));
    }

    /// <summary>
    ///   Removes all elements that satisfy <paramref name="predicate"/> and
    ///   returns a past-the-end iterator for the new end of the range.
    /// </summary>
    template <typename T, typename F>
    auto remove_if(T& container, F&& predicate)
    {
        return container.erase(std::remove_if(std::begin(container),
                                              std::end(container),
                                              std::forward<F>(predicate)),
                               std::end(container));
    }

    /// <summary>Performs a left rotation on the container.</summary>
    template <typename T>
    void rotate_left(T& container)
    {
        auto begin = std::begin(container);
        std::rotate(begin, begin + 1, std::end(container));
    }

    /// <summary>Performs a right rotation on the container.</summary>
    template <typename T>
    void rotate_right(T& container)
    {
        auto rbegin = std::rbegin(container);
        std::rotate(rbegin, rbegin + 1, std::rend(container));
    }

    /// <summary>
    ///   Returns -1, 0, or 1 for <paramref name="x"/> < 0,
    ///   <paramref name="x"/> == 0, or <paramref name="x"/> > 0 respectively.
    /// </summary>
    template <typename T>
    T signum(T x)
    {
        return (x > T(0)) - (x < T(0));
    }

    template <typename Enum>
    constexpr auto to_underlying_type(Enum value)
    {
        return static_cast<std::underlying_type_t<Enum>>(value);
    }
}

#endif
