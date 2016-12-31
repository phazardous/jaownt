//----------------------------------------------------------------
// BLACK
//----------------------------------------------------------------

textures/colors/c_black_pitch
{
	qer_editorimage	textures/editor/c_black
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_black_pitch-nc
{
	qer_editorimage	textures/editor/c_black
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_black
{
	qer_editorimage	textures/editor/c_black
    {
        map $whiteimage
        rgbGen const ( 0.100000 0.100000 0.100000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_black-nc
{
	qer_editorimage	textures/editor/c_black
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.100000 0.100000 0.100000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_black_model
{
	qer_editorimage	textures/editor/c_black
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.100000 0.100000 0.100000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

//----------------------------------------------------------------
// GREY
//----------------------------------------------------------------

textures/colors/c_grey
{
	qer_editorimage	textures/editor/c_grey
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.500000 0.500000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_grey-nc
{
	qer_editorimage	textures/editor/c_grey
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.500000 0.500000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

//----------------------------------------------------------------
// WHITE
//----------------------------------------------------------------

textures/colors/c_white
{
	qer_editorimage	textures/editor/c_white
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_white-nc
{
	qer_editorimage	textures/editor/c_white
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_white_model
{
	qer_editorimage	textures/editor/c_white
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 1.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_white
{
	qer_editorimage	textures/editor/c_white
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        glow
    }
}

textures/colors/light_white-nc
{
	qer_editorimage	textures/editor/c_white
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        glow
    }
}

textures/colors/light_white_warm
{
	qer_editorimage	textures/editor/c_white_warm
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.700000 0.500000 )
        glow
    }
}

textures/colors/light_white_warm-nc
{
	qer_editorimage	textures/editor/c_white_warm
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.700000 0.500000 )
        glow
    }
}

textures/colors/light_white_cool
{
	qer_editorimage	textures/editor/c_white
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.700000 1.000000 )
        glow
    }
}

textures/colors/light_white_cool-nc
{
	qer_editorimage	textures/editor/c_white
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.700000 1.000000 )
        glow
    }
}

//----------------------------------------------------------------
// RED
//----------------------------------------------------------------

textures/colors/c_red
{
	qer_editorimage	textures/editor/c_red
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_red-nc
{
	qer_editorimage	textures/editor/c_red
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_red_model
{
	qer_editorimage	textures/editor/c_red
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_red
{
	qer_editorimage	textures/editor/c_red
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
        glow
    }
}

textures/colors/light_red-nc
{
	qer_editorimage	textures/editor/c_red
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
        glow
    }
}

//----------------------------------------------------------------
// ORANGE
//----------------------------------------------------------------

textures/colors/c_orange
{
	qer_editorimage	textures/editor/c_orange
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.500000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_orange-nc
{
	qer_editorimage	textures/editor/c_orange
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.500000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_orange_model
{
	qer_editorimage	textures/editor/c_orange
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.500000 0.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_orange
{
	qer_editorimage	textures/editor/c_orange
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.500000 0.000000 )
        glow
    }
}

textures/colors/light_orange-nc
{
	qer_editorimage	textures/editor/c_orange
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.500000 0.000000 )
        glow
    }
}

//----------------------------------------------------------------
// YELLOW
//----------------------------------------------------------------

textures/colors/c_yellow
{
	qer_editorimage	textures/editor/c_yellow
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_yellow-nc
{
	qer_editorimage	textures/editor/c_yellow
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_yellow_model
{
	qer_editorimage	textures/editor/c_yellow
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 0.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_yellow
{
	qer_editorimage	textures/editor/c_yellow
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 0.000000 )
        glow
    }
}

textures/colors/light_yellow-nc
{
	qer_editorimage	textures/editor/c_yellow
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 1.000000 0.000000 )
        glow
    }
}

//----------------------------------------------------------------
// LIME
//----------------------------------------------------------------

textures/colors/c_lime
{
	qer_editorimage	textures/editor/c_lime
    {
        map $whiteimage
        rgbGen const ( 0.500000 1.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_lime-nc
{
	qer_editorimage	textures/editor/c_lime
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.500000 1.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_lime_model
{
	qer_editorimage	textures/editor/c_lime
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.500000 1.000000 0.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_lime
{
	qer_editorimage	textures/editor/c_lime
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.500000 1.000000 0.000000 )
        glow
    }
}

textures/colors/light_lime-nc
{
	qer_editorimage	textures/editor/c_lime
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.500000 1.000000 0.000000 )
        glow
    }
}

//----------------------------------------------------------------
// GREEN
//----------------------------------------------------------------

textures/colors/c_green
{
	qer_editorimage	textures/editor/c_green
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_green-nc
{
	qer_editorimage	textures/editor/c_green
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_green_model
{
	qer_editorimage	textures/editor/c_green
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_green
{
	qer_editorimage	textures/editor/c_green
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.000000 )
        glow
    }
}

textures/colors/light_green-nc
{
	qer_editorimage	textures/editor/c_green
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.000000 )
        glow
    }
}

//----------------------------------------------------------------
// SPRING GREEN
//----------------------------------------------------------------

textures/colors/c_spring
{
	qer_editorimage	textures/editor/c_spring
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.500000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_spring-nc
{
	qer_editorimage	textures/editor/c_spring
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.500000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_spring_model
{
	qer_editorimage	textures/editor/c_spring
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.500000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_spring
{
	qer_editorimage	textures/editor/c_spring
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.500000 )
        glow
    }
}

textures/colors/light_spring-nc
{
	qer_editorimage	textures/editor/c_spring
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.500000 )
        glow
    }
}

//----------------------------------------------------------------
// CYAN
//----------------------------------------------------------------

textures/colors/c_cyan
{
	qer_editorimage	textures/editor/c_cyan
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_cyan-nc
{
	qer_editorimage	textures/editor/c_cyan
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_cyan_model
{
	qer_editorimage	textures/editor/c_cyan
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 1.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_cyan
{
	qer_editorimage	textures/editor/c_cyan
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 1.000000 )
        glow
    }
}

textures/colors/light_cyan-nc
{
	qer_editorimage	textures/editor/c_cyan
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 1.000000 )
        glow
    }
}

//----------------------------------------------------------------
// DODGER BLUE
//----------------------------------------------------------------

textures/colors/c_dodger
{
	qer_editorimage	textures/editor/c_dodger
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.500000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_dodger-nc
{
	qer_editorimage	textures/editor/c_dodger
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.500000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_dodger_model
{
	qer_editorimage	textures/editor/c_dodger
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.500000 1.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_dodger
{
	qer_editorimage	textures/editor/c_dodger
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.500000 1.000000 )
        glow
    }
}

textures/colors/light_dodger-nc
{
	qer_editorimage	textures/editor/c_dodger
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.500000 1.000000 )
        glow
    }
}

//----------------------------------------------------------------
// BLUE
//----------------------------------------------------------------

textures/colors/c_blue
{
	qer_editorimage	textures/editor/c_blue
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_blue-nc
{
	qer_editorimage	textures/editor/c_blue
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_blue_model
{
	qer_editorimage	textures/editor/c_blue
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_blue
{
	qer_editorimage	textures/editor/c_blue
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
        glow
    }
}

textures/colors/light_blue-nc
{
	qer_editorimage	textures/editor/c_blue
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
        glow
    }
}

//----------------------------------------------------------------
// VIOLET
//----------------------------------------------------------------

textures/colors/c_violet
{
	qer_editorimage	textures/editor/c_violet
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_violet-nc
{
	qer_editorimage	textures/editor/c_violet
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_violet_model
{
	qer_editorimage	textures/editor/c_violet
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 1.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_violet
{
	qer_editorimage	textures/editor/c_violet
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 1.000000 )
        glow
    }
}

textures/colors/light_violet-nc
{
	qer_editorimage	textures/editor/c_violet
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 1.000000 )
        glow
    }
}

//----------------------------------------------------------------
// MAGENTA
//----------------------------------------------------------------

textures/colors/c_magenta
{
	qer_editorimage	textures/editor/c_magenta
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_magenta-nc
{
	qer_editorimage	textures/editor/c_magenta
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 1.000000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_magenta_model
{
	qer_editorimage	textures/editor/c_magenta
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 1.000000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_magenta
{
	qer_editorimage	textures/editor/c_magenta
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 1.000000 )
        glow
    }
}

textures/colors/light_magenta-nc
{
	qer_editorimage	textures/editor/c_magenta
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 1.000000 )
        glow
    }
}

//----------------------------------------------------------------
// HOT PINK
//----------------------------------------------------------------

textures/colors/c_hotpink
{
	qer_editorimage	textures/editor/c_hotpink
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.500000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_hotpink-nc
{
	qer_editorimage	textures/editor/c_hotpink
	surfaceparm	nonsolid
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.500000 )
    }
    {
        map $lightmap
        blendFunc GL_ZERO GL_SRC_COLOR
    }
}

textures/colors/c_hotpink_model
{
	qer_editorimage	textures/editor/c_hotpink
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.500000 )
    }
    {
        map $whiteimage
        rgbGen lightingDiffuse
        blendFunc GL_DST_COLOR GL_ZERO
    }
}

textures/colors/light_hotpink
{
	qer_editorimage	textures/editor/c_hotpink
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.500000 )
        glow
    }
}

textures/colors/light_hotpink-nc
{
	qer_editorimage	textures/editor/c_hotpink
	q3map_surfacelight	15000
	q3map_lightsubdivide	512
	surfaceparm	nonsolid
	q3map_nolightmap
    {
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.500000 )
        glow
    }
}
