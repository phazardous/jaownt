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
	
	void add_sprite(qm::mat3 const & vm, qm::mat3 const & um, qhandle_t sh);
	
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
		
		struct stage {
			
			enum struct gen_type {
				none,
				rgb_const,
			};
			
			GLuint diffuse = 0; // GL Texture Handle
			GLenum blend_src = GL_SRC_ALPHA, blend_dst = GL_ONE_MINUS_SRC_ALPHA;
			
			gen_type gen = gen_type::none;
			float gen_r = 1;
			float gen_g = 1;
			float gen_b = 1;
			float gen_a = 1;
		};

		struct construct {
			std::vector<stage> stages;
		};
		
		bool init();
		void term();
		
		qhandle_t reg(char const *);

		construct const * get(qhandle_t);
		void setup(construct const *);
		void setup(stage const *);
		
		void uniform_global_color(float r, float g, float b, float a);
		void uniform_mm(qm::mat3 const &);
		void uniform_um(qm::mat3 const &);
		
	}
	
	namespace texture {
		
		bool init();
		void term();
		
		GLuint load(char const *);
		
	}
}
