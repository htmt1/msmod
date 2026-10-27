#pragma once
#include <cstdint>

namespace gameui {
// This Unity build uses pointer-sized GC handles, not the older uint32_t API.
// Keep the full handle at every boundary, including widget keys and saved focus.
using GcHandle = uintptr_t;
struct RootKey { GcHandle handle = 0; };

struct GcHandles {
    GcHandle (*create)(void*, bool) = nullptr;
    void* (*get)(GcHandle) = nullptr;
    void (*free)(GcHandle) = nullptr;

    GcHandle retain(void* object) const { return object ? create(object, false) : 0; }
    void* target(GcHandle handle) const { return handle ? get(handle) : nullptr; }
    void release(GcHandle handle) const { if (handle) free(handle); }
};
static_assert(sizeof(GcHandle) == sizeof(void*));
static_assert(sizeof(RootKey) == sizeof(void*));
}
