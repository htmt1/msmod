#pragma once
#include <cstring>
namespace wallet {
// Unity's scene callback or a game MonoBehaviour callback runs after IL2CPP startup.
// Never initialize game hooks from a static constructor or a random worker invocation.
inline bool startupCallback(const char* method,const char* image) {
    if(!method)return false;
    if(std::strcmp(method,"Internal_ActiveSceneChanged")==0)
        return image && (std::strcmp(image,"UnityEngine.CoreModule.dll")==0 || std::strcmp(image,"UnityEngine.dll")==0);
    if(!image || std::strcmp(image,"Assembly-CSharp.dll")!=0)return false;
    return std::strcmp(method,"Awake")==0 || std::strcmp(method,"OnEnable")==0 ||
           std::strcmp(method,"Start")==0 || std::strcmp(method,"Update")==0;
}
}
