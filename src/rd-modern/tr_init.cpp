#include "rd-common/tr_public.h"
#include "tr_local.h"
#include "ghoul2/g2_local.h"

static refimport_t * ri;

extern "C" {
Q_EXPORT refexport_t* QDECL GetRefAPI( int apiVersion, refimport_t *rimp ) {
	static refexport_t re;

	assert( rimp );
	ri = rimp;

	memset( &re, 0, sizeof( re ) );

	if ( apiVersion != REF_API_VERSION ) {
		ri->Printf( PRINT_ALL,  "Mismatched REF_API_VERSION: expected %i, got %i\n", REF_API_VERSION, apiVersion );
		return NULL;
	}

	// the RE_ functions are Renderer Entry points

	/*
	re.Shutdown								= RE_Shutdown;
	re.BeginRegistration					= RE_BeginRegistration;
	re.RegisterModel						= RE_RegisterModel;
	re.RegisterServerModel					= RE_RegisterServerModel;
	re.RegisterSkin							= RE_RegisterSkin;
	re.RegisterServerSkin					= RE_RegisterServerSkin;
	re.RegisterShader						= RE_RegisterShader;
	re.RegisterShaderNoMip					= RE_RegisterShaderNoMip;
	re.ShaderNameFromIndex					= RE_ShaderNameFromIndex;
	re.LoadWorld							= RE_LoadWorldMap;
	re.SetWorldVisData						= RE_SetWorldVisData;
	re.EndRegistration						= RE_EndRegistration;
	re.BeginFrame							= RE_BeginFrame;
	re.EndFrame								= RE_EndFrame;
	re.MarkFragments						= R_MarkFragments;
	re.LerpTag								= R_LerpTag;
	re.ModelBounds							= R_ModelBounds;
	re.DrawRotatePic						= RE_RotatePic;
	re.DrawRotatePic2						= RE_RotatePic2;
	re.ClearScene							= RE_ClearScene;
	re.ClearDecals							= RE_ClearDecals;
	re.AddRefEntityToScene					= RE_AddRefEntityToScene;
	re.AddMiniRefEntityToScene				= RE_AddMiniRefEntityToScene;
	re.AddPolyToScene						= RE_AddPolyToScene;
	re.AddDecalToScene						= RE_AddDecalToScene;
	re.LightForPoint						= R_LightForPoint;
	re.AddLightToScene						= RE_AddLightToScene;
	re.AddAdditiveLightToScene				= RE_AddAdditiveLightToScene;

	re.RenderScene							= RE_RenderScene;
	re.SetColor								= RE_SetColor;
	re.DrawStretchPic						= RE_StretchPic;
	re.DrawStretchRaw						= RE_StretchRaw;
	re.UploadCinematic						= RE_UploadCinematic;

	re.RegisterFont							= RE_RegisterFont;
	re.Font_StrLenPixels					= RE_Font_StrLenPixels;
	re.Font_StrLenChars						= RE_Font_StrLenChars;
	re.Font_HeightPixels					= RE_Font_HeightPixels;
	re.Font_DrawString						= RE_Font_DrawString;
	re.Language_IsAsian						= Language_IsAsian;
	re.Language_UsesSpaces					= Language_UsesSpaces;
	re.AnyLanguage_ReadCharFromString		= AnyLanguage_ReadCharFromString;

	re.RemapShader							= R_RemapShader;
	re.GetEntityToken						= R_GetEntityToken;
	re.inPVS								= R_inPVS;
	re.GetLightStyle						= RE_GetLightStyle;
	re.SetLightStyle						= RE_SetLightStyle;
	re.GetBModelVerts						= RE_GetBModelVerts;

	// missing from 1.01
	re.SetRangedFog							= SetRangedFog;
	re.SetRefractionProperties				= SetRefractionProperties;
	re.GetDistanceCull						= GetDistanceCull;
	re.GetRealRes							= GetRealRes;
	re.AutomapElevationAdjustment			= R_AutomapElevationAdjustment; //tr_world.cpp
	re.InitializeWireframeAutomap			= R_InitializeWireframeAutomap; //tr_world.cpp
	re.AddWeatherZone						= RE_AddWeatherZone;
	re.WorldEffectCommand					= RE_WorldEffectCommand;
	re.RegisterMedia_LevelLoadBegin			= RE_RegisterMedia_LevelLoadBegin;
	re.RegisterMedia_LevelLoadEnd			= RE_RegisterMedia_LevelLoadEnd;
	re.RegisterMedia_GetLevel				= RE_RegisterMedia_GetLevel;
	re.RegisterImages_LevelLoadEnd			= RE_RegisterImages_LevelLoadEnd;
	re.RegisterModels_LevelLoadEnd			= RE_RegisterModels_LevelLoadEnd;

	// AVI recording
	re.TakeVideoFrame						= RE_TakeVideoFrame;

	// G2 stuff
	re.InitSkins							= R_InitSkins;
	re.InitShaders							= R_InitShaders;
	re.SVModelInit							= R_SVModelInit;
	re.HunkClearCrap						= RE_HunkClearCrap;

	// G2API
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
	#endif // _SOF2

	// this is set in R_Init
	//re.G2VertSpaceServer	= G2VertSpaceServer;

	re.ext.Font_StrLenPixels				= RE_Font_StrLenPixelsNew;
	*/

	return &re;
}

} //extern "C"
