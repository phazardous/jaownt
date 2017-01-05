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

void Phys_World_Add_Current_Map(phys_world_t * world, void * world_token);

void Phys_World_Remove_Object(phys_world_t * w, phys_object_t * obj);

phys_object_t * Phys_Object_Create_Box(phys_world_t * w, vec3_t mins, vec3_t maxs, phys_transform_t * initial_transform, phys_properties_t * properties);
phys_object_t * Phys_Object_Create_From_Obj(phys_world_t * world, char const * path, phys_transform_t * initial_transform, phys_properties_t * properties, float scale);
phys_object_t * Phys_Object_Create_From_BModel(phys_world_t * world, int modeli, phys_transform_t * initial_transform, phys_properties_t * properties);
phys_object_t * Phys_Object_Create_Capsule(phys_world_t * w, float cylinder_height, float radius, float v_center_offs, phys_transform_t * initial_transform, phys_properties_t * properties);

void Phys_Object_Get_Transform(phys_object_t *, phys_transform_t *);
void Phys_Object_Set_Transform(phys_object_t *, phys_transform_t const *);

phys_properties_t * Phys_Object_Get_Properties(phys_object_t *);
void Phys_Object_Set_Properties(phys_object_t *);

void Phys_Object_Force(phys_object_t *, vec3_t lin, vec3_t ang);
void Phys_Object_Impulse(phys_object_t *, vec3_t lin, vec3_t ang);

void Phys_Obj_Set_Linear_Velocity(phys_object_t *, vec3_t lin);
void Phys_Obj_Get_Linear_Velocity(phys_object_t *, vec3_t lin);

void Phys_Obj_Set_Angular_Velocity(phys_object_t *, vec3_t ang);
void Phys_Obj_Get_Angular_Velocity(phys_object_t *, vec3_t ang);

#endif //PHYS_PUBLIC_HPP
