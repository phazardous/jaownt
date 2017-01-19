#include "rd-common/tr_public.h"
#include "tr_local.h"
#include "ghoul2/g2_local.h"
#include "rendm_local.h"
#include "rd-common/tr_font.h"

#include <cstdarg>

trGlobals_t tr;
refimport_t * ri;
glconfig_t glConfig;

cvar_t	*r_noServerGhoul2 = nullptr;
cvar_t	*r_Ghoul2AnimSmooth = nullptr;
cvar_t	*r_Ghoul2UnSqashAfterSmooth = nullptr;

cvar_t	*broadsword = nullptr;
cvar_t	*broadsword_kickbones = nullptr;
cvar_t	*broadsword_kickorigin = nullptr;
cvar_t	*broadsword_playflop = nullptr;
cvar_t	*broadsword_dontstopanim = nullptr;
cvar_t	*broadsword_waitforshot = nullptr;
cvar_t	*broadsword_smallbbox = nullptr;
cvar_t	*broadsword_extra1 = nullptr;
cvar_t	*broadsword_extra2 = nullptr;
cvar_t	*broadsword_effcorr = nullptr;
cvar_t	*broadsword_ragtobase = nullptr;
cvar_t	*broadsword_dircap = nullptr;

cvar_t * se_language;

cvar_t * r_aspectCorrectFonts;
cvar_t * r_verbose;

static void TRM_Cvars() {
	
	r_noServerGhoul2 = ri->Cvar_Get("r_noserverghoul2", "0", CVAR_CHEAT, "");
	r_Ghoul2AnimSmooth = ri->Cvar_Get("r_ghoul2animsmooth", "0.3", CVAR_NONE, "");
	r_Ghoul2UnSqashAfterSmooth = ri->Cvar_Get("r_ghoul2unsqashaftersmooth", "1", CVAR_NONE, "");
	broadsword = ri->Cvar_Get("broadsword", "0", CVAR_NONE, "");
	broadsword_kickbones = ri->Cvar_Get("broadsword_kickbones", "1", CVAR_NONE, "");
	broadsword_kickorigin = ri->Cvar_Get("broadsword_kickorigin", "1", CVAR_NONE, "");
	broadsword_dontstopanim = ri->Cvar_Get("broadsword_dontstopanim", "0", CVAR_NONE, "");
	broadsword_waitforshot = ri->Cvar_Get("broadsword_waitforshot", "0", CVAR_NONE, "");
	broadsword_playflop = ri->Cvar_Get("broadsword_playflop", "1", CVAR_NONE, "");
	broadsword_smallbbox = ri->Cvar_Get("broadsword_smallbbox", "0", CVAR_NONE, "");
	broadsword_extra1 = ri->Cvar_Get("broadsword_extra1", "0", CVAR_NONE, "");
	broadsword_extra2 = ri->Cvar_Get("broadsword_extra2", "0", CVAR_NONE, "");
	broadsword_effcorr = ri->Cvar_Get("broadsword_effcorr", "1", CVAR_NONE, "");
	broadsword_ragtobase = ri->Cvar_Get("broadsword_ragtobase", "2", CVAR_NONE, "");
	broadsword_dircap = ri->Cvar_Get("broadsword_dircap", "64", CVAR_NONE, "");

	se_language = ri->Cvar_Get("se_language", "english", CVAR_ARCHIVE|CVAR_NORESTART, "");
	r_aspectCorrectFonts = ri->Cvar_Get("r_aspectCorrectFonts", "0", CVAR_ARCHIVE, "");
	r_verbose = ri->Cvar_Get("r_verbose", "0", CVAR_CHEAT, "");
}

void TRM_Shutdown( qboolean destroyWindow, qboolean restarting  ) {
	rendm::term();
	if ( restarting ) {
		SaveGhoul2InfoArray();
	}
	if (destroyWindow) ri->WIN_Shutdown();
	R_ShutdownFonts();
}

static bool glad_initialized = false;

void TRM_BeginRegistration( glconfig_t *config  ) {
	
	Com_Printf("================================================================\n");
	Com_Printf("Initializing Renderer\n");
	Com_Printf("================================\n");
	
	windowDesc_t windesc { GRAPHICS_API_OPENGL };
	tr.window = ri->WIN_Init(&windesc, config);
	
	if (!glad_initialized) { // TRM_BeginRegistration can be called more than once, but this only gets called once for the entire program lifetime
		gladLoadGL();
		glad_initialized = true;
	}
	
	config->vendor_string = (const char *) glGetString (GL_VENDOR);
	config->renderer_string = (const char *) glGetString (GL_RENDERER);
	config->version_string = (const char *) glGetString (GL_VERSION);
	config->extensions_string = (const char *) glGetString (GL_EXTENSIONS);
	
	Com_Printf("Vendor: %s\nRenderer: %s\nVersion: %s\n", config->vendor_string, config->renderer_string, config->version_string);
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &config->maxTextureSize);
	config->maxActiveTextures = 0; // old GL
	if (GLAD_GL_EXT_texture_filter_anisotropic)
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &config->maxTextureFilterAnisotropy);
	else config->maxTextureFilterAnisotropy = 0;
	
	glEnable(GL_SCISSOR_TEST);

	config->isFullscreen = qfalse;
	config->stereoEnabled = qfalse;
	
	glConfig = *config;
	
	TRM_Cvars();
	
	R_InitFonts();
	
	if (!rendm::init()) {
		srcprintf_fatal("Failed to initialize RendM (rd-modern)");
	}
	
	Com_Printf("================================================================\n");
}

qhandle_t TRM_RegisterModel( const char *name  ) {
	return rendm::model::reg(name);
}

qhandle_t TRM_RegisterServerModel( const char *name  ) {
	return 0; //TODO
}

qhandle_t TRM_RegisterSkin( const char *name  ) {
	return 0; //TODO
}

qhandle_t TRM_RegisterServerSkin( const char *name  ) {
	return 0; //TODO
}

qhandle_t TRM_RegisterShader( const char *name  ) {
	return rendm::shader::reg(name, true);
}

qhandle_t TRM_RegisterShaderNoMip( const char *name  ) {
	return rendm::shader::reg(name, true);
}

const char * TRM_ShaderNameFromIndex( int index  ) {
	return nullptr; //TODO
}

void TRM_LoadWorld( const char *name  ) {
	//TODO
}

void TRM_SetWorldVisData( const byte *vis  ) {
	//TODO
}

void TRM_EndRegistration( void  ) {
	// do nothing I think
}

void TRM_ClearScene( void  ) {
	//TODO
}

void TRM_ClearDecals( void  ) {
	//TODO
}

void TRM_AddRefEntityToScene( const refEntity_t *re  ) {
	//TODO
}

void TRM_AddMiniRefEntityToScene( const miniRefEntity_t *re  ) {
	//TODO
}

void TRM_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts, int num  ) {
	//TODO
}

void TRM_AddDecalToScene( qhandle_t shader, const vec3_t origin, const vec3_t dir, float orientation, float r, float g, float b, float a, qboolean alphaFade, float radius, qboolean temporary  ) {
	//TODO
}

int TRM_LightForPoint( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir  ) {
	return 0; //TODO
}

void TRM_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b  ) {
	//TODO
}

void TRM_AddAdditiveLightToScene( const vec3_t org, float intensity, float r, float g, float b  ) {
	//TODO
}

void TRM_RenderScene( const refdef_t *fd  ) {
	rendm::draw(fd);
}

void TRM_SetColor( const float *rgba  ) {
	if (rgba) rendm::globals.color_mod = {rgba[0], rgba[1], rgba[2], rgba[3]};
	else rendm::globals.color_mod = {1, 1, 1, 1};
}

static const qm::mat4 projection_2d = qm::mat4::ortho(0, 480, 0, 640, 0, 1);

void TRM_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader  ) {
	
	if (w == 0) {
		w = h * (640.0f / 480.0f);
	}
	
	//assert(w != 0 && h != 0);
	
	qm::mat4 m = qm::mat4::scale(w, h, 1);
	m *= qm::mat4::translate(x, y, 0);
	
	
	qm::mat3 uv = qm::mat3::scale(s2 - s1, t2 - t1);
	uv *= qm::mat3::translate(s1, t1);
	
	rendm::add_sprite( projection_2d * m, uv, hShader );
}

void TRM_DrawRotatePic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, float a1, qhandle_t hShader  ) {
	//TODO
}

void TRM_DrawRotatePic2( float x, float y, float w, float h, float s1, float t1, float s2, float t2, float a1, qhandle_t hShader  ) {
	//TODO
}

void TRM_DrawStretchRaw( int x, int y, int w, int h, int cols, int rows, const byte *data, int client, qboolean dirty  ) {
	//TODO
}

void TRM_UploadCinematic( int cols, int rows, const byte *data, int client, qboolean dirty  ) {
	//TODO
}

void TRM_BeginFrame( stereoFrame_t stereoFrame  ) {
	rendm::begin_frame();
}

void TRM_EndFrame( int *frontEndMsec, int *backEndMsec  ) {
	rendm::end_frame();
}

int TRM_MarkFragments( int numPoints, const vec3_t *points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer  ) {
	return 0; //TODO
}

int TRM_LerpTag( orientation_t *tag,  qhandle_t model, int startFrame, int endFrame, float frac, const char *tagName  ) {
	return 0; //TODO
}

void TRM_ModelBounds( qhandle_t model, vec3_t mins, vec3_t maxs  ) {
	//TODO
}

void TRM_ModelBoundsRef( refEntity_t *model, vec3_t mins, vec3_t maxs  ) {
	//TODO
}

qhandle_t TRM_RegisterFont( const char *fontName  ) {
	return RE_RegisterFont(fontName);
}

int TRM_Font_StrLenPixels( const char *text, const int iFontIndex, const float scale  ) {
	return RE_Font_StrLenPixels( text, iFontIndex, scale );
}

int TRM_Font_StrLenChars( const char *text  ) {
	return RE_Font_StrLenChars(text);
}

int TRM_Font_HeightPixels( const int iFontIndex, const float scale  ) {
	return RE_Font_HeightPixels(iFontIndex, scale);
}

void TRM_Font_DrawString( int ox, int oy, const char *text, const float *rgba, const int setIndex, int iCharLimit, const float scale  ) {
	RE_Font_DrawString(ox, oy, text, rgba, setIndex, iCharLimit, scale);
}

qboolean TRM_Language_IsAsian( void  ) {
	return Language_IsAsian();
}

qboolean TRM_Language_UsesSpaces( void  ) {
	return Language_UsesSpaces();
}

unsigned int TRM_AnyLanguage_ReadCharFromString( const char *psText, int *piAdvanceCount, qboolean *pbIsTrailingPunctuation/* = NULL*/  ) {
	return AnyLanguage_ReadCharFromString(psText, piAdvanceCount, pbIsTrailingPunctuation);
}

void TRM_RemapShader( const char *oldShader, const char *newShader, const char *offsetTime  ) {
	//TODO
}

qboolean TRM_GetEntityToken( char *buffer, int size  ) {
	return qfalse; //TODO
}

qboolean TRM_inPVS( const vec3_t p1, const vec3_t p2, byte *mask  ) {
	return qfalse; //TODO
}

void TRM_GetLightStyle( int style, color4ub_t color  ) {
	//TODO
}

void TRM_SetLightStyle( int style, int color  ) {
	//TODO
}


void TRM_GetBModelVerts( int bmodelIndex, vec3_t *vec, vec3_t normal  ) {
	//TODO
}


void TRM_SetRangedFog( float range  ) {
	//TODO
}

void TRM_SetRefractionProperties( float distortionAlpha, float distortionStretch, qboolean distortionPrePost, qboolean distortionNegate  ) {
	//TODO
}

float TRM_GetDistanceCull( void  ) {
	return 0; //TODO
}

void TRM_GetRealRes( int *w, int *h  ) {
	//TODO
}

void TRM_AutomapElevationAdjustment( float newHeight  ) {
	//TODO
}

qboolean TRM_InitializeWireframeAutomap( void  ) {
	return qfalse;
}

void TRM_AddWeatherZone( vec3_t mins, vec3_t maxs  ) {
	//TODO
}

void TRM_WorldEffectCommand( const char *command  ) {
	//TODO
}

void TRM_RegisterMedia_LevelLoadBegin( const char *psMapName, ForceReload_e eForceReload  ) {
	//TODO
}

void TRM_RegisterMedia_LevelLoadEnd( void  ) {
	//TODO
}

int TRM_RegisterMedia_GetLevel( void  ) {
	return 0; //TODO
}

qboolean TRM_RegisterImages_LevelLoadEnd( void  ) {
	return qfalse; //TODO
}

qboolean TRM_RegisterModels_LevelLoadEnd( qboolean bDeleteEverythingNotUsedThisLevel  ) {
	return qfalse; //TODO
}


void TRM_TakeVideoFrame( int h, int w, byte* captureBuffer, byte *encodeBuffer, qboolean motionJpeg  ) {
	//TODO
}

void TRM_InitSkins( void  ) {
	//TODO
}

void TRM_InitShaders( qboolean server  ) {
	//TODO
}

void TRM_SVModelInit( void  ) {
	//TODO
}

void TRM_HunkClearCrap( void  ) {
	//TODO
}

void G2API_BoltMatrixReconstruction( qboolean reconstruct  ) {
	//TODO
}

void G2API_BoltMatrixSPMethod( qboolean spMethod  ) {
	//TODO
}

float TRM_Font_StrLenPixelsNew( const char *text, const int iFontIndex, const float scale  ) {
	return RE_Font_StrLenPixelsNew(text, iFontIndex, scale);
}


extern "C" Q_EXPORT refexport_t* QDECL GetRefAPI( int apiVersion, refimport_t *rimp ) {
	static refexport_t re;

	assert( rimp );
	ri = rimp;

	memset( &re, 0, sizeof( re ) );

	if ( apiVersion != REF_API_VERSION ) {
		ri->Printf( PRINT_ALL,  "Mismatched REF_API_VERSION: expected %i, got %i\n", REF_API_VERSION, apiVersion );
		return NULL;
	}

	re.Shutdown								= TRM_Shutdown;
	re.BeginRegistration					= TRM_BeginRegistration;
 	re.RegisterModel						= TRM_RegisterModel;
 	re.RegisterServerModel					= TRM_RegisterServerModel;
 	re.RegisterSkin							= TRM_RegisterSkin;
 	re.RegisterServerSkin					= TRM_RegisterServerSkin;
 	re.RegisterShader						= TRM_RegisterShader;
 	re.RegisterShaderNoMip					= TRM_RegisterShaderNoMip;
 	re.ShaderNameFromIndex					= TRM_ShaderNameFromIndex;
 	re.LoadWorld							= TRM_LoadWorld;
 	re.SetWorldVisData						= TRM_SetWorldVisData;
 	re.EndRegistration						= TRM_EndRegistration;
 	re.BeginFrame							= TRM_BeginFrame;
 	re.EndFrame								= TRM_EndFrame;
 	re.MarkFragments						= TRM_MarkFragments;
 	re.LerpTag								= TRM_LerpTag;
 	re.ModelBounds							= TRM_ModelBounds;
 	re.DrawRotatePic						= TRM_DrawRotatePic;
 	re.DrawRotatePic2						= TRM_DrawRotatePic2;
 	re.ClearScene							= TRM_ClearScene;
 	re.ClearDecals							= TRM_ClearDecals;
 	re.AddRefEntityToScene					= TRM_AddRefEntityToScene;
 	re.AddMiniRefEntityToScene				= TRM_AddMiniRefEntityToScene;
 	re.AddPolyToScene						= TRM_AddPolyToScene;
 	re.AddDecalToScene						= TRM_AddDecalToScene;
 	re.LightForPoint						= TRM_LightForPoint;
 	re.AddLightToScene						= TRM_AddLightToScene;
 	re.AddAdditiveLightToScene				= TRM_AddAdditiveLightToScene;
 
 	re.RenderScene							= TRM_RenderScene;
 	re.SetColor								= TRM_SetColor;
 	re.DrawStretchPic						= TRM_DrawStretchPic;
 	re.DrawStretchRaw						= TRM_DrawStretchRaw;
 	re.UploadCinematic						= TRM_UploadCinematic;
 
 	re.RegisterFont							= TRM_RegisterFont;
 	re.Font_StrLenPixels					= TRM_Font_StrLenPixels;
 	re.Font_StrLenChars						= TRM_Font_StrLenChars;
 	re.Font_HeightPixels					= TRM_Font_HeightPixels;
 	re.Font_DrawString						= TRM_Font_DrawString;
 	re.Language_IsAsian						= TRM_Language_IsAsian;
 	re.Language_UsesSpaces					= TRM_Language_UsesSpaces;
 	re.AnyLanguage_ReadCharFromString		= TRM_AnyLanguage_ReadCharFromString;
 
 	re.RemapShader							= TRM_RemapShader;
 	re.GetEntityToken						= TRM_GetEntityToken;
 	re.inPVS								= TRM_inPVS;
 	re.GetLightStyle						= TRM_GetLightStyle;
 	re.SetLightStyle						= TRM_SetLightStyle;
 	re.GetBModelVerts						= TRM_GetBModelVerts;
 
 	re.SetRangedFog							= TRM_SetRangedFog;
 	re.SetRefractionProperties				= TRM_SetRefractionProperties;
 	re.GetDistanceCull						= TRM_GetDistanceCull;
 	re.GetRealRes							= TRM_GetRealRes;
 	re.AutomapElevationAdjustment			= TRM_AutomapElevationAdjustment;
 	re.InitializeWireframeAutomap			= TRM_InitializeWireframeAutomap;
 	re.AddWeatherZone						= TRM_AddWeatherZone;
 	re.WorldEffectCommand					= TRM_WorldEffectCommand;
 	re.RegisterMedia_LevelLoadBegin			= TRM_RegisterMedia_LevelLoadBegin;
 	re.RegisterMedia_LevelLoadEnd			= TRM_RegisterMedia_LevelLoadEnd;
 	re.RegisterMedia_GetLevel				= TRM_RegisterMedia_GetLevel;
 	re.RegisterImages_LevelLoadEnd			= TRM_RegisterImages_LevelLoadEnd;
 	re.RegisterModels_LevelLoadEnd			= TRM_RegisterModels_LevelLoadEnd;
 
 	re.TakeVideoFrame						= TRM_TakeVideoFrame;
 
 	re.InitSkins							= TRM_InitSkins;
 	re.InitShaders							= TRM_InitShaders;
 	re.SVModelInit							= TRM_SVModelInit;
 	re.HunkClearCrap						= TRM_HunkClearCrap;
 
 	re.G2API_AddBolt						= G2API_AddBolt;
 	re.G2API_AddBoltSurfNum					= G2API_AddBoltSurfNum;
 	re.G2API_AddSurface						= G2API_AddSurface;
 	re.G2API_AnimateG2ModelsRag				= G2API_AnimateG2ModelsRag;
 	re.G2API_AttachEnt						= G2API_AttachEnt;
 	re.G2API_AttachG2Model					= G2API_AttachG2Model;
 	re.G2API_AttachInstanceToEntNum			= G2API_AttachInstanceToEntNum;
 	re.G2API_AbsurdSmoothing				= G2API_AbsurdSmoothing;
 	re.G2API_BoltMatrixReconstruction		= G2API_BoltMatrixReconstruction;
 	re.G2API_BoltMatrixSPMethod				= G2API_BoltMatrixSPMethod;
 	re.G2API_CleanEntAttachments			= G2API_CleanEntAttachments;
 	re.G2API_CleanGhoul2Models				= G2API_CleanGhoul2Models;
 	re.G2API_ClearAttachedInstance			= G2API_ClearAttachedInstance;
 	re.G2API_CollisionDetect				= G2API_CollisionDetect;
 	re.G2API_CollisionDetectCache			= G2API_CollisionDetectCache;
 	re.G2API_CopyGhoul2Instance				= G2API_CopyGhoul2Instance;
 	re.G2API_CopySpecificG2Model			= G2API_CopySpecificG2Model;
 	re.G2API_DetachG2Model					= G2API_DetachG2Model;
 	re.G2API_DoesBoneExist					= G2API_DoesBoneExist;
 	re.G2API_DuplicateGhoul2Instance		= G2API_DuplicateGhoul2Instance;
 	re.G2API_FreeSaveBuffer					= G2API_FreeSaveBuffer;
 	re.G2API_GetAnimFileName				= G2API_GetAnimFileName;
 	re.G2API_GetAnimFileNameIndex			= G2API_GetAnimFileNameIndex;
 	re.G2API_GetAnimRange					= G2API_GetAnimRange;
 	re.G2API_GetBoltMatrix					= G2API_GetBoltMatrix;
 	re.G2API_GetBoneAnim					= G2API_GetBoneAnim;
 	re.G2API_GetBoneIndex					= G2API_GetBoneIndex;
 	re.G2API_GetGhoul2ModelFlags			= G2API_GetGhoul2ModelFlags;
 	re.G2API_GetGLAName						= G2API_GetGLAName;
 	re.G2API_GetModelName					= G2API_GetModelName;
 	re.G2API_GetParentSurface				= G2API_GetParentSurface;
 	re.G2API_GetRagBonePos					= G2API_GetRagBonePos;
 	re.G2API_GetSurfaceIndex				= G2API_GetSurfaceIndex;
 	re.G2API_GetSurfaceName					= G2API_GetSurfaceName;
 	re.G2API_GetSurfaceOnOff				= G2API_GetSurfaceOnOff;
 	re.G2API_GetSurfaceRenderStatus			= G2API_GetSurfaceRenderStatus;
 	re.G2API_GetTime						= G2API_GetTime;
 	re.G2API_Ghoul2Size						= G2API_Ghoul2Size;
 	re.G2API_GiveMeVectorFromMatrix			= G2API_GiveMeVectorFromMatrix;
 	re.G2API_HasGhoul2ModelOnIndex			= G2API_HasGhoul2ModelOnIndex;
 	re.G2API_HaveWeGhoul2Models				= G2API_HaveWeGhoul2Models;
 	re.G2API_IKMove							= G2API_IKMove;
 	re.G2API_InitGhoul2Model				= G2API_InitGhoul2Model;
 	re.G2API_IsGhoul2InfovValid				= G2API_IsGhoul2InfovValid;
 	re.G2API_IsPaused						= G2API_IsPaused;
 	re.G2API_ListBones						= G2API_ListBones;
 	re.G2API_ListSurfaces					= G2API_ListSurfaces;
 	re.G2API_LoadGhoul2Models				= G2API_LoadGhoul2Models;
 	re.G2API_LoadSaveCodeDestructGhoul2Info	= G2API_LoadSaveCodeDestructGhoul2Info;
 	re.G2API_OverrideServerWithClientData	= G2API_OverrideServerWithClientData;
 	re.G2API_PauseBoneAnim					= G2API_PauseBoneAnim;
 	re.G2API_PrecacheGhoul2Model			= G2API_PrecacheGhoul2Model;
 	re.G2API_RagEffectorGoal				= G2API_RagEffectorGoal;
 	re.G2API_RagEffectorKick				= G2API_RagEffectorKick;
 	re.G2API_RagForceSolve					= G2API_RagForceSolve;
 	re.G2API_RagPCJConstraint				= G2API_RagPCJConstraint;
 	re.G2API_RagPCJGradientSpeed			= G2API_RagPCJGradientSpeed;
 	re.G2API_RemoveBolt						= G2API_RemoveBolt;
 	re.G2API_RemoveBone						= G2API_RemoveBone;
 	re.G2API_RemoveGhoul2Model				= G2API_RemoveGhoul2Model;
 	re.G2API_RemoveGhoul2Models				= G2API_RemoveGhoul2Models;
 	re.G2API_RemoveSurface					= G2API_RemoveSurface;
 	re.G2API_ResetRagDoll					= G2API_ResetRagDoll;
 	re.G2API_SaveGhoul2Models				= G2API_SaveGhoul2Models;
 	re.G2API_SetBoltInfo					= G2API_SetBoltInfo;
 	re.G2API_SetBoneAngles					= G2API_SetBoneAngles;
 	re.G2API_SetBoneAnglesIndex				= G2API_SetBoneAnglesIndex;
 	re.G2API_SetBoneAnglesMatrix			= G2API_SetBoneAnglesMatrix;
 	re.G2API_SetBoneAnglesMatrixIndex		= G2API_SetBoneAnglesMatrixIndex;
 	re.G2API_SetBoneAnim					= G2API_SetBoneAnim;
 	re.G2API_SetBoneAnimIndex				= G2API_SetBoneAnimIndex;
 	re.G2API_SetBoneIKState					= G2API_SetBoneIKState;
 	re.G2API_SetGhoul2ModelIndexes			= G2API_SetGhoul2ModelIndexes;
 	re.G2API_SetGhoul2ModelFlags			= G2API_SetGhoul2ModelFlags;
 	re.G2API_SetLodBias						= G2API_SetLodBias;
 	re.G2API_SetNewOrigin					= G2API_SetNewOrigin;
 	re.G2API_SetRagDoll						= G2API_SetRagDoll;
 	re.G2API_SetRootSurface					= G2API_SetRootSurface;
 	re.G2API_SetShader						= G2API_SetShader;
 	re.G2API_SetSkin						= G2API_SetSkin;
 	re.G2API_SetSurfaceOnOff				= G2API_SetSurfaceOnOff;
 	re.G2API_SetTime						= G2API_SetTime;
 	re.G2API_SkinlessModel					= G2API_SkinlessModel;
 	re.G2API_StopBoneAngles					= G2API_StopBoneAngles;
 	re.G2API_StopBoneAnglesIndex			= G2API_StopBoneAnglesIndex;
 	re.G2API_StopBoneAnim					= G2API_StopBoneAnim;
 	re.G2API_StopBoneAnimIndex				= G2API_StopBoneAnimIndex;
 
 	#ifdef _G2_GORE
 	re.G2API_GetNumGoreMarks				= G2API_GetNumGoreMarks;
 	re.G2API_AddSkinGore					= G2API_AddSkinGore;
 	re.G2API_ClearSkinGore					= G2API_ClearSkinGore;
 	#endif
 
 	re.ext.Font_StrLenPixels				= TRM_Font_StrLenPixelsNew;

	return &re;
}

void RE_SetColor( const float *rgba ) {
	TRM_SetColor(rgba);
}

void RE_StretchPic ( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader ) {
	TRM_DrawStretchPic(x, y, w, h, s1, t1, s2, t2, hShader);
}

qhandle_t RE_RegisterShaderNoMip( char const * sh ) {
	return TRM_RegisterShaderNoMip(sh);
}

void QDECL Com_Printf( const char *msg, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof(text), msg, argptr);
	va_end(argptr);

	ri->Printf(PRINT_ALL, "%s", text);
}

void QDECL Com_OPrintf( const char *msg, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof(text), msg, argptr);
	va_end(argptr);

	ri->OPrintf("%s", text);
}

void QDECL Com_Error( int level, const char *error, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start(argptr, error);
	Q_vsnprintf(text, sizeof(text), error, argptr);
	va_end(argptr);

	ri->Error(level, "%s", text);
}

// HUNK
void *Hunk_AllocateTempMemory( int size ) {
	return ri->Hunk_AllocateTempMemory( size );
}

void Hunk_FreeTempMemory( void *buf ) {
	ri->Hunk_FreeTempMemory( buf );
}

void *Hunk_Alloc( int size, ha_pref preference ) {
	return ri->Hunk_Alloc( size, preference );
}

int Hunk_MemoryRemaining( void ) {
	return ri->Hunk_MemoryRemaining();
}

// ZONE
void *Z_Malloc( int iSize, memtag_t eTag, qboolean bZeroit, int iAlign ) {
	return ri->Z_Malloc( iSize, eTag, bZeroit, iAlign );
}

void Z_Free( void *ptr ) {
	ri->Z_Free( ptr );
}

int Z_MemSize( memtag_t eTag ) {
	return ri->Z_MemSize( eTag );
}

void Z_MorphMallocTag( void *pvBuffer, memtag_t eDesiredTag ) {
	ri->Z_MorphMallocTag( pvBuffer, eDesiredTag );
}

qboolean ShaderHashTableExists() {
	return qtrue;
}

skin_t * TRM_GetSkinByHandle(qhandle_t) {
	return nullptr;
}

model_t * TRM_GetModelByHandle( qhandle_t hModel ) {
	return nullptr;
}

float ProjectRadius( float r, vec3_t location )
{
	float pr;
	float dist;
	float c;
	vec3_t	p;
	float width;
	float depth;

	c = DotProduct( tr.viewParms.ori.axis[0], tr.viewParms.ori.origin );
	dist = DotProduct( tr.viewParms.ori.axis[0], location ) - c;

	if ( dist <= 0 )
		return 0;

	p[0] = 0;
	p[1] = Q_fabs( r );
	p[2] = -dist;

	width = p[0] * tr.viewParms.projectionMatrix[1] +
		           p[1] * tr.viewParms.projectionMatrix[5] +
				   p[2] * tr.viewParms.projectionMatrix[9] +
				   tr.viewParms.projectionMatrix[13];

	depth = p[0] * tr.viewParms.projectionMatrix[3] +
		           p[1] * tr.viewParms.projectionMatrix[7] +
				   p[2] * tr.viewParms.projectionMatrix[11] +
				   tr.viewParms.projectionMatrix[15];

	pr = width / depth;

	if ( pr > 1.0f )
		pr = 1.0f;

	return pr;
}
