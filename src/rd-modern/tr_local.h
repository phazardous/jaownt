#pragma once

#include "rd-common/tr_public.h"
#include "qcommon/q_shared.h"

#include "glad.h"

extern refimport_t * ri;
extern glconfig_t glConfig;

extern cvar_t * r_aspectCorrectFonts;

void RE_SetColor( const float *rgba );
void RE_StretchPic ( float x, float y, float w, float h,
					  float s1, float t1, float s2, float t2, qhandle_t hShader );
qhandle_t RE_RegisterShaderNoMip( char const * );
