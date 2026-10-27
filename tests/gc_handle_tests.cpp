#include "../src/gc_handles.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <vector>

namespace {
struct Slot { void* object = nullptr; };
std::array<Slot, 4> slots;
unsigned created = 0, released = 0, lookedUp = 0;
gameui::GcHandle allocate(void* object, bool pinned) {
    assert(!pinned);
    for (auto& slot : slots) if (!slot.object) {
        slot.object = object;
        ++created;
        auto handle = reinterpret_cast<uintptr_t>(&slot);
        assert(handle > UINT32_MAX); // Reproduce the game's pointer-backed handles.
        return handle;
    }
    assert(false);
    return 0;
}
Slot& resolve(gameui::GcHandle handle) {
    for (auto& slot : slots) if (handle == reinterpret_cast<uintptr_t>(&slot)) return slot;
    assert(false && "handle was truncated or corrupted");
    return slots[0];
}
void* target(gameui::GcHandle handle) { ++lookedUp; return resolve(handle).object; }
void release(gameui::GcHandle handle) { assert(resolve(handle).object); resolve(handle).object = nullptr; ++released; }
}

int main() {
    gameui::GcHandles api{allocate, target, release};
    assert(api.retain(nullptr) == 0 && api.target(0) == nullptr);
    api.release(0);
    assert(created == 0 && released == 0 && lookedUp == 0);

    int widget = 1, focus = 2, eventSystem = 3;
    std::vector<gameui::RootKey> retained{
        {api.retain(&widget)}, {api.retain(&focus)}, {api.retain(&eventSystem)}
    };
    std::vector<gameui::GcHandle> roots;
    for (auto key : retained) roots.push_back(key.handle);
    auto savedFocus = retained[1];
    assert(api.target(retained[0].handle) == &widget);
    assert(api.target(savedFocus.handle) == &focus);
    assert(api.target(retained[2].handle) == &eventSystem);
    for (auto handle : roots) api.release(handle);
    assert(created == 3 && released == 3);
    for (auto& slot : slots) assert(!slot.object);
    std::cout << "PASS: 64-bit GC handles survive UI keys, retained roots, saved focus, lookup and release; null handles make no runtime calls\n";
}
