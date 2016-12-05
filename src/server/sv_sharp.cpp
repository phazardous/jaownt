#include "server.h"
#include "sharp/sharp_public.hpp"

struct sharpsv_callset {
	
	scriptcon_classh ib_c = nullptr;
	scriptcon_method m_frame = nullptr;
	
	scriptcon_classh ob_c = nullptr;
};

static scriptcon_handle sch = nullptr;
static sharpsv_callset cs;

#define SHARP_SERVER_DLL "sharpsv.dll"
#define SHARP_SERVER_NAMESPACE "SHARPSV"
#define SHARP_SERVER_API_IB_CLASS "SVAPI_IN"
#define SHARP_SERVER_API_OB_CLASS "SVAPI_OUT"

#define SHARPSV_RESOLVE_METHOD(m, n, c) cs.m = Sharp_Context_ResolveM(cs.ib_c, #n, c); if (!cs.m) Com_Printf("WARNING: SV_Sharp_Init: SV_API::" #n " not found in " SHARP_SERVER_DLL "\n");
#define SHARPSV_DEFINE_INTERNAL(name, call) Sharp_Context_Define_Internal_Call( sch, SHARP_SERVER_NAMESPACE "." SHARP_SERVER_API_OB_CLASS "::" #name , reinterpret_cast<void *>(call) );

static void SV_Sharp_OB_Print(scriptcon_string strh) {
	Com_Printf(va("%s", Sharp_Context_Unbox_String(strh).c_str()));
}

void SV_Sharp_Init() {
	Com_Printf("================================\n");
	Com_Printf("Initializing Serverside Sharp\n");
	SV_Sharp_Shutdown();
	
	sch = Sharp_Context_Create(SHARP_SERVER_DLL);
	if (!sch) Com_Printf("ERROR: SV_Sharp_Init: could not find/load " SHARP_SERVER_DLL ", scripting will NOT work without it!\n");
	
	cs.ib_c = Sharp_Context_ResolveC(sch, SHARP_SERVER_NAMESPACE, SHARP_SERVER_API_IB_CLASS);
	if (!cs.ib_c) Com_Printf("ERROR: SV_Sharp_Init: loaded " SHARP_SERVER_DLL " does not contain the inbound API class (" SHARP_SERVER_API_IB_CLASS "), scripts cannot be executed without this!\n");
	else {
		SHARPSV_RESOLVE_METHOD(m_frame, Event_Frame, 1)
	}
	
	cs.ob_c = Sharp_Context_ResolveC(sch, SHARP_SERVER_NAMESPACE, SHARP_SERVER_API_OB_CLASS);
	if (!cs.ob_c) Com_Printf("ERROR: SV_Sharp_Init: loaded " SHARP_SERVER_DLL " does not contain the outbound API class (" SHARP_SERVER_API_OB_CLASS "), outbound calls will likely segfault!\n");
	else {
		SHARPSV_DEFINE_INTERNAL(PrintStr, SV_Sharp_OB_Print);
	}
	
	Com_Printf("================================\n");
}

void SV_Sharp_Shutdown() {
	cs = {};
	if (sch) Sharp_Context_Destroy(sch);
	sch = nullptr;
}

void SV_Sharp_Frame() {
	if (cs.m_frame) {
		std::string err;
		void * args [1];
		args[0] = &sv.time;
		Sharp_Context_Invoke(sch, cs.m_frame, args, err);
		if (err.length()) Com_Printf(va("WARNING: SV_Sharp_Frame: Script Exception: %s\n", err.c_str()));
	}
}
