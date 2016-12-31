#include "phys_public.hpp"
#include "qcommon/cm_public.h"
#include "qcommon/qcommon.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <btBulletDynamicsCommon.h>

cvar_t * phys_playerclip;

void Com_Phys_Init() {
	phys_playerclip = Cvar_Get("phys_playerclip", "1", CVAR_ARCHIVE | CVAR_SYSTEMINFO, "Should physics objects collide with CONTENTS_PLAYERCLIP?");
}

static constexpr float d2r_mult = 0.0174532925f;
static constexpr float r2d_mult = 57.2957795f;

static std::unordered_map<btDynamicsWorld const *, phys_world_t *> world_map;

struct phys_object_s {
	std::mutex objlock {};
	
	bool do_trans_update = false;
	
	btTransform new_trans {};
	btTransform trans_cache {};
	
	phys_properties_t properties;
	
	btVector3 inertia {0, 0, 0}; 
	btCollisionShape * shape = nullptr;
	btDefaultMotionState * motion_state = nullptr;
	btRigidBody * body = nullptr;
	
	bool is_compound = false;
	
	void set_properties() {
		shape->calculateLocalInertia(properties.mass, inertia);
		body->setMassProps(properties.mass, inertia);
		body->setDamping(properties.dampening, properties.dampening);
		body->setFriction(properties.friction);
		body->setRestitution(properties.restitution);
		body->setSleepingThresholds(20.f, 20.f);
	}
	
	~phys_object_s() {
		if (body) delete body;
		if (motion_state) delete motion_state;
		if (shape) {
			if (is_compound) {
				btCompoundShape * cs = reinterpret_cast<btCompoundShape *>(shape);
				for (int i = 0; i < cs->getNumChildShapes(); i++) {
					delete cs->getChildShape(i);
				}
			}
			delete shape;
		}
	}
};

struct phys_world_s {
	btBroadphaseInterface * broadphase = nullptr;
	btDefaultCollisionConfiguration * config = nullptr;
	btCollisionDispatcher * dispatcher = nullptr;
	btSequentialImpulseConstraintSolver * solver = nullptr;
	btDiscreteDynamicsWorld * world = nullptr;
	
	int step_resolution = 120;
	
	std::unordered_set<phys_object_t *> objects;
	std::unordered_map<btCollisionObject const *, phys_object_t *> object_map;
	
	phys_object_t * map_static = nullptr;
	phys_object_t * map_static_slick = nullptr;
	
	std::mutex simlock {};
	
	std::atomic_bool run {true};
	std::atomic_int advance {0};
	std::thread * thr = nullptr;
	
	~phys_world_s() {
		run.store(false);
		thr->join();
		
		delete thr;
		
		if (map_static) delete map_static;
		if (map_static_slick) delete map_static_slick;
		
		for (phys_object_t * obj : objects) {
			delete obj;
		}
		
		objects.clear();
		
		if (world) delete world;
		if (solver) delete solver;
		if (dispatcher) delete dispatcher;
		if (config) delete config;
		if (broadphase) delete broadphase;
	}
};

static void bullet_world_substep_cb(btDynamicsWorld *world, btScalar timeStep) {
	
	phys_world_t * this_world = nullptr;
	{
		auto iter = world_map.find(world);
		if (iter == world_map.end()) return; 
		this_world = iter->second;
	}
	
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++) {
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		int numContacts = contactManifold->getNumContacts();
		if (!numContacts) continue;
		
		btCollisionObject const * obA = static_cast<btCollisionObject const *>(contactManifold->getBody0());
		btCollisionObject const * obB = static_cast<btCollisionObject const *>(contactManifold->getBody1());
		
		phys_object_t * pA = nullptr;
		phys_object_t * pB = nullptr;
		
		{
			if (obA == this_world->map_static->body) {
				pA = this_world->map_static;
			} else if (obA == this_world->map_static_slick->body) {
				pA = this_world->map_static_slick;
			} else {
				auto iter = this_world->object_map.find(obA);
				if (iter == this_world->object_map.end()) continue; 
				pA = iter->second;
			}
		}
		{
			if (obB == this_world->map_static->body) {
				pB = this_world->map_static;
			} else if (obB == this_world->map_static_slick->body) {
				pB = this_world->map_static_slick;
			} else {
				auto iter = this_world->object_map.find(obB);
				if (iter == this_world->object_map.end()) continue; 
				pB = iter->second;
			}
		}
		
		pA->objlock.lock();
		pB->objlock.lock();
		
		// TODO -- WORLD INTERACTION CALLBACK
		
		pA->objlock.unlock();
		pB->objlock.unlock();
	}
}

static void bullet_world_thread_loop(phys_world_t * w) {
	while (w->run) {
		
		int adv = w->advance.exchange(0);
		
		if (!adv) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			continue;
		}
		
		w->simlock.lock();
		for (phys_object_t * obj : w->objects) {
			obj->objlock.lock();
			if (obj->do_trans_update) {
				obj->do_trans_update = false;
				obj->motion_state->setWorldTransform(obj->new_trans);
				obj->body->activate(true);
			}
			obj->objlock.unlock();
		}
		
		btScalar advc = adv / 1000.0f;
		w->world->stepSimulation(advc, w->step_resolution, 1.0f / w->step_resolution);
		
		for (phys_object_t * obj : w->objects) {
			obj->objlock.lock();
			obj->motion_state->getWorldTransform(obj->trans_cache);
			obj->objlock.unlock();
		}
		w->simlock.unlock();
		
	}
}

phys_world_t * Phys_World_Create() {
	phys_world_t * nw = new phys_world_t;
	
	nw->broadphase = new btDbvtBroadphase;
	nw->config = new btDefaultCollisionConfiguration;
	nw->dispatcher = new btCollisionDispatcher {nw->config};
	nw->solver = new btSequentialImpulseConstraintSolver;
	nw->world = new btDiscreteDynamicsWorld {nw->dispatcher, nw->broadphase, nw->solver, nw->config};
	
	nw->world->setGravity( {0, 0, -800} );
	nw->world->setInternalTickCallback(bullet_world_substep_cb);
	nw->world->setForceUpdateAllAabbs(false);

	nw->thr = new std::thread {bullet_world_thread_loop, nw};
	
	world_map[nw->world] = nw;
	
	return nw;
}

void Phys_World_Destroy(phys_world_t * w) {
	world_map.erase(w->world);
	delete w;
}

void Phys_World_Advance(phys_world_t * world, int time) {
	world->advance += time;
}

void Phys_World_Set_Resolution(phys_world_t * world, unsigned int resolution) {
	world->simlock.lock();
	world->step_resolution = resolution;
	world->simlock.unlock();
}

void Phys_World_Set_Gravity(phys_world_t * world, float gravity) {
	world->simlock.lock();
	world->world->setGravity( {0, 0, -gravity} );
	world->simlock.unlock();
}

#define BP_POINTS_SIZE (2048*4)

struct gptp_brusurf_task_data {
	phys_world_t * world;
	int brushes_num, surfaces_num;
	int * brushes = nullptr;
	int * patches = nullptr;
	std::atomic_int brush_i, patch_i;
	std::mutex objmut;
	
	btCompoundShape * cs = nullptr;
	btCompoundShape * css = nullptr;
	
	~gptp_brusurf_task_data() {
		if (brushes) delete brushes;
		if (patches) delete patches;
	}
};

void * gptp_brush_task(void * arg) {
	
	gptp_brusurf_task_data * data = reinterpret_cast<gptp_brusurf_task_data *>(arg);
	vec3_t points [BP_POINTS_SIZE];
	int points_num;
	
	int content_mask = CONTENTS_SOLID;
	if (phys_playerclip->integer) content_mask |= CONTENTS_PLAYERCLIP;
	
	while (true) {
		int ib = data->brush_i--;
		if (ib < 0) return nullptr;
		int i = data->brushes[ib];
		if (CM_BrushContentFlags(i) & content_mask) {
			points_num = CM_CalculateHull(i, points, BP_POINTS_SIZE);
			if (points_num < 4) continue;
			
			
			btConvexHullShape * chs = new btConvexHullShape();
			for (int i = 0; i < points_num; i++) {
				btVector3 vec { points[i][0], points[i][1], points[i][2] };
				chs->addPoint(vec, false);
			}
			chs->recalcLocalAabb();
			data->objmut.lock();
			if (CM_BrushOverallFlags(i) & SURF_SLICK) data->css->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
			else data->cs->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
			data->objmut.unlock();
		}
	}
}

void * gptp_surface_task(void * arg) {
	
	gptp_brusurf_task_data * data = reinterpret_cast<gptp_brusurf_task_data *>(arg);
	vec3_t points [BP_POINTS_SIZE];
	
	int content_mask = CONTENTS_SOLID;
	if (phys_playerclip->integer) content_mask |= CONTENTS_PLAYERCLIP;
	
	while (true) {
		int pi = data->patch_i--;
		if (pi < 0) return nullptr;
		int i = data->patches[pi];
		if (CM_PatchContentFlags(i) & content_mask) {
			int width, height;
			CM_PatchMeshPoints(i, points, BP_POINTS_SIZE, &width, &height);
			if (width * height < 4) continue;
			
			for (int x = 0; x < width - 1; x++) {
				for (int y = 0; y < height - 1; y++) {
					btConvexHullShape * chs = new btConvexHullShape();
					chs->addPoint(btVector3 {points[((y + 0) * width) + (x + 0)][0], points[((y + 0) * width) + (x + 0)][1], points[((y + 0) * width) + (x + 0)][2]}, false);
					chs->addPoint(btVector3 {points[((y + 1) * width) + (x + 0)][0], points[((y + 1) * width) + (x + 0)][1], points[((y + 1) * width) + (x + 0)][2]}, false);
					chs->addPoint(btVector3 {points[((y + 0) * width) + (x + 1)][0], points[((y + 0) * width) + (x + 1)][1], points[((y + 0) * width) + (x + 1)][2]}, false);
					chs->addPoint(btVector3 {points[((y + 1) * width) + (x + 1)][0], points[((y + 1) * width) + (x + 1)][1], points[((y + 1) * width) + (x + 1)][2]}, false);
					chs->recalcLocalAabb();
					
					chs->recalcLocalAabb();
					data->objmut.lock();
					if (CM_PatchSurfaceFlags(i) & SURF_SLICK) data->css->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
					else data->cs->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
					data->objmut.unlock();
				}
			}
		}
	}
}

void Phys_World_Add_Current_Map(phys_world_t * world) {
	
	if (world->map_static) delete world->map_static;
	
	int brushes_max, surfaces_max;
	gptp_brusurf_task_data btd;
	
	btd.world = world;
	
	CM_NumData(&brushes_max, &surfaces_max);
	
	btd.brushes = new int [brushes_max];
	btd.patches = new int [surfaces_max];
	
	CM_SubmodelIndicies(0, btd.brushes, btd.patches, &btd.brushes_num, &btd.surfaces_num);
	
	btd.brush_i = btd.brushes_num - 1;
	btd.patch_i = btd.surfaces_num - 1;
	
	btd.cs = new btCompoundShape;
	btd.css = new btCompoundShape;
	
	std::vector<void *> tasks;
	for (unsigned int i = 0; i < GPTP_GetThreadCount(); i++) {
		tasks.push_back(GPTP_TaskBegin(gptp_brush_task, &btd));
	}
	for (void * task : tasks) {
		GPTP_TaskCollect(task);
	}
	tasks.clear();
	for (unsigned int i = 0; i < GPTP_GetThreadCount(); i++) {
		tasks.push_back(GPTP_TaskBegin(gptp_surface_task, &btd));
	}
	for (void * task : tasks) {
		GPTP_TaskCollect(task);
	}
	
	world->map_static = new phys_object_t;
	world->map_static->is_compound = true;
	btd.cs->recalculateLocalAabb();
	world->map_static->shape = btd.cs;
	world->map_static->motion_state = new btDefaultMotionState { btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} } };
	world->map_static->shape->calculateLocalInertia(0, world->map_static->inertia);
	btRigidBody::btRigidBodyConstructionInfo CI {0, world->map_static->motion_state, world->map_static->shape, world->map_static->inertia};
	world->map_static->body = new btRigidBody {CI};
	world->world->addRigidBody(world->map_static->body);
	
	world->map_static_slick = new phys_object_t;
	world->map_static_slick->is_compound = true;
	btd.css->recalculateLocalAabb();
	world->map_static_slick->shape = btd.css;
	world->map_static_slick->motion_state = new btDefaultMotionState { btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} } };
	world->map_static_slick->shape->calculateLocalInertia(0, world->map_static_slick->inertia);
	btRigidBody::btRigidBodyConstructionInfo CIS {0, world->map_static_slick->motion_state, world->map_static_slick->shape, world->map_static_slick->inertia};
	world->map_static_slick->body = new btRigidBody {CIS};
	world->map_static_slick->body->setFriction(0);
	world->world->addRigidBody(world->map_static_slick->body);
}

static void bullet_world_add_object(phys_world_t * w, phys_object_t * obj) {
	w->simlock.lock();
	w->world->addRigidBody(obj->body);
	w->objects.insert(obj);
	w->object_map[obj->body] = obj;
	obj->trans_cache = obj->body->getWorldTransform();
	w->simlock.unlock();
}

phys_object_t * Phys_Object_Create_From_Obj(phys_world_t * world, char const * path, phys_transform_t * initial_transform, phys_properties_t * properties, float scale, qboolean kinematic) {
	phys_object_t * no = new phys_object_t;
	
	no->properties = *properties;

	objModel_t * mod = CM_LoadObj(path);
	if (!mod) return nullptr;
	
	if (mod->numSurfaces > 1) {
		
		btCompoundShape * cps = new btCompoundShape;
		no->is_compound = true;
		
		for (int s = 0; s < mod->numSurfaces; s++) {
			
			btConvexHullShape * chs = new btConvexHullShape;
			for (int i = 0; i < mod->surfaces[s].numFaces; i++) {
				chs->addPoint( {mod->surfaces[s].faces[i][0].vertex[0], mod->surfaces[s].faces[i][0].vertex[1], mod->surfaces[s].faces[i][0].vertex[2]}, false );
				chs->addPoint( {mod->surfaces[s].faces[i][1].vertex[0], mod->surfaces[s].faces[i][1].vertex[1], mod->surfaces[s].faces[i][1].vertex[2]}, false );
				chs->addPoint( {mod->surfaces[s].faces[i][2].vertex[0], mod->surfaces[s].faces[i][2].vertex[1], mod->surfaces[s].faces[i][2].vertex[2]}, false );
			}
			
			chs->recalcLocalAabb();
			cps->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
		}
		cps->setLocalScaling({scale, scale, scale});
		cps->recalculateLocalAabb();
		no->shape = cps;
		
	} else {
		
		btConvexHullShape * chs = new btConvexHullShape;
		
		for (int i = 0; i < mod->numVerts; i++) {
			chs->addPoint( {mod->verts[i*3], mod->verts[i*3+1], mod->verts[i*3+2]}, false );
		}
		chs->setLocalScaling({scale, scale, scale});
		chs->recalcLocalAabb();
		no->shape = chs;
	}
	
	no->motion_state = new btDefaultMotionState { btTransform { 
		btQuaternion { initial_transform->angles[0] * d2r_mult, initial_transform->angles[2] * d2r_mult, initial_transform->angles[1] * d2r_mult }, 
		btVector3 {initial_transform->origin[0], initial_transform->origin[1], initial_transform->origin[2]} } };
	no->shape->calculateLocalInertia(no->properties.mass, no->inertia);
	btRigidBody::btRigidBodyConstructionInfo CI {no->properties.mass, no->motion_state, no->shape, no->inertia};
	no->body = new btRigidBody {CI};
	
	no->set_properties();
	
	if (kinematic) {
		no->body->setCollisionFlags(no->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		no->body->setActivationState(DISABLE_DEACTIVATION);
	}
	
	bullet_world_add_object(world, no);
	
	return no;
}

phys_object_t * Phys_Object_Create_From_BModel(phys_world_t * world, int modeli, phys_transform_t * initial_transform, phys_properties_t * properties, qboolean kinematic) {
	vec3_t points[BP_POINTS_SIZE];
	int brushes_max, surfaces_max, brushes_num, surfaces_num;
	CM_NumData(&brushes_max, &surfaces_max);
	int * brushes = new int [brushes_max];
	int * patches = new int [surfaces_max];
	CM_SubmodelIndicies(modeli, brushes, patches, &brushes_num, &surfaces_num);
	
	
	
	phys_object_t * no = new phys_object_t;
	no->properties = *properties;
	no->is_compound = true;
	btCompoundShape * cs = new btCompoundShape;
	
	for (int i = 0; i < brushes_num; i++) {
		if (!(CM_BrushContentFlags(brushes[i]) & CONTENTS_SOLID)) continue;
		int n = CM_CalculateHull(brushes[i], points, BP_POINTS_SIZE);
		
		btConvexHullShape * chs = new btConvexHullShape();
		for (int i = 0; i < n; i++) {
			btVector3 vec { points[i][0], points[i][1], points[i][2] };
			chs->addPoint(vec, false);
		}
		chs->recalcLocalAabb();
		cs->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
	}
	
	for (int i = 0; i < surfaces_num; i++) {
		if (CM_PatchContentFlags(patches[i]) & CONTENTS_SOLID) {
			int width, height;
			CM_PatchMeshPoints(patches[i], points, BP_POINTS_SIZE, &width, &height);
			if (width * height < 4) continue;
			
			for (int x = 0; x < width - 1; x++) {
				for (int y = 0; y < height - 1; y++) {
					btConvexHullShape * chs = new btConvexHullShape();
					chs->addPoint(btVector3 {points[((y + 0) * width) + (x + 0)][0], points[((y + 0) * width) + (x + 0)][1], points[((y + 0) * width) + (x + 0)][2]}, false);
					chs->addPoint(btVector3 {points[((y + 1) * width) + (x + 0)][0], points[((y + 1) * width) + (x + 0)][1], points[((y + 1) * width) + (x + 0)][2]}, false);
					chs->addPoint(btVector3 {points[((y + 0) * width) + (x + 1)][0], points[((y + 0) * width) + (x + 1)][1], points[((y + 0) * width) + (x + 1)][2]}, false);
					chs->addPoint(btVector3 {points[((y + 1) * width) + (x + 1)][0], points[((y + 1) * width) + (x + 1)][1], points[((y + 1) * width) + (x + 1)][2]}, false);
					chs->recalcLocalAabb();
					
					chs->recalcLocalAabb();
					cs->addChildShape( btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {0, 0, 0} }, chs );
				}
			}
		}
	}
	
// 	cs->recalculateLocalAabb();
	no->shape = cs;
	no->motion_state = new btDefaultMotionState { btTransform { 
		btQuaternion { initial_transform->angles[0] * d2r_mult, initial_transform->angles[2] * d2r_mult, initial_transform->angles[1] * d2r_mult }, 
		btVector3 {initial_transform->origin[0], initial_transform->origin[1], initial_transform->origin[2]} } };
	no->shape->calculateLocalInertia(no->properties.mass, no->inertia);
	btRigidBody::btRigidBodyConstructionInfo CI {no->properties.mass, no->motion_state, no->shape, no->inertia};
	no->body = new btRigidBody {CI};
	
	no->set_properties();
	
	if (kinematic) {
		no->body->setCollisionFlags(no->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		no->body->setActivationState(DISABLE_DEACTIVATION);
	}
	
	bullet_world_add_object(world, no);
	
	return no;
}

void Phys_Object_Get_Transform(phys_object_t * obj, phys_transform_t * ia) {
	obj->objlock.lock();
	
	btVector3 origin = obj->trans_cache.getOrigin();
	VectorSet(ia->origin, origin.x(), origin.y(), origin.z());
	
	btMatrix3x3 { obj->trans_cache.getRotation() }.getEulerYPR(ia->angles[1], ia->angles[0], ia->angles[2]);
	VectorScale(ia->angles, r2d_mult, ia->angles);
	
	obj->objlock.unlock();
}

void Phys_Object_Set_Transform(phys_object_t * obj, phys_transform_t const * ia) {
	obj->objlock.lock();
	
	obj->new_trans.setIdentity();
	obj->new_trans.setOrigin( {ia->origin[0], ia->origin[1], ia->origin[2]} );
	obj->new_trans.setRotation( btQuaternion {ia->angles[0] * d2r_mult, ia->angles[2] * d2r_mult, ia->angles[1] * d2r_mult} );
	
	obj->do_trans_update = true;
	
	obj->objlock.unlock();
}

void Phys_Object_Get_Properties(phys_object_t * obj, phys_properties_t * props) {
	obj->objlock.lock();
	*props = obj->properties;
	obj->objlock.unlock();
}

void Phys_Object_Set_Properties(phys_object_t * obj, phys_properties_t const * props) {
	obj->objlock.lock();
	obj->properties = *props;
	obj->set_properties();
	obj->objlock.unlock();
}
