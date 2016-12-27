#include "g_local.h"

static phys_world_t * gworld = NULL;
static int last_time = 0;

void G_Phys_Init() {
	trap->Print("================================\n");
	trap->Print("Initializing Serverside Physics\n");
	gworld = trap->Phys_World_Create();
	
	vec3_t pos, mins, maxs;
	VectorSet(pos, 0, 0, 150);
	VectorSet(mins, -4, -4, -4);
	VectorSet(maxs, 4, 4, 4);
	
	trap->Print("================================\n");
}

void G_Phys_Shutdown() {
	if (gworld) {
		trap->Phys_World_Destroy(gworld);
		gworld = NULL;
	}
	last_time = 0;
}

void G_Phys_Frame() {
	int delta = level.time - last_time;
	trap->Phys_World_Advance(gworld, delta);
	last_time = level.time;
}

static phys_interactor_t act;

void G_Phys_UpdateEnt(gentity_t * ent) {
	trap->Phys_Object_Get_Transform(ent->phys, &act);
	G_SetOrigin(ent, act.origin);
	G_SetAngles(ent, act.angles);
	trap->LinkEntity( (sharedEntity_t *) ent);
}

void G_TEST_PhysTestEnt() {
	vec3_t pos;
	VectorSet(pos, 0, 0, 500);
	gentity_t * physent = G_Spawn();
	physent->s.eFlags |= EF_PHYS;
	physent->phys = trap->Phys_Object_Create_From_Obj(gworld, "models/testbox.obj", pos, 50);
	physent->s.modelindex = G_ModelIndex("models/testbox.obj");
	G_SetOrigin(physent, pos);
	trap->LinkEntity( (sharedEntity_t *) physent);
}
