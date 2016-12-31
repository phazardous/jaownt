gfx/effects/sabers/black_glow
{
	cull	twosided
    {
        map gfx/effects/sabers/black_glow2
        blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
        glow
        rgbGen vertex
    }
}

gfx/effects/sabers/black_line
{
	cull	twosided
    {
        map gfx/effects/sabers/black_line
        blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
        rgbGen vertex
    }
}
gfx/effects/sabers/saberBlur_black
{
	cull	twosided
    {
        clampmap gfx/effects/sabers/blurcore
        blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
        rgbGen identity
    }
}

gfx/effects/sabers/swordTrail_black
{
	qer_editorimage	gfx/effects/sabers/blurglow
	nopicmip
	nomipmaps
	cull	twosided
    {
        clampmap gfx/effects/sabers/swordtrail
        blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
        glow
        rgbGen vertex
    }
}
