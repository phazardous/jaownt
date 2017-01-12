#include "rendm_local.h"

bool rendm::init() {
	
	if (!model::init()) {
		srcprintf_error("could not initialize models");
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
	
	return true;
}

void rendm::term() {
	model::term();
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

void rendm::add_sprite(qm::mat4 const & m, qhandle_t sh) {
	shader::uniform_mm(m);
	glBindVertexArray(rendm::model::unitquad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
