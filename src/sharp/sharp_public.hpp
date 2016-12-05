#ifndef MONO_PUBLIC_HPP
#define MONO_PUBLIC_HPP
	
#include "qcommon/q_shared.h"
#include <string>
	
void Sharp_Init();
void Sharp_Shutdown();

typedef void * scriptcon_handle;
typedef void * scriptcon_classh;
typedef void * scriptcon_method;
typedef void * scriptcon_string;

scriptcon_handle Sharp_Context_Create (char const * assemblyLocation);
void Sharp_Context_Destroy (scriptcon_handle);
scriptcon_classh Sharp_Context_ResolveC (scriptcon_handle, char const * name_space, char const * name);
scriptcon_method Sharp_Context_ResolveM (scriptcon_classh, char const * name, int arg_c);
void * Sharp_Context_Invoke (scriptcon_handle, scriptcon_method, void * * arg, std::string & err);
void Sharp_Context_Define_Internal_Call(scriptcon_handle, char const * name, void * call);

std::string Sharp_Context_Unbox_String(scriptcon_string);

#endif
