#include "rendm_local.h"

bool rendm::init() {
	
	if (!model::init()) {
		srcprintf_error("could not initialize models");
		return false;
	}
	if (!texture::init()) {
		srcprintf_error("could not initialize textures");
		return false;
	}
	if (!shader::init()) {
		srcprintf_error("could not initialize shaders");
		return false;
	}
	
	glViewport(0, 0, glConfig.vidWidth, glConfig.vidHeight);
	glScissor(0, 0, glConfig.vidWidth, glConfig.vidHeight);
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_BLEND);
	
	return true;
}

void rendm::term() {
	model::term();
	texture::term();
	shader::term();
}

void rendm::begin_frame() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void rendm::end_frame() {
	ri->WIN_Present(&tr.window);
}

void rendm::draw (refdef_t const * rf) {
	
}

void rendm::add_sprite(qm::mat3 const & vm, qm::mat3 const & um, qhandle_t sh) {
	shader::construct const * c = shader::get(sh);
	if (!c) return;
	shader::uniform_mm(vm);
	shader::uniform_um(um);
	glBindVertexArray(rendm::model::unitquad);
	for (shader::stage const & stg : c->stages) {
		shader::setup(&stg);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}
