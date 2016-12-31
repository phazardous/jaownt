
textures/crystal/crystal_rainbow
{
	qer_editorimage	textures/crystal/env
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	{
        map $lightmap
        blendFunc GL_ONE GL_ONE
    }
	{
        map $whiteimage
        rgbGen rainbow 0 1.0
        blendFunc GL_DST_COLOR GL_ZERO
    }
    {
        map textures/crystal/env
        blendFunc GL_DST_COLOR GL_ONE
        tcGen environment
    }

}

textures/crystal/crystal_rainbow_unlit
{
	qer_editorimage	textures/crystal/env
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen rainbow 1 1.0
        blendFunc GL_ONE GL_ONE
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

textures/crystal/crystal_rainbow_glowing
{
	qer_editorimage	textures/crystal/env
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_nolightmap
	{
        map $whiteimage
		rgbGen rainbow 1 1.0
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// RED
//----------------------------------------------------------------

textures/crystal/crystal_red
{
	qer_editorimage	textures/editor/crys_red_dull
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	{
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        rgbGen const ( 1.000000 0.500000 0.500000 )
        tcGen environment
    }
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 0.000000 )
        blendFunc GL_ONE GL_ONE
    }
	{
        map $lightmap
        blendFunc GL_DST_COLOR GL_SRC_COLOR
    }
}

textures/crystal/crystal_red_unlit
{
	qer_editorimage	textures/editor/crys_red_unlit
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
        blendFunc GL_ONE GL_ONE
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

textures/crystal/crystal_red_glowing
{
	qer_editorimage textures/editor/crys_red_glowing
	q3map_lightImage textures/editor/c_red
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 1.000000 0.000000 0.000000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// ORANGE
//----------------------------------------------------------------

textures/crystal/crystal_orange
{
	qer_editorimage	textures/editor/crys_orange_unlit
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	{
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        rgbGen const ( 1.000000 1.000000 0.500000 )
        tcGen environment
    }
    {
        map $whiteimage
        rgbGen const ( 0.500000 0.500000 0.000000 )
        blendFunc GL_ONE GL_ONE
    }
	{
        map $lightmap
        blendFunc GL_DST_COLOR GL_SRC_COLOR
    }
}

textures/crystal/crystal_orange_unlit
{
	qer_editorimage	textures/crystal/env
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.750000 0.250000 0.000000 )
        blendFunc GL_ONE GL_ONE
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

textures/crystal/crystal_orange_glowing
{
	qer_editorimage textures/editor/crys_orange_glowing
	q3map_lightImage textures/editor/c_orange
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.750000 0.250000 0.000000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// YELLOW
//----------------------------------------------------------------

textures/crystal/crystal_yellow_glowing
{
	qer_editorimage textures/editor/crys_yellow_glowing
	q3map_lightImage textures/editor/c_yellow
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.500000 0.500000 0.000000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// LIME
//----------------------------------------------------------------

textures/crystal/crystal_lime_glowing
{
	qer_editorimage textures/editor/crys_lime_glowing
	q3map_lightImage textures/editor/c_lime
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.250000 0.750000 0.000000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// GREEN
//----------------------------------------------------------------

textures/crystal/crystal_green_glowing
{
	qer_editorimage textures/editor/crys_green_glowing
	q3map_lightImage textures/editor/c_green
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.000000 1.000000 0.000000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// SPRING
//----------------------------------------------------------------

textures/crystal/crystal_spring_glowing
{
	qer_editorimage textures/editor/crys_spring_glowing
	q3map_lightImage textures/editor/c_spring
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.000000 0.750000 0.250000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// CYAN
//----------------------------------------------------------------

textures/crystal/crystal_cyan_glowing
{
	qer_editorimage textures/editor/crys_cyan_glowing
	q3map_lightImage textures/editor/c_cyan
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.000000 0.500000 0.500000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// DODGER
//----------------------------------------------------------------

textures/crystal/crystal_dodger_glowing
{
	qer_editorimage textures/editor/crys_dodger_glowing
	q3map_lightImage textures/editor/c_dodger
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.000000 0.2500000 0.750000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// BLUE
//----------------------------------------------------------------

textures/crystal/crystal_blue
{
	qer_editorimage	textures/editor/crys_blue_dull
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	{
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        rgbGen const ( 0.500000 0.500000 1.000000 )
        tcGen environment
    }
    {
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 0.500000 )
        blendFunc GL_ONE GL_ONE
    }
	{
        map $lightmap
        blendFunc GL_DST_COLOR GL_SRC_COLOR
    }
}

textures/crystal/crystal_blue_unlit
{
	qer_editorimage	textures/editor/crys_blue_unlit
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
        blendFunc GL_ONE GL_ONE
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

textures/crystal/crystal_blue_glowing
{
	qer_editorimage textures/editor/crys_blue_glowing
	q3map_lightImage textures/editor/c_blue
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.000000 0.000000 1.000000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// VIOLET
//----------------------------------------------------------------

textures/crystal/crystal_violet_glowing
{
	qer_editorimage textures/editor/crys_violet_glowing
	q3map_lightImage textures/editor/c_violet
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.250000 0.000000 0.750000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// MAGENTA
//----------------------------------------------------------------

textures/crystal/crystal_magenta_glowing
{
	qer_editorimage textures/editor/crys_magenta_glowing
	q3map_lightImage textures/editor/c_magenta
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.500000 0.000000 0.500000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// HOT PINK
//----------------------------------------------------------------

textures/crystal/crystal_hotpink_glowing
{
	qer_editorimage textures/editor/crys_hotpink_glowing
	q3map_lightImage textures/editor/c_hotpink
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	q3map_surfacelight		6000
	q3map_lightsubdivide	512
	q3map_nolightmap
	{
        map $whiteimage
        rgbGen const ( 0.750000 0.000000 0.250000 )
        blendFunc GL_ONE GL_ONE
        glow
    }
    {
        map textures/crystal/env
        blendFunc GL_ONE GL_ONE
        tcGen environment
    }
}

//----------------------------------------------------------------
// CRYSTAL GLASS
//----------------------------------------------------------------

textures/crystal/crystal_glass
{
	qer_editorimage	textures/crystal/env
	surfaceparm	nonopaque
	surfaceparm	forcefield
	surfaceparm	trans
	q3map_material Glass
	qer_trans	0.5
	
	{
        map $lightmap
        alphaGen const 0.15
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    }
	{
        map textures/crystal/env
        blendFunc GL_DST_COLOR GL_ONE
        rgbGen const ( 0.300000 0.300000 0.300000 )
        tcGen environment
    }
}

textures/crystal/crystal_glass_black
{
	qer_editorimage	textures/crystal/env
	surfaceparm	forcefield
	q3map_material Glass

	{
        map textures/crystal/env
        blendFunc GL_ONE GL_ZERO
        tcGen environment
    }
}
