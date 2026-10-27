#include "../src/gc_handles.hpp"

// Test-only DLL. Exercise the production handle API with the Windows x64 ABI.
extern "C" __declspec(dllexport) void* cycle_widget(gameui::GcHandles* api, void* object) {
    gameui::RootKey key{api->retain(object)};
    void* result = api->target(key.handle);
    api->release(key.handle);
    return result;
}
