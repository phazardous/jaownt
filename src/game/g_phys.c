#include "g_local.h"

static phys_world_t * gworld = NULL;
static int last_time = 0;

void G_Phys_Init() {
	trap->Print("================================\n");
	trap->Print("Initializing Serverside Physics\n");
	
	gworld = trap->Phys_World_Create();
	G_Phys_Upd_Res();
	G_Phys_Upd_Grav();
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

void G_Phys_Upd_Res() {
	if (!gworld) return;
	trap->Phys_World_Set_Resolution(gworld, g_physresolution.integer);
}

void G_Phys_Upd_Grav() {
	if (!gworld) return;
	trap->Phys_World_Set_Gravity(gworld, g_gravity.value);
}

static phys_transform_t trans;
static phys_properties_t props;

void G_Phys_UpdateEnt(gentity_t * ent) {
	if (!ent->phys) return;
	trap->Phys_Object_Get_Transform(ent->phys, &trans);
	G_SetOrigin(ent, trans.origin);
	G_SetAngles(ent, trans.angles);
	
	ent->s.pos.trType = TR_INTERPOLATE;
	ent->s.apos.trType = TR_INTERPOLATE;
	
	trap->LinkEntity( (sharedEntity_t *) ent);
}

void G_Phys_UpdateEntMover(gentity_t * ent) {
	if (!ent->phys) return;
	VectorCopy(ent->r.currentOrigin, trans.origin);
	VectorCopy(ent->r.currentAngles, trans.angles);
	trap->Phys_Object_Set_Transform(ent->phys, &trans);
}

void G_Phys_AddBMover(gentity_t * mover) {
	if (!mover->r.bmodel) return;
	int bmodi = strtol(mover->model + 1, NULL, 10);
	
	VectorClear(trans.origin);
	VectorClear(trans.angles);
	
	props.mass = 0;
	props.friction = 0.5;
	props.restitution = 0.125;
	props.dampening = 0;
	props.token = mover;
	
	mover->phys = trap->Phys_Object_Create_From_BModel(gworld, bmodi, &trans, &props, qtrue);
}

void G_TEST_PhysTestEnt(vec3_t pos) {
	gentity_t * physent = G_Spawn();
	physent->s.eType = ET_GENERAL;
	physent->s.eFlags |= EF_PHYS;
	physent->r.svFlags |= SVF_BROADCAST;
	
	VectorCopy(pos, trans.origin);
	VectorClear(trans.angles);
	
	props.mass = 500;
	props.friction = 0.5;
	props.restitution = 0.125;
	props.dampening = 0.125;
	props.token = physent;
	
	physent->phys = trap->Phys_Object_Create_From_Obj(gworld, "models/testbox.obj", &trans, &props, 0.25, qfalse);
	
	physent->s.modelindex = G_ModelIndex("models/testbox.obj");
	physent->s.iModelScale = 25;
	
	G_SetOrigin(physent, pos);
	trap->LinkEntity( (sharedEntity_t *) physent);
}
