// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_CORESCRIPT_H_
#define DOTNET_CORESCRIPT_H_

#include "DotNet/Core.h"
#include "DotNet/SharedLibrary.h"
#include "Input/InputListener.h"
#include "Script/GameBase.h"

class CoreScript final : public GameBase, public InputListener
{
public:
    enum class Requirement
    {
        Optional,
        Required,
    };

    CoreScript(rainbow::Director&);
    ~CoreScript() override;

    // Implement GameBase.

    void init(const Vec2i& screen) override;
    void update(unsigned long dt) override;

    void on_memory_warning() override;

private:
    using InitFuncPtr = void (*)(int32_t width, int32_t height);
    using UpdateFuncPtr = void (*)(uint64_t dt);
    using OnMemoryWarningFuncPtr = void (*)();

    UpdateFuncPtr update_;
    OnMemoryWarningFuncPtr on_memory_warning_;
    InitFuncPtr init_;
    rainbow::SharedLibrary core_clr_;
    coreclr::ShutdownFunction shutdown_;
    void* host_handle_;
    unsigned int domain_id_;

    // Implement InputListener.

    bool on_key_down_impl(const rainbow::KeyStroke&) override;
    bool on_key_up_impl(const rainbow::KeyStroke&) override;

    bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
    bool on_pointer_canceled_impl() override;
    bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
    bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
};

#endif
