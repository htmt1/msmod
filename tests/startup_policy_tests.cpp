#include "../src/startup_policy.hpp"
#include <cassert>
#include <iostream>
int main(){
    using wallet::startupCallback;
    assert(startupCallback("Awake","Assembly-CSharp.dll"));
    assert(startupCallback("OnEnable","Assembly-CSharp.dll"));
    assert(startupCallback("Start","Assembly-CSharp.dll"));
    assert(startupCallback("Update","Assembly-CSharp.dll"));
    assert(startupCallback("Internal_ActiveSceneChanged","UnityEngine.CoreModule.dll"));
    assert(!startupCallback(".cctor","Assembly-CSharp.dll"));
    assert(!startupCallback("Update","SomeOtherPlugin.dll"));
    assert(!startupCallback("Internal_ActiveSceneChanged","SomeOtherPlugin.dll"));
    assert(!startupCallback(nullptr,"Assembly-CSharp.dll"));
    assert(!startupCallback("Awake",nullptr));
    std::cout<<"PASS: startup callback filter excludes static constructors and unrelated/unknown images\n";
}
