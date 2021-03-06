// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERMANAGER_H_
#define GRAPHICS_SHADERMANAGER_H_

#include <vector>

#include "Common/Global.h"
#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"
#include "Math/Vec2.h"

namespace rainbow
{
    namespace graphics
    {
        struct State;
    }

    struct ISolemnlySwearThatIAmOnlyTesting;
}

class ShaderManager : public Global<ShaderManager>
{
public:
    enum
    {
        kInvalidProgram,
        kDefaultProgram
    };

    /// <summary>
    ///   Saves the current shader context and restores it when exiting scope.
    /// </summary>
    class Context
    {
    public:
        Context() : program_(ShaderManager::Get()->current_) {}
        ~Context() { ShaderManager::Get()->use(program_); }

    private:
        unsigned int program_;
    };

    ShaderManager(const rainbow::ISolemnlySwearThatIAmOnlyTesting&)
        : ShaderManager()
    {
        make_global();
    }

    ~ShaderManager();

    /// <summary>Compiles program.</summary>
    /// <param name="shaders">Shader parameters.</param>
    /// <param name="attributes">Shader attributes.</param>
    /// <returns>
    ///   Unique program identifier; <c>kInvalidProgram</c> if unsuccessful.
    /// </returns>
    unsigned int compile(Shader::Params* shaders,
                         const Shader::AttributeParams* attributes);

    /// <summary>Returns current program details.</summary>
    const Shader::Details& get_program() const
    {
        R_ASSERT(current_ > 0, "ShaderManager is uninitialised");
        return programs_[current_ - 1];
    }

    /// <summary>Returns program details.</summary>
    Shader::Details& get_program(unsigned int pid)
    {
        R_ASSERT(pid > 0, "Invalid shader program id");
        return programs_[pid - 1];
    }

    /// <summary>Updates orthographic projection.</summary>
    void update_projection();

    /// <summary>Updates viewport.</summary>
    void update_viewport();

    /// <summary>Activates program.</summary>
    void use(unsigned int program);

private:
    unsigned int current_;                   ///< Currently used program.
    std::vector<Shader::Details> programs_;  ///< Linked shader programs.
    std::vector<unsigned int> shaders_;      ///< Compiled shaders.

    ShaderManager() : current_(kInvalidProgram) {}

    bool init();

    friend rainbow::graphics::State;
};

#endif
