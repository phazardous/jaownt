#ifndef PHYS_PUBLIC_HPP
#define PHYS_PUBLIC_HPP

#include "qcommon/q_shared.h"



phys_world_t * Phys_World_Create();
void Phys_World_Destroy(phys_world_t *);
void Phys_World_Advance(phys_world_t * world, int time);

phys_object_t * Phys_Object_Create_From_Obj(phys_world_t * world, char const * path, vec3_t pos, float mass);
void Phys_Object_Get_Transform(phys_object_t *, phys_interactor_t *);
void Phys_Object_Set_Transform(phys_object_t *, phys_interactor_t const *);

#endif //PHYS_PUBLIC_HPP
