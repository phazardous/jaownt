#include "rd-common/tr_public.h"
#include "tr_local.h"
#include "ghoul2/g2_local.h"

#include "rd-common/tr_font.h"

#include <cstdarg>

refimport_t * ri;
window_t window;
glconfig_t glConfig;

cvar_t * se_language;
cvar_t * r_aspectCorrectFonts;

void TRM_Shutdown( qboolean destroyWindow, qboolean restarting  ) {
	ri->WIN_Shutdown();
	R_ShutdownFonts();
}

void TRM_BeginRegistration( glconfig_t *config  ) {
	
	Com_Printf("================================================================\n");
	Com_Printf("Initializing Renderer\n");
	Com_Printf("================================\n");
	
	windowDesc_t windesc { GRAPHICS_API_OPENGL };
	window = ri->WIN_Init(&windesc, config);
	
	gladLoadGL();
	
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
	
	glViewport(0, 0, config->vidWidth, config->vidHeight);
	glScissor(0, 0, config->vidWidth, config->vidHeight);
	
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	config->isFullscreen = qfalse;
	config->stereoEnabled = qfalse;
	
	glConfig = *config;
	
	se_language = ri->Cvar_Get("se_language", "english", CVAR_ARCHIVE | CVAR_NORESTART, "");
	r_aspectCorrectFonts = ri->Cvar_Get( "r_aspectCorrectFonts", "0", CVAR_ARCHIVE, "");
	
	R_InitFonts();
	
	Com_Printf("================================================================\n");
}

qhandle_t TRM_RegisterModel( const char *name  ) {
	return 0; //TODO
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
	return 0; //TODO
}

qhandle_t TRM_RegisterShaderNoMip( const char *name  ) {
	return 0; //TODO
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
	//TODO
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
	//TODO
}

void TRM_SetColor( const float *rgba  ) {
	//TODO
}

void TRM_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader  ) {
	//TODO
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
	//TODO
}

void TRM_EndFrame( int *frontEndMsec, int *backEndMsec  ) {
	ri->WIN_Present(&window);
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

int G2API_AddBolt( CGhoul2Info_v &ghoul2, const int modelIndex, const char *boneName  ) {
	return 0; //TODO
}

int G2API_AddBoltSurfNum( CGhoul2Info *ghlInfo, const int surfIndex  ) {
	return 0; //TODO
}

int G2API_AddSurface( CGhoul2Info *ghlInfo, int surfaceNumber, int polyNumber, float BarycentricI, float BarycentricJ, int lod  ) {
	return 0; //TODO
}

void G2API_AnimateG2ModelsRag( CGhoul2Info_v &ghoul2, int AcurrentTime, CRagDollUpdateParams *params  ) {
	//TODO
}

qboolean G2API_AttachEnt( int *boltInfo, CGhoul2Info_v& ghoul2, int modelIndex, int toBoltIndex, int entNum, int toModelNum  ) {
	return qfalse; //TODO
}

qboolean G2API_AttachG2Model( CGhoul2Info_v &ghoul2From, int modelFrom, CGhoul2Info_v &ghoul2To, int toBoltIndex, int toModel  ) {
	return qfalse; //TODO
}

void G2API_AttachInstanceToEntNum( CGhoul2Info_v &ghoul2, int entityNum, qboolean server  ) {
	//TODO
}

void G2API_AbsurdSmoothing( CGhoul2Info_v &ghoul2, qboolean status  ) {
	//TODO
}

void G2API_BoltMatrixReconstruction( qboolean reconstruct  ) {
	//TODO
}

void G2API_BoltMatrixSPMethod( qboolean spMethod  ) {
	//TODO
}

void G2API_CleanEntAttachments( void  ) {
	//TODO
}

void G2API_CleanGhoul2Models( CGhoul2Info_v **ghoul2Ptr  ) {
	//TODO
}

void G2API_ClearAttachedInstance( int entityNum  ) {
	//TODO
}

void G2API_CollisionDetect( CollisionRecord_t *collRecMap, CGhoul2Info_v &ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, IHeapAllocator *G2VertSpace, int traceFlags, int useLod, float fRadius  ) {
	//TODO
}

void G2API_CollisionDetectCache( CollisionRecord_t *collRecMap, CGhoul2Info_v &ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, IHeapAllocator *G2VertSpace, int traceFlags, int useLod, float fRadius  ) {
	//TODO
}

int G2API_CopyGhoul2Instance( CGhoul2Info_v &g2From, CGhoul2Info_v &g2To, int modelIndex  ) {
	return 0; //TODO
}

void G2API_CopySpecificG2Model( CGhoul2Info_v &ghoul2From, int modelFrom, CGhoul2Info_v &ghoul2To, int modelTo  ) {
	//TODO
}

qboolean G2API_DetachG2Model( CGhoul2Info *ghlInfo  ) {
	return qfalse; //TODO
}

qboolean G2API_DoesBoneExist( CGhoul2Info_v& ghoul2, int modelIndex, const char *boneName  ) {
	return qfalse; //TODO
}

void G2API_DuplicateGhoul2Instance( CGhoul2Info_v &g2From, CGhoul2Info_v **g2To  ) {
	//TODO
}

void G2API_FreeSaveBuffer( char *buffer  ) {
	//TODO
}

qboolean G2API_GetAnimFileName( CGhoul2Info *ghlInfo, char **filename  ) {
	return qfalse; //TODO
}

char * G2API_GetAnimFileNameIndex( qhandle_t modelIndex  ) {
	return nullptr; //TODO
}

qboolean G2API_GetAnimRange( CGhoul2Info *ghlInfo, const char *boneName, int *startFrame, int *endFrame  ) {
	return qfalse; //TODO
}

qboolean G2API_GetBoltMatrix( CGhoul2Info_v &ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale  ) {
	return qfalse; //TODO
}

qboolean G2API_GetBoneAnim( CGhoul2Info_v& ghoul2, int modelIndex, const char *boneName, const int currentTime, float *currentFrame, int *startFrame, int *endFrame, int *flags, float *animSpeed, qhandle_t *modelList  ) {
	return qfalse; //TODO
}

int G2API_GetBoneIndex( CGhoul2Info *ghlInfo, const char *boneName  ) {
	return 0; //TODO
}

int G2API_GetGhoul2ModelFlags( CGhoul2Info *ghlInfo  ) {
	return 0; //TODO
}

char * G2API_GetGLAName( CGhoul2Info_v &ghoul2, int modelIndex  ) {
	return nullptr; //TODO
}

const char * G2API_GetModelName( CGhoul2Info_v& ghoul2, int modelIndex  ) {
	return nullptr; //TODO
}

int G2API_GetParentSurface( CGhoul2Info *ghlInfo, const int index  ) {
	return 0; //TODO
}

qboolean G2API_GetRagBonePos( CGhoul2Info_v &ghoul2, const char *boneName, vec3_t pos, vec3_t entAngles, vec3_t entPos, vec3_t entScale  ) {
	return qfalse; //TODO
}

int G2API_GetSurfaceIndex( CGhoul2Info *ghlInfo, const char *surfaceName  ) {
	return 0; //TODO
}

char * G2API_GetSurfaceName( CGhoul2Info_v& ghlInfo, int modelIndex, int surfNumber  ) {
	return nullptr; //TODO
}

int G2API_GetSurfaceOnOff( CGhoul2Info *ghlInfo, const char *surfaceName  ) {
	return 0; //TODO
}

int G2API_GetSurfaceRenderStatus( CGhoul2Info_v& ghoul2, int modelIndex, const char *surfaceName  ) {
	return 0; //TODO
}

int G2API_GetTime( int argTime  ) {
	return 0; //TODO
}

int G2API_Ghoul2Size( CGhoul2Info_v &ghoul2  ) {
	return 0; //TODO
}

void G2API_GiveMeVectorFromMatrix( mdxaBone_t *boltMatrix, Eorientations flags, vec3_t vec  ) {
	//TODO
}

qboolean G2API_HasGhoul2ModelOnIndex( CGhoul2Info_v **ghlRemove, const int modelIndex  ) {
	return qfalse; //TODO
}

qboolean G2API_HaveWeGhoul2Models( CGhoul2Info_v &ghoul2  ) {
	return qfalse; //TODO
}

qboolean G2API_IKMove( CGhoul2Info_v &ghoul2, int time, sharedIKMoveParams_t *params  ) {
	return qfalse; //TODO
}

int G2API_InitGhoul2Model( CGhoul2Info_v **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin, qhandle_t customShader, int modelFlags, int lodBias  ) {
	return 0; //TODO
}

qboolean G2API_IsGhoul2InfovValid( CGhoul2Info_v& ghoul2  ) {
	return qfalse; //TODO
}

qboolean G2API_IsPaused( CGhoul2Info *ghlInfo, const char *boneName  ) {
	return qfalse; //TODO
}

void G2API_ListBones( CGhoul2Info *ghlInfo, int frame  ) {
	//TODO
}

void G2API_ListSurfaces( CGhoul2Info *ghlInfo  ) {
	//TODO
}

void G2API_LoadGhoul2Models( CGhoul2Info_v &ghoul2, char *buffer  ) {
	//TODO
}

void G2API_LoadSaveCodeDestructGhoul2Info( CGhoul2Info_v &ghoul2  ) {
	//TODO
}

qboolean G2API_OverrideServerWithClientData( CGhoul2Info_v& serverInstance, int modelIndex  ) {
	return qfalse; //TODO
}

qboolean G2API_PauseBoneAnim( CGhoul2Info *ghlInfo, const char *boneName, const int currentTime  ) {
	return qfalse; //TODO
}

qhandle_t G2API_PrecacheGhoul2Model( const char *fileName  ) {
	return 0; //TODO
}

qboolean G2API_RagEffectorGoal( CGhoul2Info_v &ghoul2, const char *boneName, vec3_t pos  ) {
	return qfalse; //TODO
}

qboolean G2API_RagEffectorKick( CGhoul2Info_v &ghoul2, const char *boneName, vec3_t velocity  ) {
	return qfalse; //TODO
}

qboolean G2API_RagForceSolve( CGhoul2Info_v &ghoul2, qboolean force  ) {
	return qfalse; //TODO
}

qboolean G2API_RagPCJConstraint( CGhoul2Info_v &ghoul2, const char *boneName, vec3_t min, vec3_t max  ) {
	return qfalse; //TODO
}

qboolean G2API_RagPCJGradientSpeed( CGhoul2Info_v &ghoul2, const char *boneName, const float speed  ) {
	return qfalse; //TODO
}

qboolean G2API_RemoveBolt( CGhoul2Info *ghlInfo, const int index  ) {
	return qfalse; //TODO
}

qboolean G2API_RemoveBone( CGhoul2Info_v& ghoul2, int modelIndex, const char *boneName  ) {
	return qfalse; //TODO
}

qboolean G2API_RemoveGhoul2Model( CGhoul2Info_v **ghlRemove, const int modelIndex  ) {
	return qfalse; //TODO
}

qboolean G2API_RemoveGhoul2Models( CGhoul2Info_v **ghlRemove  ) {
	return qfalse; //TODO
}

qboolean G2API_RemoveSurface( CGhoul2Info *ghlInfo, const int index  ) {
	return qfalse; //TODO
}

void G2API_ResetRagDoll( CGhoul2Info_v &ghoul2  ) {
	//TODO
}

qboolean G2API_SaveGhoul2Models( CGhoul2Info_v &ghoul2, char **buffer, int *size  ) {
	return qfalse; //TODO
}

void G2API_SetBoltInfo( CGhoul2Info_v &ghoul2, int modelIndex, int boltInfo  ) {
	//TODO
}

qboolean G2API_SetBoneAngles( CGhoul2Info_v &ghoul2, const int modelIndex, const char *boneName, const vec3_t angles, const int flags, const Eorientations up, const Eorientations left, const Eorientations forward, qhandle_t *modelList, int blendTime, int currentTime   ) {
	return qfalse; //TODO
}

qboolean G2API_SetBoneAnglesIndex( CGhoul2Info *ghlInfo, const int index, const vec3_t angles, const int flags, const Eorientations yaw, const Eorientations pitch, const Eorientations roll, qhandle_t *modelList, int blendTime, int currentTime  ) {
	return qfalse; //TODO
}

qboolean G2API_SetBoneAnglesMatrix( CGhoul2Info *ghlInfo, const char *boneName, const mdxaBone_t &matrix, const int flags, qhandle_t *modelList, int blendTime, int currentTime  ) {
	return qfalse; //TODO
}

qboolean G2API_SetBoneAnglesMatrixIndex( CGhoul2Info *ghlInfo, const int index, const mdxaBone_t &matrix, const int flags, qhandle_t *modelList, int blendTime, int currentTime  ) {
	return qfalse; //TODO
}

qboolean G2API_SetBoneAnim( CGhoul2Info_v &ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame /*= -1*/, const int blendTime /*= -1*/  ) {
	return qfalse; //TODO
}

qboolean G2API_SetBoneAnimIndex( CGhoul2Info *ghlInfo, const int index, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime  ) {
	return qfalse; //TODO
}

qboolean G2API_SetBoneIKState( CGhoul2Info_v &ghoul2, int time, const char *boneName, int ikState, sharedSetBoneIKStateParams_t *params  ) {
	return qfalse; //TODO
}

qboolean G2API_SetGhoul2ModelFlags( CGhoul2Info *ghlInfo, const int flags  ) {
	return qfalse; //TODO
}

void G2API_SetGhoul2ModelIndexes( CGhoul2Info_v &ghoul2, qhandle_t *modelList, qhandle_t *skinList  ) {
	//TODO
}

qboolean G2API_SetLodBias( CGhoul2Info *ghlInfo, int lodBias  ) {
	return qfalse; //TODO
}

qboolean G2API_SetNewOrigin( CGhoul2Info_v &ghoul2, const int boltIndex  ) {
	return qfalse; //TODO
}

void G2API_SetRagDoll( CGhoul2Info_v &ghoul2, CRagDollParams *parms  ) {
	//TODO
}

qboolean G2API_SetRootSurface( CGhoul2Info_v &ghoul2, const int modelIndex, const char *surfaceName  ) {
	return qfalse; //TODO
}

qboolean G2API_SetShader( CGhoul2Info *ghlInfo, qhandle_t customShader  ) {
	return qfalse; //TODO
}

qboolean G2API_SetSkin( CGhoul2Info_v& ghoul2, int modelIndex, qhandle_t customSkin, qhandle_t renderSkin  ) {
	return qfalse; //TODO
}

qboolean G2API_SetSurfaceOnOff( CGhoul2Info_v &ghoul2, const char *surfaceName, const int flags  ) {
	return qfalse; //TODO
}

void G2API_SetTime( int currentTime, int clock  ) {
	//TODO
}

qboolean G2API_SkinlessModel( CGhoul2Info_v& ghoul2, int modelIndex  ) {
	return qfalse; //TODO
}

qboolean G2API_StopBoneAngles( CGhoul2Info *ghlInfo, const char *boneName  ) {
	return qfalse; //TODO
}

qboolean G2API_StopBoneAnglesIndex( CGhoul2Info *ghlInfo, const int index  ) {
	return qfalse; //TODO
}

qboolean G2API_StopBoneAnim( CGhoul2Info *ghlInfo, const char *boneName  ) {
	return qfalse; //TODO
}

qboolean G2API_StopBoneAnimIndex( CGhoul2Info *ghlInfo, const int index  ) {
	return qfalse; //TODO
}


#ifdef _G2_GORE
int G2API_GetNumGoreMarks( CGhoul2Info_v& ghoul2, int modelIndex  ) {
	return 0; //TODO
}

void G2API_AddSkinGore( CGhoul2Info_v &ghoul2, SSkinGoreData &gore  ) {
	//TODO
}

void G2API_ClearSkinGore( CGhoul2Info_v &ghoul2  ) {
	//TODO
}

#endif // _G2_GORE

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
	// TODO -- change "color" ???
}

void RE_StretchPic ( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader ) {
	// TODO -- stretch.... something??? it's font related
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

