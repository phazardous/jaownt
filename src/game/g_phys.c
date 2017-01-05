#include "g_local.h"

static phys_world_t * gworld = NULL;
static int last_time = 0;

static vec3_t nullvec = {0, 0, 0};

#define VELLERP_THRESH 30

static void vellerp(vec3_t player_vel, vec3_t other_vel, float lerp, qboolean clip_low, vec3_t out) {
	out[0] = player_vel[0] * lerp + other_vel[0] * (1 - lerp);
	out[1] = player_vel[1] * lerp + other_vel[1] * (1 - lerp);
	out[2] = player_vel[2] * lerp + other_vel[2] * (1 - lerp);
	
	if (clip_low) {
		vec3_t diff;
		VectorSet( diff, fabs(player_vel[0] - other_vel[0]), fabs(player_vel[1] - other_vel[1]), fabs(player_vel[2] - other_vel[2]) );
		if (fabs(diff[0]) + fabs(diff[1]) + fabs(diff[2]) < VELLERP_THRESH) {
			VectorCopy(other_vel, out);
		}
	}
}

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
		if (entClient) return; // client on client collisions are irrelevant (for now) -- TODO
		entClient = entB;
		entOther = entA;
		hitClient = col->posB;
		hitOther = col->posA;
		VectorInverse(col->normal);
	}
	
	if (!entClient) return; // collisions not involving a client are irrelevant
	
	if (col->normal[2] > col->normal[0] && col->normal[2] > col->normal[1] && entClient->playerState->velocity[2] <= 0) {
		
		// on floor, move velocity closer to floor's
		if (entOther == &g_entities[ENTITYNUM_WORLD]) {
			VectorClear(entClient->phys_post_target_velocity);
		} else {
			trap->Phys_Obj_Get_Linear_Velocity(entOther->phys, entClient->phys_post_target_velocity);
		}
		entClient->phys_post_do_vellerp = qtrue;
		
		entClient->playerState->eFlags |= EF_ON_PHYS;
		entClient->playerState->groundEntityNum = entOther->s.number;
		trap->LinkEntity((sharedEntity_t *)entClient);
	}
}

void G_Phys_Init() {
	trap->Print("================================\n");
	trap->Print("Initializing Serverside Physics\n");
	
	gworld = trap->Phys_World_Create(g_touch_cb);
	G_Phys_Upd_Res();
	G_Phys_Upd_Grav();
	trap->Phys_World_Add_Current_Map(gworld, &g_entities[ENTITYNUM_WORLD]);
	
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
	gentity_t * gent = g_entities;
	for (int i = 0; i < MAX_GENTITIES; i++, gent++) {
		if (!gent->playerState || !gent->phys) continue;
		gent->playerState->eFlags &= ~EF_ON_PHYS;
	}
	
	int delta = level.time - last_time;
	trap->Phys_World_Advance(gworld, delta);
	last_time = level.time;
	
	gent = g_entities;
	for (int i = 0; i < MAX_GENTITIES; i++, gent++) {
		if (!(gent->client && gent->phys)) continue;
		phys_transform_t tr;
		trap->Phys_Object_Get_Transform(gent->phys, &tr);
		VectorCopy(tr.origin, gent->playerState->origin);
		VectorCopy(tr.origin, gent->r.currentOrigin);
		trap->Phys_Obj_Get_Linear_Velocity(gent->phys, gent->playerState->velocity);
		/*
		if (gent->phys_post_do_vellerp) {
			qboolean clip = qtrue;
			if (gent->playerState->moveDir[0] || gent->playerState->moveDir[1]) clip = qtrue;
			vellerp(gent->playerState->velocity, gent->phys_post_target_velocity, 0.95, clip, gent->playerState->velocity);
			gent->phys_post_do_vellerp = qfalse;
		}
		*/
		trap->LinkEntity( (sharedEntity_t *) gent );
	}
}

void G_Phys_Upd_Res() {
	if (!gworld) return;
	trap->Phys_World_Set_Resolution(gworld, g_phys_resolution.integer);
}

void G_Phys_Upd_Grav() {
	if (!gworld) return;
	trap->Phys_World_Set_Gravity(gworld, g_gravity.value);
}

void G_Phys_Upd_ClF() {
	gentity_t * gent = g_entities;
	phys_properties_t * props;
	for (int i = 0; i < MAX_GENTITIES; i++, gent++) {
		if (!gent->playerState || !gent->phys) continue;
		props = trap->Phys_Object_Get_Properties(gent->phys);
		props->friction = g_phys_clientfriction.value;
		trap->Phys_Object_Set_Properties(gent->phys);
	}
}

static phys_transform_t trans;
static phys_properties_t props;

void G_Phys_UpdateEnt(gentity_t * ent) {
	
	phys_properties_t * props;
	
	if (!ent->phys) return;
	switch (ent->s.eType) {
	case ET_PLAYER:
		props = trap->Phys_Object_Get_Properties(ent->phys);
		if (ent->client->noclip) {
			props->contents = 0;
		} else {
			props->contents = ent->r.contents;
		}
		trap->Phys_Object_Set_Properties(ent->phys);
	case ET_NPC:
		VectorCopy(ent->r.currentOrigin, trans.origin);
		VectorCopy(ent->r.currentAngles, trans.angles);
		trap->Phys_Object_Set_Transform(ent->phys, &trans);
		trap->Phys_Obj_Set_Linear_Velocity(ent->phys, ent->playerState->velocity);
		break;
	case ET_MOVER:
		VectorCopy(ent->r.currentOrigin, trans.origin);
		VectorCopy(ent->r.currentAngles, trans.angles);
		trap->Phys_Object_Set_Transform(ent->phys, &trans);
		phys_properties_t * props = trap->Phys_Object_Get_Properties(ent->phys);
		if (props->contents != ent->r.contents) {
			props->contents = ent->r.contents;
			trap->Phys_Object_Set_Properties(ent->phys);
		}
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
	props.contents = mover->r.contents;
	props.token = mover;
	
	mover->phys = trap->Phys_Object_Create_From_BModel(gworld, bmodi, &trans, &props);
}

void G_Phys_AddHitboxKinematic(gentity_t * ent) {
	if (ent->phys) trap->Phys_World_Remove_Object(gworld, ent->phys);
	
	props.mass = -1;
	props.friction = 0;
	props.restitution = 0;
	props.dampening = 0;
	props.actor = qtrue;
	props.kinematic = qfalse;
	props.contents = ent->r.contents;
	props.token = ent;
	
	VectorCopy(ent->r.currentOrigin, trans.origin);
	VectorClear(trans.angles);
	
	ent->phys = trap->Phys_Object_Create_Box(gworld, ent->r.mins, ent->r.maxs, &trans, &props);
}

void G_Phys_AddClientCapsule(gentity_t * ent) {
	if (ent->phys) trap->Phys_World_Remove_Object(gworld, ent->phys);
	
	props.mass = -1;
	props.friction = g_phys_clientfriction.value;
	props.restitution = 0;
	props.dampening = 0;
	props.actor = qtrue;
	props.kinematic = qfalse;
	props.contents = ent->r.contents;
	props.token = ent;
	
	VectorCopy(ent->r.currentOrigin, trans.origin);
	VectorClear(trans.angles);
	
	float radius = (fabs(ent->r.maxs[0] - ent->r.mins[0]) + fabs(ent->r.maxs[1] - ent->r.mins[1])) / 4;
	float cheight = fabs(ent->r.maxs[2] - ent->r.mins[2]) - 2 * radius;
	float voffs = (ent->r.mins[2] + ent->r.maxs[2]) / 2;
	
	ent->phys = trap->Phys_Object_Create_Capsule(gworld, cheight, radius, voffs, &trans, &props);
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
	physent->r.svFlags |= SVF_BROADCAST;
	
	VectorCopy(pos, trans.origin);
	VectorClear(trans.angles);
	
	size_t testm_i = rand() % testmodels_num;
	
	props.mass = -1;
	props.friction = 0.5;
	props.restitution = 0.125;
	props.dampening = 0.05;
	props.actor = qfalse;
	props.kinematic = qfalse;
	props.contents = CONTENTS_SOLID;
	props.token = physent;
	
	physent->phys = trap->Phys_Object_Create_From_Obj(gworld, testmodels[testm_i], &trans, &props, 1);
	physent->s.modelindex = G_ModelIndex(testmodels[testm_i]);
	
	G_SetOrigin(physent, pos);
	trap->LinkEntity( (sharedEntity_t *) physent);
}
