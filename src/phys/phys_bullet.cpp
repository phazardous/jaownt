#include "phys_public.hpp"

#include "qcommon/cm_public.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_set>

#include <btBulletDynamicsCommon.h>

static constexpr float d2r_mult = 0.0174532925f;
static constexpr float r2d_mult = 57.2957795f;

struct phys_world_s {
	btBroadphaseInterface * broadphase;
	btDefaultCollisionConfiguration * config;
	btCollisionDispatcher * dispatcher;
	btSequentialImpulseConstraintSolver * solver;
	btDiscreteDynamicsWorld * world;
	
	int step_resolution = 1000;
	
	std::unordered_set<phys_object_t *> objects;
	
	std::mutex simlock {};
	
	std::atomic_bool run {true};
	std::atomic_int advance {0};
	std::thread * thr = nullptr;
};

struct phys_object_s {
	btScalar mass {1};
	btVector3 inertia {0, 0, 0};
	btCollisionShape * shape = nullptr;
	btDefaultMotionState * motion_state = nullptr;
	btRigidBody::btRigidBodyConstructionInfo * CI = nullptr;
	btRigidBody * body = nullptr;
	
	std::mutex objlock {};
	
	bool do_trans_update = false;
	btTransform new_trans {};
	btTransform trans_cache {};
};

static void bullet_world_substep_cb(btDynamicsWorld *world, btScalar timeStep) {
	
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
				obj->body->setWorldTransform(obj->new_trans);
			}
			obj->objlock.unlock();
		}
		
		btScalar advc = adv / 1000.0f;
		w->world->stepSimulation(advc, w->step_resolution, 1.0f / w->step_resolution);
		
		for (phys_object_t * obj : w->objects) {
			obj->objlock.lock();
			obj->body->getMotionState()->getWorldTransform(obj->trans_cache);
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
	
	nw->world->setGravity( {0, 0, -500} );
	nw->world->setInternalTickCallback(bullet_world_substep_cb);
	
	btStaticPlaneShape * ps = new btStaticPlaneShape( {0, 0, 1}, 1);
	btMotionState * ms = new btDefaultMotionState {};
	btRigidBody * rb = new btRigidBody { {0, ms, ps, {0, 0, 0}} };
	nw->world->addRigidBody(rb);
	
	nw->thr = new std::thread {bullet_world_thread_loop, nw};
	
	return nw;
}

void Phys_World_Destroy(phys_world_t * w) {
	
	w->run.store(false);
	w->thr->join();
	
	delete w->thr;
	
	for (phys_object_t * obj : w->objects) {
		delete obj;
	}
	
	w->objects.clear();
	
	delete w->world;
	delete w->solver;
	delete w->dispatcher;
	delete w->config;
	delete w->broadphase;
	delete w;
}

void Phys_World_Advance(phys_world_t * world, int time) {
	world->advance += time;
}

phys_object_t * Phys_Object_Create_From_Obj(phys_world_t * world, char const * path, vec3_t pos, float mass) {
	phys_object_t * no = new phys_object_t;
	
	no->mass = mass;
	btConvexHullShape * chs = new btConvexHullShape;

	objSurface_t * surf = CM_LoadObj(path);
	if (!surf) return nullptr;
	
	for (int i = 0; i < surf->numVerts; i++) {
		chs->addPoint( {surf->verts[i*3], surf->verts[i*3+1], surf->verts[i*3+2]}, false );
	}
	
	chs->recalcLocalAabb();
	no->shape = chs;
	no->motion_state = new btDefaultMotionState { btTransform { btQuaternion {0, 0, 0, 1}, btVector3 {pos[0], pos[1], pos[2]} } };
	no->shape->calculateLocalInertia(no->mass, no->inertia);
	btRigidBody::btRigidBodyConstructionInfo CI {no->mass, no->motion_state, no->shape, no->inertia};
	no->body = new btRigidBody {CI};
	no->body->setDamping(0.125f, 0.125f);
	
	world->simlock.lock();
	world->world->addRigidBody(no->body);
	world->objects.insert(no);
	no->trans_cache = no->body->getWorldTransform();
	world->simlock.unlock();
	
	return no;
}

void Phys_Object_Get_Transform(phys_object_t * obj, phys_interactor_t * ia) {
	obj->objlock.lock();
	btVector3 origin = obj->trans_cache.getOrigin();
	VectorSet(ia->origin, origin.x(), origin.y(), origin.z());
	btMatrix3x3 { obj->trans_cache.getRotation() }.getEulerYPR(ia->angles[1], ia->angles[0], ia->angles[2]);
	VectorScale(ia->angles, r2d_mult, ia->angles);
	obj->objlock.unlock();
}

void Phys_Object_Set_Transform(phys_object_t * obj, phys_interactor_t const * ia) {
	obj->objlock.lock();
	obj->new_trans.setOrigin( {ia->origin[0], ia->origin[1], ia->origin[2]} );
	obj->new_trans.setRotation( btQuaternion {ia->angles[1] * d2r_mult, ia->angles[0] * d2r_mult, ia->angles[2] * d2r_mult} );
	obj->do_trans_update = true;
	obj->objlock.unlock();
}
