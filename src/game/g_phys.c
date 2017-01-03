#include "g_local.h"

static phys_world_t * gworld = NULL;
static int last_time = 0;

static void g_touch_cb(phys_world_t * w, phys_collision_t * col) {
	assert(w == gworld);
	
	phys_properties_t * propsA = trap->Phys_Object_Get_Properties(col->A);
	phys_properties_t * propsB = trap->Phys_Object_Get_Properties(col->B);
	
	gentity_t * entA = propsA->token;
	gentity_t * entB = propsB->token;
	
	if (!entA || !entB) return;

	gentity_t * entClient = NULL;
	gentity_t * entOther = NULL;
	float * hitClient;
	float * hitOther;
	
	if (entA->s.eType == ET_PLAYER || entA->s.eType == ET_NPC) {
		entClient = entA;
		entOther = entB;
		hitClient = col->posA;
		hitOther = col->posB;
	}
	if (entB->s.eType == ET_PLAYER || entB->s.eType == ET_NPC) {
		if (entClient) return; // client on client collisions are irrelevant
		entClient = entB;
		entOther = entA;
		hitClient = col->posB;
		hitOther = col->posA;
		VectorInverse(col->normal);
	}
	
	if (!entClient) return; // collisions not involving a client are irrelevant
		
	vec3_t va1, va2, work;
	VectorScale(col->normal, col->impulse, va1);
	VectorSubtract(hitOther, hitClient, work);
	VectorScale(col->normal, fabs(VectorLength(work)) * 5, va2);
	VectorAdd(va2, entClient->playerState->velocity, entClient->playerState->velocity);
}

void G_Phys_Init() {
	trap->Print("================================\n");
	trap->Print("Initializing Serverside Physics\n");
	
	gworld = trap->Phys_World_Create(g_touch_cb);
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
	switch (ent->s.eType) {
	case ET_PLAYER:
		VectorCopy(ent->r.currentOrigin, trans.origin);
		VectorCopy(ent->r.currentAngles, trans.angles);
		trap->Phys_Object_Set_Transform(ent->phys, &trans);
		break;
	case ET_NPC:
		VectorCopy(ent->r.currentOrigin, trans.origin);
		VectorCopy(ent->r.currentAngles, trans.angles);
		trap->Phys_Object_Set_Transform(ent->phys, &trans);
		break;
	case ET_MOVER:
		VectorCopy(ent->r.currentOrigin, trans.origin);
		VectorCopy(ent->r.currentAngles, trans.angles);
		trap->Phys_Object_Set_Transform(ent->phys, &trans);
		break;
	case ET_GENERAL:
	default:
		trap->Phys_Object_Get_Transform(ent->phys, &trans);
		G_SetOrigin(ent, trans.origin);
		G_SetAngles(ent, trans.angles);
		
		ent->s.pos.trType = TR_INTERPOLATE;
		ent->s.apos.trType = TR_INTERPOLATE;
		
		trap->LinkEntity( (sharedEntity_t *) ent);
		break;
	}
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
	props.actor = qfalse;
	props.kinematic = qtrue;
	props.token = mover;
	
	mover->phys = trap->Phys_Object_Create_From_BModel(gworld, bmodi, &trans, &props);
}

void G_Phys_AddHitboxKinematic(gentity_t * ent) {
	if (ent->phys) trap->Phys_World_Remove_Object(gworld, ent->phys);
	
	props.mass = 0;
	props.friction = 1;
	props.restitution = 0;
	props.dampening = 0;
	props.actor = qfalse;
	props.kinematic = qtrue;
	props.token = ent;
	
	VectorCopy(ent->r.currentOrigin, trans.origin);
	VectorClear(trans.angles);
	
	ent->phys = trap->Phys_Object_Create_Box(gworld, ent->r.mins, ent->r.maxs, &trans, &props);
}

void G_Phys_Remove(gentity_t * ent) {
	if (ent->phys) trap->Phys_World_Remove_Object(gworld, ent->phys);
	ent->phys = NULL;
};

static char const * testmodels [] = {
	"models/testbox.obj",
	"models/testbox2.obj"
};
static size_t const testmodels_num = sizeof(testmodels) / sizeof(char const *);

void G_TEST_PhysTestEnt(vec3_t pos) {
	gentity_t * physent = G_Spawn();
	physent->s.eType = ET_GENERAL;
	physent->s.eFlags |= EF_PHYS;
	physent->r.svFlags |= SVF_BROADCAST;
	
	VectorCopy(pos, trans.origin);
	VectorClear(trans.angles);
	
	size_t testm_i = rand() % testmodels_num;
	
	props.mass = -1;
	props.friction = 0.5;
	props.restitution = 0.125;
	props.dampening = 0.125;
	props.actor = qfalse;
	props.kinematic = qfalse;
	props.token = physent;
	
	physent->phys = trap->Phys_Object_Create_From_Obj(gworld, testmodels[testm_i], &trans, &props, 1);
	physent->s.modelindex = G_ModelIndex(testmodels[testm_i]);
	
	G_SetOrigin(physent, pos);
	trap->LinkEntity( (sharedEntity_t *) physent);
}
