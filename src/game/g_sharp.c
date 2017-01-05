#include "g_local.h"
#include "b_local.h"

typedef struct sharp_callset_s {
	
	   sharp_class ib_c;
	   
	   sharp_method m_sload;
	   sharp_method m_sclose;
	   
	   sharp_method m_shutdown;
	   sharp_method m_frame;
	   sharp_method m_chat;
	   sharp_method m_cmd;
	
	   sharp_class ob_c;
} sharp_callset_t;

static sharp_handle shandle = NULL;
static sharp_callset_t cs;

#define SHARP_SERVER_DLL "sharpsv.dll"
#define SHARP_SERVER_NAMESPACE "JAOWNT"
#define SHARP_SERVER_API_CLASS "G_INTERNAL"
#define SHARP_SERVER_MANIFEST_SUBDIR "sharp"
#define SHARP_SERVER_MANIFEST_EXT "ssm"

#define SHARPG_RESOLVE_METHOD(m, n, c) cs.m = trap->Sharp_Resolve_Method(cs.ib_c, #n, c); if (!cs.m) trap->Print("WARNING: G_Sharp_Init: G_API::" #n " not found in " SHARP_SERVER_DLL "\n");
#define SHARPG_DEFINE_INTERNAL(name, call) trap->Sharp_Resolve_Internal( shandle, SHARP_SERVER_NAMESPACE "." SHARP_SERVER_API_CLASS "::" #name , call);

// ================================================================================================================================
// Script Engine -> Native Engine
// ================================================================================================================================

// ================ //
// ---- ENTITY ---- //
// ================ //

// Entity

// C# --- public static extern IntPtr G_CreateEntity();
static gentity_t * G_SharpI_CreateEntity() {
	gentity_t * new_ent = G_Spawn();
	// TODO -- actually do things that need to be done
	return new_ent;
}

// C# --- public static extern void G_DestroyEntity(IntPtr handle);
static void G_SharpI_DestroyEntity(gentity_t * ent) {
	if (!ent) return;
	G_FreeEntity(ent);
}

// C# --- public static extern bool G_IsPlayer(IntPtr handle);
static int G_SharpI_IsPlayer(gentity_t * ent) {
	if (!ent) return 0;
	return ent->playerState != NULL && !ent->NPC;
}

// C# --- public static extern bool G_IsNPC(IntPtr handle);
static int G_SharpI_IsNPC(gentity_t * ent) {
	if (!ent) return 0;
	return ent->NPC != NULL;
}


// C# --- public static extern IntPtr G_Activator(IntPtr ent);
static gentity_t * G_SharpI_Activator (gentity_t * ent) {
	if (!ent) return NULL;
	return ent->activator;
}

// C# --- public static extern IntPtr G_Other(IntPtr ent);
static gentity_t * G_SharpI_Other (gentity_t * ent) {
	if (!ent) return NULL;
	return ent->other;
}

// C# --- public static extern string G_Target(IntPtr ent);
static sharp_string G_SharpI_Target (gentity_t * ent) {
	if (!ent) return NULL;
	return trap->Sharp_Create_String(shandle, ent->target);
}

// C# --- public static extern string G_Targetname(IntPtr ent);
static sharp_string G_SharpI_Targetname (gentity_t * ent) {
	if (!ent) return NULL;
	return trap->Sharp_Create_String(shandle, ent->targetname);
}

// C# --- public static extern IntPtr G_Origin(IntPtr ent);
static float * G_SharpI_Origin(gentity_t * ent) {
	return ent->s.origin;
}

// NPC

// C# --- public static extern void G_NPC_Kill(IntPtr ent);
static void G_SharpI_NPC_Kill (gentity_t * ent) {
	if (!ent || !ent->NPC || !ent->client || !ent->die) return;
	ent->health = 0;
	ent->client->ps.stats[STAT_HEALTH] = 0;
	ent->die(ent, ent, ent, ent->client->pers.maxHealth, MOD_UNKNOWN);
}

// Player

// C# --- public static extern IntPtr G_EntFromPlayer(int clinum);
static gentity_t * G_SharpI_EntFromPlayer(int clinum) {
	return g_entities + clinum;
}

// C# --- public static extern int SVG_PlayerFromEnt(IntPtr handle);
static int G_SharpI_PlayerFromEnt(gentity_t * ent) {
	if (!ent) return 0;
	return ent - g_entities;
}

// C# --- public static extern string SVG_PlayerName(int clinum);
static sharp_string G_SharpI_PlayerName(int clinum) {
	if (clinum >= MAX_CLIENTS || !g_entities[clinum].client) return NULL;
	return trap->Sharp_Create_String(shandle, g_entities[clinum].client->pers.netname);
}

// Special

static gentity_t * G_SharpI_SpawnNPC(sharp_string svnpc_type, vec3_t pos, float yaw, int health) {
	
	gentity_t	*newent = NULL;
	char *		npc_type = Q_strlwr(trap->Sharp_Unbox_String(svnpc_type));
	vec3_t		angles = {0, yaw, 0};
	
	newent = G_Spawn();
	newent->fullName = "Sharp NPC";
	newent->NPC = New_NPC_t(newent->s.number);
	G_CreateFakeClient(newent->s.number, &newent->client);
	newent->NPC->tempGoal = G_Spawn();
	newent->NPC->tempGoal->classname = "NPC_goal";
	newent->NPC->tempGoal->parent = newent;
	newent->NPC->tempGoal->r.svFlags |= SVF_NOCLIENT;
	memset ( newent->client, 0, sizeof(*newent->client) );
	newent->playerState = &newent->client->ps;
	newent->client->ps.weapon = WP_NONE;
	VectorCopy(pos, newent->s.origin);
	VectorCopy(pos, newent->client->ps.origin);
	VectorCopy(pos, newent->r.currentOrigin);
	G_SetOrigin(newent, pos);
	if (!NPC_ParseParms( npc_type, newent )) {
		G_FreeEntity( newent );
		trap->Print("G_SharpI_SpawnNPC: could not parse NPC parms for %s\n", npc_type);
		return NULL;
	}
	if (health != 0) {
		if (health < 1) newent->health = 1 << 30;
		else newent->health = health;
	}
	newent->classname = "NPC";
	newent->NPC_type = G_NewString(npc_type); //FIXME -- This is terrible and I hate it, but this is now "npc spawn" works. soft memory leaking, gj previous devs
	trap->UnlinkEntity((sharedEntity_t *)newent);
	VectorCopy(angles, newent->s.angles);
	VectorCopy(angles, newent->r.currentAngles);
	VectorCopy(angles, newent->client->ps.viewangles);
	newent->NPC->desiredYaw = yaw;
	newent->s.eType	= ET_NPC;
	newent->s.pos.trType = TR_INTERPOLATE;
	newent->s.pos.trTime = level.time;
	VectorCopy( newent->r.currentOrigin, newent->s.pos.trBase );
	VectorClear( newent->s.pos.trDelta );
	newent->s.pos.trDuration = 0;
	newent->s.apos.trType = TR_INTERPOLATE;
	newent->s.apos.trTime = level.time;
	VectorCopy( newent->s.angles, newent->s.apos.trBase );
	VectorClear( newent->s.apos.trDelta );
	newent->s.apos.trDuration = 0;
	newent->NPC->combatPoint = -1;
	newent->flags |= FL_NOTARGET;
	newent->s.eFlags |= EF_NODRAW;
	newent->think = NPC_Begin;
	newent->nextthink = level.time + FRAMETIME;
	NPC_DefaultScriptFlags( newent );
	newent->client->sess.sessionTeam = TEAM_FREE;
	newent->client->ps.persistant[PERS_TEAM] = newent->client->sess.sessionTeam;
	
	G_Phys_AddClientCapsule(newent);
	
	trap->LinkEntity ((sharedEntity_t *)newent);

	return newent;
}
		
// ============== //
// ---- FILE ---- //
// ============== //

typedef struct sharpFile_s {
	fileHandle_t handle;
	int length;
} sharpFile_t;

// C# --- public static extern IntPtr FS_OpenR(string str);
static void * G_SharpI_FS_OpenR(sharp_string sh) {
	fileHandle_t fh;
	int len = trap->FS_Open(trap->Sharp_Unbox_String(sh), &fh, FS_READ);
	if (!fh) return NULL;
	sharpFile_t * scf = malloc(sizeof(sharpFile_t));
	scf->handle = fh;
	scf->length = len;
	return scf;
}

// C# --- public static extern IntPtr FS_OpenW(string str);
static void * G_SharpI_FS_OpenW(sharp_string sh) {
	fileHandle_t fh;
	int len = trap->FS_Open(trap->Sharp_Unbox_String(sh), &fh, FS_WRITE);
	if (!fh) return NULL;
	sharpFile_t * scf = malloc(sizeof(sharpFile_t));
	scf->handle = fh;
	scf->length = len;
	return scf;
}

// C# --- public static extern void FS_Close(IntPtr fh);
static void G_SharpI_FS_Close(sharpFile_t * fh) {
	trap->FS_Close(fh->handle);
	free(fh);
}

// C# --- public static extern int FS_Length(IntPtr fh);
static long G_SharpI_FS_Length(sharpFile_t * fh) {
	return fh->length;
}

// C# --- public static extern int FS_Read(IntPtr fh, IntPtr buf, int len);
static int G_SharpI_FS_Read(sharpFile_t * fh, void * buffer, int length) {
	return trap->FS_Read(buffer, length, fh->handle);
}

// C# --- public static extern int FS_Write(IntPtr fh, IntPtr buf, int len);
static int G_SharpI_FS_Write(sharpFile_t * fh, void * buffer, int length) {
	return trap->FS_Write(buffer, length, fh->handle);
}

// =============== //
// ---- PRINT ---- //
// =============== //

// C# --- public static extern void PrintStr(string str);
static void G_SharpI_Print(sharp_string strh) {
	trap->Print(trap->Sharp_Unbox_String(strh));
}

// C# --- public static extern void ChatAll(string name, string msg);
static void G_SharpI_ChatAll(sharp_string name, sharp_string msg) {
	G_Say(NULL, NULL, SAY_ALL, trap->Sharp_Unbox_String(msg), trap->Sharp_Unbox_String(name));
}

// C# --- public static extern void ChatTo(string name, string msg, int clinum);
static void G_SharpI_ChatTo(sharp_string name, sharp_string msg, int clinum) {
	G_Say(NULL, g_entities + clinum, SAY_TELL, trap->Sharp_Unbox_String(msg), trap->Sharp_Unbox_String(name));
}

// ================ //
// ---- SEARCH ---- //
// ================ //

// C# --- public static extern IntPtr[] G_FindEntitiesBySharpTag (string tag);
static sharp_array G_SharpI_FindEntitiesBySharpTag(sharp_string stag) {
	char const * tag = trap->Sharp_Unbox_String(stag);
	size_t tagl = strlen(tag);
	static gentity_t * ments [MAX_GENTITIES];
	size_t num_matches = 0;
	
	if (!tagl) goto end;
	
	gentity_t * ent = g_entities;
	//char tokenbuf [MAX_STRING_CHARS];
	for (int i = 0; i < level.num_entities; i++, ent++) {
		if (!ent->sharpTags) continue;
		//strncpy(tokenbuf, ent->sharpTags, MAX_STRING_CHARS);
		if (!strcmp(ent->sharpTags, tag)) {
			ments[num_matches++] = ent;
			continue;
		} 
		
		char * tagm = strchr(ent->sharpTags, ' ');
		while (tagm != NULL) {
			if (!strncmp(tagm + 1, tag, tagl)) {
				ments[num_matches++] = ent;
				break;
			} 
			tagm = strchr(tagm + 1, ' ');
		}
	}
	
	end:
	return trap->Sharp_Create_Ptr_Array(shandle, (void * *)ments, num_matches);
}

// C# --- public static extern IntPtr[] G_FindEntitiesBySharpTargetname (string name);
static sharp_array G_SharpI_FindEntitiesBySharpTargetname(sharp_string stargetname) {
	char const * tg = trap->Sharp_Unbox_String(stargetname);
	size_t tgl = strlen(tg);
	static gentity_t * ments [MAX_GENTITIES];
	size_t num_matches = 0;
	
	if (!tgl) goto end;
	
	gentity_t * ent = g_entities;
	for (int i = 0; i < level.num_entities; i++, ent++) {
		if (!ent->targetname) continue;
		if (!strcmp(ent->targetname, tg)) {
			ments[num_matches++] = ent;
			continue;
		} 
	}
	
	end:
	return trap->Sharp_Create_Ptr_Array(shandle, (void * *)ments, num_matches);
}

// ================================================================================================================================
// Serverside Sharp Internal
// ================================================================================================================================

void G_Sharp_Init() {
	G_Sharp_Shutdown();
	trap->Print("================================\n");
	trap->Print("Initializing Serverside Sharp\n");
	shandle = trap->Sharp_Create(SHARP_SERVER_DLL);
	if (!shandle) trap->Print("ERROR: SV_Sharp_Init: could not find/load " SHARP_SERVER_DLL ", scripting will NOT work without it!\n");
	
	cs.ib_c = trap->Sharp_Resolve_Class(shandle, SHARP_SERVER_NAMESPACE, SHARP_SERVER_API_CLASS);
	if (!cs.ib_c) trap->Print("ERROR: SV_Sharp_Init: loaded " SHARP_SERVER_DLL " does not contain the inbound API class (" SHARP_SERVER_API_CLASS "), scripts cannot be executed without this!\n");
	else {
		SHARPG_RESOLVE_METHOD(m_sload, Load_Manifest, 1)
		SHARPG_RESOLVE_METHOD(m_sclose, Close_Manifest, 1)
		SHARPG_RESOLVE_METHOD(m_shutdown, Event_Shutdown, 0)
		SHARPG_RESOLVE_METHOD(m_frame, Event_Frame, 1)
		SHARPG_RESOLVE_METHOD(m_chat, Event_Chat, 2)
		SHARPG_RESOLVE_METHOD(m_cmd, Event_Cmd, 2)
	}
	
	cs.ob_c = trap->Sharp_Resolve_Class(shandle, SHARP_SERVER_NAMESPACE, SHARP_SERVER_API_CLASS);
	if (!cs.ob_c) trap->Print("ERROR: SV_Sharp_Init: loaded " SHARP_SERVER_DLL " does not contain the outbound API class (" SHARP_SERVER_API_CLASS "), outbound calls will likely segfault!\n");
	else {
		SHARPG_DEFINE_INTERNAL(G_CreateEntity, G_SharpI_CreateEntity);
		SHARPG_DEFINE_INTERNAL(G_DestroyEntity, G_SharpI_DestroyEntity);
 		SHARPG_DEFINE_INTERNAL(G_IsPlayer, G_SharpI_IsPlayer);
		SHARPG_DEFINE_INTERNAL(G_IsNPC, G_SharpI_IsNPC);
		SHARPG_DEFINE_INTERNAL(G_Activator, G_SharpI_Activator);
		SHARPG_DEFINE_INTERNAL(G_Other, G_SharpI_Other);
		SHARPG_DEFINE_INTERNAL(G_Target, G_SharpI_Target);
		SHARPG_DEFINE_INTERNAL(G_Targetname, G_SharpI_Targetname);
		SHARPG_DEFINE_INTERNAL(G_Origin, G_SharpI_Origin);
		SHARPG_DEFINE_INTERNAL(G_NPC_Kill, G_SharpI_NPC_Kill);
		SHARPG_DEFINE_INTERNAL(G_EntFromPlayer, G_SharpI_EntFromPlayer);
		SHARPG_DEFINE_INTERNAL(G_PlayerFromEnt, G_SharpI_PlayerFromEnt);
		SHARPG_DEFINE_INTERNAL(G_PlayerName, G_SharpI_PlayerName);
		SHARPG_DEFINE_INTERNAL(G_SpawnNPC, G_SharpI_SpawnNPC);
		
		SHARPG_DEFINE_INTERNAL(FS_OpenR, G_SharpI_FS_OpenR);
		SHARPG_DEFINE_INTERNAL(FS_OpenW, G_SharpI_FS_OpenW);
		SHARPG_DEFINE_INTERNAL(FS_Close, G_SharpI_FS_Close);
		SHARPG_DEFINE_INTERNAL(FS_Length, G_SharpI_FS_Length);
		SHARPG_DEFINE_INTERNAL(FS_Read, G_SharpI_FS_Read);
		SHARPG_DEFINE_INTERNAL(FS_Write, G_SharpI_FS_Write);
		
		SHARPG_DEFINE_INTERNAL(G_PrintStr, G_SharpI_Print);
		SHARPG_DEFINE_INTERNAL(G_ChatAll, G_SharpI_ChatAll);
		SHARPG_DEFINE_INTERNAL(G_ChatTo, G_SharpI_ChatTo);
		
		SHARPG_DEFINE_INTERNAL(G_FindEntitiesBySharpTag, G_SharpI_FindEntitiesBySharpTag);
		SHARPG_DEFINE_INTERNAL(G_FindEntitiesBySharpTargetname, G_SharpI_FindEntitiesBySharpTargetname);
	}
	trap->Print("================================\n");
}

void G_Sharp_Shutdown () {
	if (shandle == NULL) return;
	G_Sharp_Event_Shutdown();
	trap->Sharp_Destroy(shandle);
	shandle = NULL;
	memset(&cs, 0, sizeof(sharp_callset_t));
}

void G_Sharp_Load_Map_Script() {
	if (level.sharpscript[0]) G_Sharp_Load(level.sharpscript);
}

void G_Sharp_Begin () {
	trap->Sharp_Bind(shandle);
}

// ================================================================================================================================
// Serverside Sharp Event Calls
// ================================================================================================================================

sharpg_handle G_Sharp_Load (char const * manifest) {
	
	if (!cs.m_sload) return 0;
	
	char const * filename = va(SHARP_SERVER_MANIFEST_SUBDIR "/%s." SHARP_SERVER_MANIFEST_EXT, manifest);
	trap->Print("Serverside Sharp Loading: %s (\"%s\")\n", manifest, filename);
	
	fileHandle_t fh;
	long len = trap->FS_Open(filename, &fh, FS_READ);
	if (!fh) {
		trap->Print("ERROR: G_Sharp_Load: manifest \"%s\" not found!\n", filename);
		return 0;
	}
	
	char * fbuf = malloc(len + 1);
	trap->FS_Read(fbuf, len, fh);
	sharp_string manifs = trap->Sharp_Create_String(shandle, fbuf);
	free(fbuf);
	trap->FS_Close(fh);
	
	char * err;
	void * args [1];
	args[0] = manifs;
	void * rhand = trap->Sharp_Invoke(shandle, cs.m_sload, args, &err);
	if (err) {
		trap->Print(va("WARNING: G_Sharp_Load: Script Exception: %s\n", err));
		return 0;
	}
	return * (sharpg_handle *) rhand;
}

void G_Sharp_Close (int manifest_id) {
	char * err;
	void * args [1];
	args[0] = &manifest_id;
	trap->Sharp_Invoke(shandle, cs.m_sclose, args, &err);
	if (err) {
		trap->Print(va("WARNING: G_Sharp_Close: Script Exception: %s\n", err));
	}
}

void G_Sharp_Event_Shutdown() {
	if (!cs.m_shutdown) return;
	
	char * err;
	trap->Sharp_Invoke(shandle, cs.m_shutdown, NULL, &err);
	if (err) trap->Print(va("WARNING: G_Sharp_Event_Shutdown: Script Exception: %s\n", err));
}

void G_Sharp_Event_Frame () {
	if (!cs.m_frame) return;
	
	char * err;
	void * args [1];
	args[0] = &level.time;
	trap->Sharp_Invoke(shandle, cs.m_frame, args, &err);
	if (err) trap->Print(va("WARNING: G_Sharp_Event_Frame: Script Exception: %s\n", err));
}

void G_Sharp_Event_Chat (int cli_num, char const * msg) {
	if (!cs.m_chat) return;
	
	char * err;
	void * args [2];
	args[0] = &cli_num;
	args[1] = trap->Sharp_Create_String(shandle, msg);
	trap->Sharp_Invoke(shandle, cs.m_chat, args, &err);
	if (err) trap->Print(va("WARNING: G_Sharp_Event_Chat: Script Exception: %s\n", err));
}

void G_Sharp_Event_Cmd (gentity_t * ent, char const * cmd) {
	if (!cs.m_cmd) return;
	
	char * err;
	void * args [2];
	args[0] = &ent;
	args[1] = trap->Sharp_Create_String(shandle, cmd);
	trap->Sharp_Invoke(shandle, cs.m_cmd, args, &err);
	if (err) trap->Print(va("WARNING: G_Sharp_Event_Cmd: Script Exception: %s\n", err));
}

// ================================================================================================================================
// Serverside Sharp Cmd
// ================================================================================================================================

void G_SharpCmd(gentity_t * ent, sharpCmd_t cmde) {
	char const * cmd = ent->sharpCmd[cmde];
	if (!cmd || !cmd[0]) return;
	G_Sharp_Event_Cmd (ent, cmd);
}
