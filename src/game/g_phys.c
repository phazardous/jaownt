#include "g_local.h"

static phys_world_t * gworld = NULL;
static int last_time = 0;

void G_Phys_Init() {
	trap->Print("================================\n");
	trap->Print("Initializing Serverside Physics\n");
	
	gworld = trap->Phys_World_Create();
	G_Phys_Update_CvarRes();
	trap->Phys_World_Add_Current_Map(gworld);
	
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

void G_Phys_Update_CvarRes() {
	if (!gworld) return;
	trap->Phys_World_Set_Resolution(gworld, g_physresolution.integer);
}

static phys_interactor_t act;

void G_Phys_UpdateEnt(gentity_t * ent) {
	if (!ent->phys) return;
	trap->Phys_Object_Get_Transform(ent->phys, &act);
	G_SetOrigin(ent, act.origin);
	G_SetAngles(ent, act.angles);
	trap->LinkEntity( (sharedEntity_t *) ent);
}

void G_Phys_UpdateEntMover(gentity_t * ent) {
	if (!ent->phys) return;
	VectorCopy(ent->r.currentOrigin, act.origin);
	VectorCopy(ent->r.currentAngles, act.angles);
	trap->Phys_Object_Set_Transform(ent->phys, &act);
}

void G_Phys_AddBMover(gentity_t * mover) {
	if (!mover->r.bmodel) return;
	int bmodi = strtol(mover->model + 1, NULL, 10);
	mover->phys = trap->Phys_Object_Create_From_BModel(gworld, bmodi, 0, qtrue);
}

void G_TEST_PhysTestEnt(vec3_t pos) {
	gentity_t * physent = G_Spawn();
	physent->s.eType = ET_GENERAL;
	physent->s.eFlags |= EF_PHYS;
	physent->r.svFlags |= SVF_BROADCAST;
	physent->phys = trap->Phys_Object_Create_From_Obj(gworld, "models/testbox.obj", pos, 500, qfalse);
	physent->s.modelindex = G_ModelIndex("models/testbox.obj");
	G_SetOrigin(physent, pos);
	trap->LinkEntity( (sharedEntity_t *) physent);
}
