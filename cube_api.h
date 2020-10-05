#pragma once
#include "natives.h"
#include <typeinfo>

template<class T>
int NativeInvoke(const T& val) // calls native by struct name
{
    static T global_value;
    global_value = val;
    return native_invoke(typeid(T).name(), &global_value, sizeof(T));
}

template<class T>
int NativeInvoke(T& val) // with return args
{
    static T global_value;
    global_value = val;
    int res = native_invoke(typeid(T).name(), &global_value, sizeof(T));
    val = global_value;
    return res;
}

