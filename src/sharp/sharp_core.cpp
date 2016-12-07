#include "sharp_public.hpp"
#include "qcommon/qcommon.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/threads.h>

#include <unordered_set>

static MonoDomain * domain = nullptr;

void Sharp_Init() {
	mono_config_parse (nullptr);
	domain = mono_jit_init_version("JAOWNT", "v4.0");
	if (!domain) Com_Error(ERR_FATAL, "Mono Scripting Engine failed to initialize!\n");
}

void Sharp_Shutdown() {
	if (domain) {
		mono_jit_cleanup(domain);
		domain = nullptr;
	}
}

struct mono_scriptcon {
	MonoDomain * domain;
	MonoAssembly * assembly;
	MonoImage * image;
};

std::unordered_set<mono_scriptcon *> active_contexts;

scriptcon_handle Sharp_Context_Create (char const * assemblyLocation) {
	
	mono_scriptcon * sc = new mono_scriptcon;
	
	fileHandle_t fh;
	int len = FS_SV_FOpenFileRead(assemblyLocation, &fh);
	if (!fh) {
		Com_Printf("ERROR: Sharp_Context_Create: could not find assembly file!");
		return nullptr;
	}
	
	sc->domain = mono_domain_create_appdomain(va("%s", assemblyLocation), nullptr);
	mono_domain_set(sc->domain, true);
	
	void * asmBuf = S_Malloc(len);
	FS_Read(asmBuf, len, fh);
	
	MonoImageOpenStatus mis;
	sc->image = mono_image_open_from_data( reinterpret_cast<char *>(asmBuf), len, true, &mis);
	if (mis != MONO_IMAGE_OK) {
		Com_Error(ERR_FATAL, "Invalid assembly requested!");
	}
	
	sc->assembly = mono_assembly_load_from(sc->image, assemblyLocation, &mis);
	if (mis != MONO_IMAGE_OK) {
		Com_Error(ERR_FATAL, "Assembly could not be created from image!");
	}
	
	FS_FCloseFile(fh);
	active_contexts.insert(sc);
	return sc;
}

void Sharp_Context_Destroy (scriptcon_handle sh) {
	
	mono_scriptcon * sc = reinterpret_cast<mono_scriptcon *>(sh);
	if (active_contexts.erase(sc)) {
		mono_domain_set(domain, true);
		mono_domain_unload(sc->domain);
		delete sc;
	}
}

scriptcon_classh Sharp_Context_ResolveC (scriptcon_handle sh, char const * name_space, char const * name) {
	
	return mono_class_from_name(reinterpret_cast<mono_scriptcon *>(sh)->image, name_space, name);
}

scriptcon_method Sharp_Context_ResolveM (scriptcon_classh ch, char const * name, int arg_c) {
	
	return mono_class_get_method_from_name(reinterpret_cast<MonoClass *>(ch), name, arg_c);
}

void * Sharp_Context_Invoke (scriptcon_handle sh, scriptcon_method mh, void * * arg, std::string & err) {
	
	MonoMethod * mm = reinterpret_cast<MonoMethod *>(mh);
	MonoObject * exc;
	MonoObject * ret = mono_runtime_invoke(mm, nullptr, arg, &exc);
	if (exc) {
		char * dat = mono_string_to_utf8(mono_object_to_string(exc, NULL));
		err = dat;
		//mono_free(dat);
		return nullptr;
	}
	if (!mono_type_is_void(mono_signature_get_return_type(mono_method_signature(mm)))) {
		return mono_object_unbox(ret);
	}
	
	return nullptr;
}

void Sharp_Context_Define_Internal_Call(scriptcon_handle sh, char const * name, void * call) {
	
	mono_add_internal_call(name, call);
}

void Sharp_Context_Begin_Session(scriptcon_handle sh) {
	
	mono_scriptcon * sc = reinterpret_cast<mono_scriptcon *>(sh);
	mono_domain_set(sc->domain, true);
}


scriptcon_string Sharp_Context_Create_String(scriptcon_handle sh, char const * chars) {
	
	return mono_string_new(reinterpret_cast<mono_scriptcon *>(sh)->domain, chars);
}

std::string Sharp_Context_Unbox_String(scriptcon_string strh) {
	std::string str;
	char * nstr = mono_string_to_utf8(reinterpret_cast<MonoString *>(strh));
	str = nstr;
	mono_free(nstr);
	return str;
}
