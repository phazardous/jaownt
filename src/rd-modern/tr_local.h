#pragma once

#include "rd-common/tr_public.h"
#include "qcommon/q_shared.h"

#define GL_INDEX_TYPE		GL_UNSIGNED_INT
typedef unsigned int glIndex_t;

#define LL(x) x=LittleLong(x)

// a trMiniRefEntity_t has all the information passed in by
// the client game, other info will come from it's parent main ref entity
typedef struct
{
	miniRefEntity_t	e;
} trMiniRefEntity_t;

// a trRefEntity_t has all the information passed in by
// the client game, as well as some locally derived info
typedef struct trRefEntity_s {
	refEntity_t	e;

	float		axisLength;		// compensate for non-normalized axis

	qboolean	needDlights;	// true for bmodels that touch a dlight
	qboolean	lightingCalculated;
	vec3_t		lightDir;		// normalized direction towards light
	vec3_t		ambientLight;	// color normalized to 0-255
	int			ambientLightInt;	// 32 bit rgba packed
	vec3_t		directedLight;
	int			dlightBits;
} trRefEntity_t;


typedef struct orientationr_s {
	vec3_t		origin;			// in world coordinates
	matrix3_t	axis;		// orientation in world
	vec3_t		viewOrigin;		// viewParms->or.origin in local coordinates
	float		modelMatrix[16];
} orientationr_t;

/*
Ghoul2 Insert Start
*/
 // bogus little registration system for hit and location based damage files in hunk memory
typedef struct hitMatReg_s {
	byte	*loc;
	int		width;
	int		height;
	char	name[MAX_QPATH];
} hitMatReg_t;

#define MAX_HITMAT_ENTRIES 1000

extern hitMatReg_t		hitMatReg[MAX_HITMAT_ENTRIES];

/*
Ghoul2 Insert End
*/


// trRefdef_t holds everything that comes in refdef_t,
// as well as the locally generated scene information
typedef struct trRefdef_s {
	int			x, y, width, height;
	float		fov_x, fov_y;
	vec3_t		vieworg;
	matrix3_t	viewaxis;		// transformation matrix

	int			time;				// time in milliseconds for shader effects and other time dependent rendering issues
	int			frametime;
	int			rdflags;			// RDF_NOWORLDMODEL, etc

	// 1 bits will prevent the associated area from rendering at all
	byte		areamask[MAX_MAP_AREA_BYTES];
	qboolean	areamaskModified;	// qtrue if areamask changed since last scene

	float		floatTime;			// tr.refdef.time / 1000.0

	// text messages for deform text shaders
	char		text[MAX_RENDER_STRINGS][MAX_RENDER_STRING_LENGTH];

	int			num_entities;
	trRefEntity_t	*entities;
	trMiniRefEntity_t	*miniEntities;

	int			num_dlights;
	struct dlight_s	*dlights;

	int			numPolys;
	struct srfPoly_s	*polys;

	int			numDrawSurfs;
	struct drawSurf_s	*drawSurfs;
} trRefdef_t;


typedef struct viewParms_s {
	orientationr_t	ori;				// Can't use "or" as it is a reserved word with gcc DREWS 2/2/2002
	orientationr_t	world;
	vec3_t		pvsOrigin;			// may be different than or.origin for portals
	qboolean	isPortal;			// true if this view is through a portal
	qboolean	isMirror;			// the portal is a mirror, invert the face culling
	int			frameSceneNum;		// copied from tr.frameSceneNum
	int			frameCount;			// copied from tr.frameCount
	cplane_t	portalPlane;		// clip anything behind this if mirroring
	int			viewportX, viewportY, viewportWidth, viewportHeight;
	float		fovX, fovY;
	float		projectionMatrix[16];
	cplane_t	frustum[4];
	vec3_t		visBounds[2];
	float		zFar;
} viewParms_t;

typedef struct trGlobals_s {
	window_t				window;

	viewParms_t				viewParms;
	trRefdef_t				refdef;
	
} trGlobals_t;

extern trGlobals_t tr;
extern refimport_t * ri;
extern glconfig_t glConfig;

extern cvar_t * r_aspectCorrectFonts;
extern cvar_t	*r_verbose;				// used for verbose debug spew
extern	cvar_t	*r_noServerGhoul2;
#ifdef _DEBUG
	extern	cvar_t	*r_noPrecacheGLA;
#endif

qhandle_t TRM_RegisterServerModel( const char *name  );
qhandle_t TRM_RegisterModel( const char *name  );
qhandle_t TRM_RegisterShader( const char *name  );

skin_t * TRM_GetSkinByHandle(qhandle_t);
model_t * TRM_GetModelByHandle( qhandle_t hModel );
// shader_t * TRM_GetShaderByHandle(qhandle_t);

qboolean ShaderHashTableExists();
void Multiply_3x4Matrix(mdxaBone_t *out, mdxaBone_t *in2, mdxaBone_t *in);

void RE_SetColor( const float *rgba );
void RE_StretchPic ( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader );
qhandle_t RE_RegisterShaderNoMip( char const * );

/* tr_misc */
float ProjectRadius( float r, vec3_t location );
