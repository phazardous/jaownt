#include "g_local.h"

static phys_world_t * gworld = NULL;
static int last_time = 0;

static vec3_t nullvec = {0, 0, 0};

#define VELLERP_THRESH 30

static void vellerp(vec3_t v1, vec3_t v2, float lerp, vec3_t out) {
	out[0] = v1[0] * lerp + v2[0] * (1 - lerp);
	out[1] = v1[1] * lerp + v2[1] * (1 - lerp);
	out[2] = v1[2] * lerp + v2[2] * (1 - lerp);
}

static void g_touch_cb_do(phys_world_t * w, phys_collision_t * col, gentity_t * entThis,  gentity_t * entOther) {
	
	if (col->normal[2] < -0.707 && !entThis->client->pers.cmd.upmove) { // 45 degrees is maximum walkable surface angle -- TODO: cvar
		
		// on floor, move velocity closer to floor's
		if (entOther == &g_entities[ENTITYNUM_WORLD]) {
			VectorClear(entThis->phys_post_target_velocity);
		} else {
			trap->Phys_Obj_Get_Linear_Velocity(entOther->phys, entThis->phys_post_target_velocity);
		}
		entThis->phys_post_do_vellerp = qtrue;
		
		entThis->playerState->eFlags |= EF_ON_PHYS;
		entThis->playerState->groundEntityNum = entOther->s.number;
		trap->LinkEntity((sharedEntity_t *)entThis);
		
		//trap->Print("%f\n", col->impulse);
		
		/*
		vec3_t right, apply, curvel;
		trap->Phys_Object_Get_Origin(entThis->phys, btorig);
		trap->Phys_Obj_Get_Linear_Velocity(entThis->phys, curvel);
		AngleVectors(entThis->playerState->viewangles, NULL, right, NULL);
		VectorCopy(btorig, down);
		down[2] -= 240;
		trap->Trace (&tr, btorig, entThis->r.mins, entThis->r.maxs, down, entThis->playerState->clientNum, MASK_PLAYERSOLID, qfalse, 0, 10);
		CrossProduct(tr.plane.normal, right, apply);
		VectorScale(apply, VectorLength(curvel), apply);
		trap->Print("(%f, %f, %f)\n", tr.plane.normal[0], tr.plane.normal[1], tr.plane.normal[2]);
		trap->Phys_Obj_Set_Linear_Velocity(entThis->phys, apply);
		*/
		
		/*
		phys_trace_t tr;
		vec3_t start, end, right, curvel, apply, cvn, an;
		trap->Phys_Object_Get_Origin(entThis->phys, start);
		VectorCopy(start, end);
		end[2] -= 200 + fabs(entThis->r.mins[2]);
		trap->Phys_World_Trace(gworld, start, end, &tr);
		if (tr.hit_object) {
			AngleVectors(entThis->playerState->viewangles, NULL, right, NULL);
			trap->Phys_Obj_Get_Linear_Velocity(entThis->phys, curvel);
			CrossProduct(tr.hit_normal, right, apply);
			VectorScale(apply, VectorLength(curvel), apply);
			VectorNormalize2(curvel, cvn);
			VectorNormalize2(apply, an);
			float v = DotProduct(cvn, an);
			if (v < 0) v = 0;
			vellerp(apply, curvel, v, apply);
			trap->Phys_Obj_Set_Linear_Velocity(entThis->phys, apply);
		}
		*/
	}
	
	/*
	trap->Phys_Object_Get_Origin(entThis->phys, btorig);
	VectorCopy(btorig, up);
	up[2] += STEPSIZE;
	trap->Trace (&tr, btorig, entThis->r.mins, entThis->r.maxs, up, entThis->playerState->clientNum, MASK_PLAYERSOLID, qfalse, 0, 10);
	step_size = tr.endpos[2] - btorig[2];
	VectorCopy(tr.endpos, btorig);
	
	VectorCopy (btorig, down);
	down[2] -= step_size;
	trap->Trace (&tr, btorig, entThis->r.mins, entThis->r.maxs, down, entThis->playerState->clientNum, MASK_PLAYERSOLID, qfalse, 0, 10);
	VectorCopy(tr.endpos, btorig);
	trap->Phys_Object_Set_Origin(entThis->phys, btorig);
	*/
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
	qboolean cli2 = qfalse;
	
	if (entA->s.eType == ET_PLAYER || entA->s.eType == ET_NPC) {
		entClient = entA;
		entOther = entB;
	}
	if (entB->s.eType == ET_PLAYER || entB->s.eType == ET_NPC) {
		if (entClient) cli2 = qtrue;
		entClient = entB;
		entOther = entA;
	}
	
	if (!entClient) return; // collisions not involving a client are irrelevant
	
	g_touch_cb_do(w, col, entClient, entOther);
	if (cli2) {
		g_touch_cb_do(w, col, entOther, entClient);
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
		vec3_t btorig;
		trap->Phys_Object_Get_Origin(gent->phys, btorig);
		VectorCopy(btorig, gent->playerState->origin);
		VectorCopy(btorig, gent->r.currentOrigin);
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
		trap->Phys_Object_Set_Origin(ent->phys, trans.origin);
		trap->Phys_Object_Set_Rotation(ent->phys, trans.angles);
		trap->Phys_Obj_Set_Linear_Velocity(ent->phys, ent->playerState->velocity);
		break;
	case ET_MOVER:
		VectorCopy(ent->r.currentOrigin, trans.origin);
		VectorCopy(ent->r.currentAngles, trans.angles);
		trap->Phys_Object_Set_Origin(ent->phys, trans.origin);
		trap->Phys_Object_Set_Rotation(ent->phys, trans.angles);
		phys_properties_t * props = trap->Phys_Object_Get_Properties(ent->phys);
		if (props->contents != ent->r.contents) {
			props->contents = ent->r.contents;
			trap->Phys_Object_Set_Properties(ent->phys);
		}
		break;
	case ET_GENERAL:
	default:
		trap->Phys_Object_Get_Origin(ent->phys, trans.origin);
		trap->Phys_Object_Get_Rotation(ent->phys, trans.angles);
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
