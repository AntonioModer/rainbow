// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_CORE_H_
#define DOTNET_CORE_H_

namespace coreclr
{
    constexpr const char kCreateDelegateFunction[] = "coreclr_create_delegate";
    constexpr const char kExecuteAssemblyFunction[] =
        "coreclr_execute_assembly";
    constexpr const char kInitializeFunction[] = "coreclr_initialize";
    constexpr const char kShutdownFunction[] = "coreclr_shutdown";

    /// <summary>
    ///   Creates a native callable delegate for a managed method.
    /// </summary>
    using CreateDelegateFunction =  //
        int (*)(void* host_handle,
                unsigned int domain_id,
                const char* entry_point_assembly_name,
                const char* entry_point_type_name,
                const char* entry_point_method_name,
                void** delegate);

    /// <summary>Executes a managed assembly with given arguments.</summary>
    using ExecuteAssemblyFunction =  //
        int (*)(void* host_handle,
                unsigned int domain_id,
                int argc,
                const char** argv,
                const char* managed_assembly_path,
                unsigned int* exit_code);

    /// <summary>
    ///   Initializes the CoreCLR. Creates and starts CoreCLR host, and creates
    ///   an app domain.
    /// </summary>
    using InitializeFunction =  //
        int (*)(const char* exe_path,
                const char* app_domain_friendly_name,
                int property_count,
                const char** property_keys,
                const char** property_values,
                void** host_handle,
                unsigned int* domain_id);

    /// <summary>
    ///   Shutdowns CoreCLR. Unloads the app domain and stops the CoreCLR host.
    /// </summary>
    using ShutdownFunction = int (*)(void* host_handle, unsigned int domain_id);

    const char* core_library_path();
    const char* native_dll_search_directories();
    const char* trusted_platform_assemblies();
}

#endif
