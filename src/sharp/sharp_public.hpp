#ifndef MONO_PUBLIC_HPP
#define MONO_PUBLIC_HPP
	
#include "qcommon/q_shared.h"
#include <string>
	
void Sharp_Init();
void Sharp_Shutdown();

typedef void * sharp_handle;
typedef void * sharp_class;
typedef void * sharp_method;
typedef void * sharp_string;

sharp_handle Sharp_Create (char const * assemblyLocation);
void Sharp_Destroy (sharp_handle);

sharp_class Sharp_Resolve_Class (sharp_handle, char const * name_space, char const * name);
sharp_method Sharp_Resolve_Method (sharp_class, char const * name, int arg_c);
void Sharp_Resolve_Internal(sharp_handle, char const * name, void * call);

void Sharp_Bind(sharp_handle);
void * Sharp_Invoke (sharp_handle, sharp_method, void * * arg, std::string & err);

sharp_string Sharp_Create_String(sharp_handle sh, char const *);
std::string Sharp_Unbox_String(sharp_string);

#endif
