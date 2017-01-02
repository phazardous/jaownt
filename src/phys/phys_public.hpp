#ifndef PHYS_PUBLIC_HPP
#define PHYS_PUBLIC_HPP

#include "qcommon/q_shared.h"

extern cvar_t * phys_playerclip;

void Com_Phys_Init();

phys_world_t * Phys_World_Create(phys_touch_callback touch_cb);
void Phys_World_Destroy(phys_world_t *);
void Phys_World_Advance(phys_world_t * world, int time);

void Phys_World_Set_Resolution(phys_world_t * world, unsigned int resolution);
void Phys_World_Set_Gravity(phys_world_t * world, float gravity);

void Phys_World_Add_Current_Map(phys_world_t * world);

void Phys_World_Remove_Object(phys_world_t * w, phys_object_t * obj);

phys_object_t * Phys_Object_Create_Box(phys_world_t * w, vec3_t mins, vec3_t maxs, phys_transform_t * initial_transform, phys_properties_t * properties, qboolean kinematic);
phys_object_t * Phys_Object_Create_From_Obj(phys_world_t * world, char const * path, phys_transform_t * initial_transform, phys_properties_t * properties, float scale, qboolean kinematic);
phys_object_t * Phys_Object_Create_From_BModel(phys_world_t * world, int modeli, phys_transform_t * initial_transform, phys_properties_t * properties, qboolean kinematic);

void Phys_Object_Get_Transform(phys_object_t *, phys_transform_t *);
void Phys_Object_Set_Transform(phys_object_t *, phys_transform_t const *);

void Phys_Object_Get_Properties(phys_object_t *, phys_properties_t *);
void Phys_Object_Set_Properties(phys_object_t *, phys_properties_t const *);

#endif //PHYS_PUBLIC_HPP
