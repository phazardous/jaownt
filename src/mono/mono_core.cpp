#include "mono_public.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

static MonoDomain * domain = nullptr;

void mono::init() {
	Com_Printf("================================\n");
	Com_Printf("Initializing Mono Scripting Engine\n----------------\n");
	
	domain = mono_jit_init_version("JAOWNT", "v4.0");
	if (!domain) {
		Com_Error(ERR_FATAL, "Mono Scripting Engine failed to initialize!");
	}
	Com_Printf("jit initialized...\n");
	Com_Printf("================================\n");
}

void mono::term() noexcept {
	Com_Printf("================================\n");
	Com_Printf("Terminating Mono Scripting Engine\n----------------\n");
	if (domain) mono_jit_cleanup(domain);
	domain = nullptr;
	Com_Printf("domain cleaned...\n");
	Com_Printf("================================\n");
}
