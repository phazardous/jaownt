#pragma once

#include "tr_local.h"
#include "glad.h"

#include "q_math2.hpp"

namespace rendm {
	
	bool init();
	void term();
	
	void begin_frame();
	void end_frame();
	void draw (refdef_t const * rf);
	
	void add_sprite(qm::mat4 const & m, qhandle_t sh);
	
	namespace model {
		
		extern GLuint fullquad;
		extern GLuint unitquad;
		
		struct model_set {
			GLuint vao_handle;
			model_t * model;
		};
		
		bool init();
		void term();
	}
	
	namespace shader {
		
		bool init();
		void term();
		
		void uniform_mm(qm::mat4 const &);
		
	}
}
