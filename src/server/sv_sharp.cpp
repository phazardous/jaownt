#include "server.h"
#include "sharp/sharp_public.hpp"

struct sharpsv_callset {
	
	   sharp_class ib_c = nullptr;
	   sharp_method m_sload = nullptr;
	   sharp_method m_frame = nullptr;
	
	   sharp_class ob_c = nullptr;
};

static sharp_handle sch = nullptr;
static sharpsv_callset cs;

#define SHARP_SERVER_DLL "sharpsv.dll"
#define SHARP_SERVER_NAMESPACE "JAOWNT"
#define SHARP_SERVER_API_CLASS "SVI"
#define SHARP_SERVER_MANIFEST_SUBDIR "sharp"
#define SHARP_SERVER_MANIFEST_EXT "ssm"

#define SHARPSV_RESOLVE_METHOD(m, n, c) cs.m = Sharp_Resolve_Method(cs.ib_c, #n, c); if (!cs.m) Com_Printf("WARNING: SV_Sharp_Init: SV_API::" #n " not found in " SHARP_SERVER_DLL "\n");
#define SHARPSV_DEFINE_INTERNAL(name, call) Sharp_Resolve_Internal( sch, SHARP_SERVER_NAMESPACE "." SHARP_SERVER_API_CLASS "::" #name , reinterpret_cast<void *>(call) );

// ================================================================================================================================
// Script Engine -> Native Engine
// ================================================================================================================================

// ============== //
// ---- FILE ---- //
// ============== //

struct sharpFile_t {
	fileHandle_t handle;
	int length;
};

// C# --- public static extern IntPtr FS_OpenR(string str);
static void * SV_Sharp_OB_FS_OpenR(sharp_string sh) {
	fileHandle_t fh;
	int len = FS_FOpenFileRead(Sharp_Unbox_String(sh).c_str(), &fh, qfalse);
	if (!fh) return nullptr;
	sharpFile_t * scf = new sharpFile_t { fh, len };
	return scf;
}

// C# --- public static extern IntPtr FS_OpenW(string str);
static void * SV_Sharp_OB_FS_OpenW(sharp_string sh) {
	fileHandle_t fh = FS_FOpenFileWrite(Sharp_Unbox_String(sh).c_str());
	if (!fh) return nullptr;
	sharpFile_t * scf = new sharpFile_t { fh, 0 };
	return scf;
}

// C# --- public static extern void FS_Close(IntPtr fh);
static void SV_Sharp_OB_FS_Close(sharpFile_t * fh) {
	FS_FCloseFile(fh->handle);
	delete fh;
}

// C# --- public static extern int FS_Length(IntPtr fh);
static long SV_Sharp_OB_FS_Length(sharpFile_t * fh) {
	return fh->length;
}

// C# --- public static extern int FS_Read(IntPtr fh, IntPtr buf, int len);
static int SV_Sharp_OB_FS_Read(sharpFile_t * fh, void * buffer, int length) {
	return FS_Read(buffer, length, fh->handle);
}

// C# --- public static extern int FS_Write(IntPtr fh, IntPtr buf, int len);
static int SV_Sharp_OB_FS_Write(sharpFile_t * fh, void * buffer, int length) {
	return FS_Write(buffer, length, fh->handle);
}

// =============== //
// ---- PRINT ---- //
// =============== //

// C# --- public static extern void PrintStr(string str);
static void SV_Sharp_OB_Print(sharp_string strh) {
	Com_Printf(va("%s", Sharp_Unbox_String(strh).c_str()));
}

// ================================================================================================================================
// Serverside Sharp Internal
// ================================================================================================================================

void SV_Sharp_Init() {
	Com_Printf("================================\n");
	Com_Printf("Initializing Serverside Sharp\n");
	SV_Sharp_Shutdown();
	
	sch = Sharp_Create(SHARP_SERVER_DLL);
	if (!sch) Com_Printf("ERROR: SV_Sharp_Init: could not find/load " SHARP_SERVER_DLL ", scripting will NOT work without it!\n");
	
	cs.ib_c = Sharp_Resolve_Class(sch, SHARP_SERVER_NAMESPACE, SHARP_SERVER_API_CLASS);
	if (!cs.ib_c) Com_Printf("ERROR: SV_Sharp_Init: loaded " SHARP_SERVER_DLL " does not contain the inbound API class (" SHARP_SERVER_API_CLASS "), scripts cannot be executed without this!\n");
	else {
		SHARPSV_RESOLVE_METHOD(m_sload, Load_Manifest, 1)
		SHARPSV_RESOLVE_METHOD(m_frame, Event_Frame, 1)
	}
	
	cs.ob_c = Sharp_Resolve_Class(sch, SHARP_SERVER_NAMESPACE, SHARP_SERVER_API_CLASS);
	if (!cs.ob_c) Com_Printf("ERROR: SV_Sharp_Init: loaded " SHARP_SERVER_DLL " does not contain the outbound API class (" SHARP_SERVER_API_CLASS "), outbound calls will likely segfault!\n");
	else {
		SHARPSV_DEFINE_INTERNAL(FS_OpenR, SV_Sharp_OB_FS_OpenR);
		SHARPSV_DEFINE_INTERNAL(FS_OpenW, SV_Sharp_OB_FS_OpenW);
		SHARPSV_DEFINE_INTERNAL(FS_Close, SV_Sharp_OB_FS_Close);
		SHARPSV_DEFINE_INTERNAL(FS_Length, SV_Sharp_OB_FS_Length);
		SHARPSV_DEFINE_INTERNAL(FS_Read, SV_Sharp_OB_FS_Read);
		SHARPSV_DEFINE_INTERNAL(FS_Write, SV_Sharp_OB_FS_Write);
		SHARPSV_DEFINE_INTERNAL(PrintStr, SV_Sharp_OB_Print);
	}
	
	Com_Printf("================================\n");
}

void SV_Sharp_Shutdown() {
	cs = {};
	if (sch) Sharp_Destroy(sch);
	sch = nullptr;
}

void SV_Sharp_Begin() {
	   Sharp_Bind(sch);
}

// ================================================================================================================================
// Native Engine -> Script Engine
// ================================================================================================================================

sharpsv_handle SV_Sharp_Load(char const * manifest) {
	
	if (!cs.m_sload) return 0;
	
	char const * filename = va(SHARP_SERVER_MANIFEST_SUBDIR "/%s." SHARP_SERVER_MANIFEST_EXT, manifest);
	Com_Printf("Serverside Sharp Loading: %s (\"%s\")\n", manifest, filename);
	
	fileHandle_t fh;
	long len = FS_FOpenFileRead(filename, &fh, qfalse);
	if (!fh) {
		Com_Printf("ERROR: SV_Sharp_Load: manifest \"%s\" not found!\n", filename);
		return 0;
	}
	
	char * fbuf = (char *) Z_Malloc(len + 1, TAG_GENERAL, qtrue);
	FS_Read(fbuf, len, fh);
	   sharp_string manifs = Sharp_Create_String(sch, fbuf);
	Z_Free(fbuf);
	FS_FCloseFile(fh);
	
	std::string err;
	void * args [1];
	args[0] = manifs;
	void * rhand = Sharp_Invoke(sch, cs.m_sload, args, err);
	if (err.length()) {
		Com_Printf(va("WARNING: SV_Sharp_Load: Script Exception: %s\n", err.c_str()));
		return 0;
	}
	return * reinterpret_cast<sharpsv_handle *>(rhand);
}

void SV_Sharp_Frame() {
	if (cs.m_frame) {
		std::string err;
		void * args [1];
		args[0] = &sv.time;
		      Sharp_Invoke(sch, cs.m_frame, args, err);
		if (err.length()) Com_Printf(va("WARNING: SV_Sharp_Frame: Script Exception: %s\n", err.c_str()));
	}
}
