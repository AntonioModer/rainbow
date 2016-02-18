// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/CoreScript.h"

#include "FileSystem/Path.h"

#define kFailedCoreCLR "Failed to load CoreCLR"
#define kFailedCreateDelegate                                                  \
    "CoreCLR: Failed to load '%s.%s' from assembly '%s' (HRESULT: 0x%x)"
#define kFailedLoadFunction "Failed to load '%s' from '%s'"

using coreclr::CreateDelegateFunction;
using coreclr::InitializeFunction;
using coreclr::ShutdownFunction;
using coreclr::kCreateDelegateFunction;
using coreclr::kInitializeFunction;
using coreclr::kShutdownFunction;

namespace
{
    constexpr const char kApplicationDomain[] = "com.bifrost.rainbow";
    constexpr const char kEntryPointAssembly[] = "rainbow-sharp";
    constexpr const char kEntryPointType[] = "Rainbow.Program";

    constexpr const char kEntryPointMethodInitialize[] = "Initialize";
    constexpr const char kEntryPointMethodOnMemoryWarning[] = "OnMemoryWarning";
    constexpr const char kEntryPointMethodUpdate[] = "Update";

    bool failed(int status) { return status < 0; }

    bool create_delegate(CreateDelegateFunction coreclr_create_delegate,
                         void* host_handle,
                         unsigned int domain_id,
                         const char* method_name,
                         void** delegate,
                         CoreScript::Requirement requirement)
    {
        const int result = coreclr_create_delegate(  //
            host_handle,
            domain_id,
            kEntryPointAssembly,
            kEntryPointType,
            method_name,
            delegate);
        const bool succeeded = !failed(result);
        if (!succeeded)
        {
            if (requirement == CoreScript::Requirement::Optional)
            {
                LOGW(kFailedCreateDelegate,
                     kEntryPointType,
                     method_name,
                     kEntryPointAssembly,
                     result);
            }
            else
            {
                LOGF(kFailedCreateDelegate,
                     kEntryPointType,
                     method_name,
                     kEntryPointAssembly,
                     result);
            }
        }
        return succeeded;
    }
}

CoreScript::CoreScript(rainbow::Director& director)
    : GameBase(director), update_(nullptr), on_memory_warning_(nullptr),
      init_(nullptr), core_clr_(coreclr::core_library_path()),
      shutdown_(nullptr), host_handle_(nullptr), domain_id_(0)
{
    if (!core_clr_)
    {
        LOGF("Failed to load '%s'", coreclr::core_library_path());
        terminate(kFailedCoreCLR);
        return;
    }

    auto coreclr_initialize =
        core_clr_.get_symbol<InitializeFunction>(kInitializeFunction);
    if (coreclr_initialize == nullptr)
    {
        LOGF(kFailedLoadFunction,
             kInitializeFunction,
             coreclr::core_library_path());
        terminate(kFailedCoreCLR);
        return;
    }

    auto coreclr_create_delegate =
        core_clr_.get_symbol<CreateDelegateFunction>(kCreateDelegateFunction);
    if (coreclr_create_delegate == nullptr)
    {
        LOGF(kFailedLoadFunction,
             kCreateDelegateFunction,
             coreclr::core_library_path());
        terminate(kFailedCoreCLR);
        return;
    }

    shutdown_ = core_clr_.get_symbol<ShutdownFunction>(kShutdownFunction);
    if (shutdown_ == nullptr)
    {
        LOGF(kFailedLoadFunction,
             kShutdownFunction,
             coreclr::core_library_path());
        terminate(kFailedCoreCLR);
        return;
    }

    const char* property_keys[]{
        "TRUSTED_PLATFORM_ASSEMBLIES",
        "APP_PATHS",
        "APP_NI_PATHS",
        "NATIVE_DLL_SEARCH_DIRECTORIES",
        "AppDomainCompatSwitch",
    };
    const char* property_values[]{
        coreclr::trusted_platform_assemblies(),
        Path::current(),
        Path::current(),
        coreclr::native_dll_search_directories(),
        "UseLatestBehaviorWhenTFMNotSpecified",  // AppDomainCompatSwitch
    };

    const int result = coreclr_initialize(  //
        Path::executable_path(),
        kApplicationDomain,
        rainbow::array_size(property_keys),
        property_keys,
        property_values,
        &host_handle_,
        &domain_id_);
    if (failed(result))
    {
        shutdown_ = nullptr;
        LOGF("CoreCLR: coreclr_initialize => HRESULT: 0x%x", result);
        terminate("Failed to initialise CoreCLR");
        return;
    }

    if (!create_delegate(coreclr_create_delegate,
                         host_handle_,
                         domain_id_,
                         kEntryPointMethodInitialize,
                         reinterpret_cast<void**>(&init_),
                         Requirement::Required) ||
        !create_delegate(coreclr_create_delegate,
                         host_handle_,
                         domain_id_,
                         kEntryPointMethodUpdate,
                         reinterpret_cast<void**>(&update_),
                         Requirement::Required))
    {
        terminate("Failed to load assembly");
        return;
    }

    create_delegate(coreclr_create_delegate,
                    host_handle_,
                    domain_id_,
                    kEntryPointMethodOnMemoryWarning,
                    reinterpret_cast<void**>(&on_memory_warning_),
                    Requirement::Optional);

    input().subscribe(this);
}

CoreScript::~CoreScript()
{
    if (!shutdown_)
        return;

    shutdown_(host_handle_, domain_id_);
}

void CoreScript::init(const Vec2i& size)
{
    init_(size.x, size.y);
}

void CoreScript::update(unsigned long dt)
{
    update_(dt);
}

void CoreScript::on_memory_warning()
{
    if (on_memory_warning_ == nullptr)
        return;

    on_memory_warning_();
}

bool CoreScript::on_key_down_impl(const rainbow::KeyStroke&)
{
    return true;
}

bool CoreScript::on_key_up_impl(const rainbow::KeyStroke&)
{
    return true;
}

bool CoreScript::on_pointer_began_impl(const ArrayView<Pointer>&)
{
    return true;
}

bool CoreScript::on_pointer_canceled_impl()
{
    return true;
}

bool CoreScript::on_pointer_ended_impl(const ArrayView<Pointer>&)
{
    return true;
}

bool CoreScript::on_pointer_moved_impl(const ArrayView<Pointer>&)
{
    return true;
}

std::unique_ptr<GameBase> GameBase::create(rainbow::Director& director)
{
    return std::make_unique<CoreScript>(director);
}
