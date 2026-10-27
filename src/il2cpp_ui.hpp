#pragma once
#include "windows_util.hpp"
#include "gc_handles.hpp"
#include <map>
#include <tuple>
#include <stdexcept>
#include <initializer_list>
namespace gameui {
using Object=void*;using Class=void*;using Method=void*;
struct Vec2{float x,y;};struct Color{float r,g,b,a;};
class Api {
    Object(*domainGet)()=nullptr;
    Object*(*assemblies)(Object,size_t*)=nullptr;
    Object(*assemblyImage)(Object)=nullptr;
    Class(*className)(Object,const char*,const char*)=nullptr;
    Class(*parent)(Class)=nullptr;
    Method(*methodName)(Class,const char*,int)=nullptr;
    uint32_t(*methodFlags)(Method,uint32_t*)=nullptr;
    Method(*virtualMethod)(Object,Method)=nullptr;
    Object(*invoke)(Method,Object,void**,Object*)=nullptr;
    void*(*unbox)(Object)=nullptr;
    Object(*allocate)(Class)=nullptr;
    void(*classInit)(Class)=nullptr;
    Object(*classType)(Class)=nullptr;
    Object(*typeObject)(Object)=nullptr;
    Object(*stringNew)(const wchar_t*,int32_t)=nullptr;
    GcHandles handles;
    Object(*fieldName)(Class,const char*)=nullptr;
    void(*fieldGet)(Object,Object,void*)=nullptr;
    void(*staticGet)(Object,void*)=nullptr;
    void(*formatException)(Object,char*,int)=nullptr;
    std::map<std::pair<std::string,std::string>,Class> classes;
    std::map<std::tuple<Class,std::string,int>,Method> methods;
    template<class T>void bind(HMODULE module,T& target,const char* name){target=reinterpret_cast<T>(GetProcAddress(module,name));if(!target)throw std::runtime_error(std::string("missing IL2CPP export: ")+name);}
public:
    void init(HMODULE module){
        bind(module,domainGet,"il2cpp_domain_get");bind(module,assemblies,"il2cpp_domain_get_assemblies");
        bind(module,assemblyImage,"il2cpp_assembly_get_image");bind(module,className,"il2cpp_class_from_name");bind(module,parent,"il2cpp_class_get_parent");
        bind(module,methodName,"il2cpp_class_get_method_from_name");bind(module,methodFlags,"il2cpp_method_get_flags");bind(module,virtualMethod,"il2cpp_object_get_virtual_method");
        bind(module,invoke,"il2cpp_runtime_invoke");bind(module,unbox,"il2cpp_object_unbox");bind(module,allocate,"il2cpp_object_new");bind(module,classInit,"il2cpp_runtime_class_init");
        bind(module,classType,"il2cpp_class_get_type");bind(module,typeObject,"il2cpp_type_get_object");bind(module,stringNew,"il2cpp_string_new_utf16");
        bind(module,handles.create,"il2cpp_gchandle_new");bind(module,handles.get,"il2cpp_gchandle_get_target");bind(module,handles.free,"il2cpp_gchandle_free");
        bind(module,fieldName,"il2cpp_class_get_field_from_name");bind(module,fieldGet,"il2cpp_field_get_value");bind(module,staticGet,"il2cpp_field_static_get_value");bind(module,formatException,"il2cpp_format_exception");
    }
    Class type(const char* ns,const char* name){
        auto key=std::make_pair(std::string(ns),std::string(name));auto existing=classes.find(key);if(existing!=classes.end())return existing->second;
        size_t count=0;auto list=assemblies(domainGet(),&count);
        for(size_t i=0;i<count;++i){auto found=className(assemblyImage(list[i]),ns,name);if(found){classes.emplace(key,found);return found;}}
        throw std::runtime_error(std::string("UI class unavailable: ")+ns+"."+name);
    }
    Method method(Class klass,const char* name,int count){
        auto key=std::make_tuple(klass,std::string(name),count);auto found=methods.find(key);if(found!=methods.end())return found->second;
        for(auto c=klass;c;c=parent(c)){auto m=methodName(c,name,count);if(m){methods.emplace(key,m);return m;}}
        throw std::runtime_error(std::string("UI method unavailable: ")+name+"/"+std::to_string(count));
    }
    Object call(Class klass,const char* name,Object self,std::initializer_list<void*> args={}){
        auto m=method(klass,name,int(args.size()));uint32_t ignored=0;
        if(self && (methodFlags(m,&ignored)&0x40))m=virtualMethod(self,m);
        std::vector<void*> parameters(args);Object exception=nullptr;
        auto value=invoke(m,self,parameters.empty()?nullptr:parameters.data(),&exception);
        if(exception){char text[2048]{};formatException(exception,text,int(sizeof(text)));throw std::runtime_error(std::string("UI call ")+name+": "+text);}
        return value;
    }
    template<class T>T value(Object boxed){if(!boxed)throw std::runtime_error("UI API returned an empty value");return *static_cast<T*>(unbox(boxed));}
    Object text(const std::wstring& s){return stringNew(s.c_str(),int32_t(s.size()));}
    std::wstring string(Object s){if(!s)return {};auto n=*reinterpret_cast<int32_t*>(uintptr_t(s)+16);if(n<0||n>32768)throw std::runtime_error("invalid UI string length");return std::wstring(reinterpret_cast<wchar_t*>(uintptr_t(s)+20),n);}
    Object create(Class klass){classInit(klass);auto value=allocate(klass);if(!value)throw std::runtime_error("UI allocation failed");call(klass,".ctor",value);return value;}
    Object reflectionType(Class klass){return typeObject(classType(klass));}
    template<class T>T field(Class klass,const char* name,Object self){auto f=fieldName(klass,name);if(!f)throw std::runtime_error(std::string("UI field unavailable: ")+name);T value{};if(self)fieldGet(self,f,&value);else{classInit(klass);staticGet(f,&value);}return value;}
    GcHandle retain(Object object){return handles.retain(object);}
    Object target(GcHandle handle){return handles.target(handle);}
    void release(GcHandle handle){handles.release(handle);}
};
}
