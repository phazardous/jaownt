#pragma once

#include "tr_local.h"
#include "glad.h"

#include "q_math2.hpp"

namespace rendm {
	
	struct globals_t {
		float shader_time = 0;
		qm::vec4 color_mod {1, 1, 1, 1};
	};
	
	extern globals_t globals;
	
	bool init();
	void term();
	
	void begin_frame();
	void end_frame();
	void draw (refdef_t const * rf);
	
	void add_sprite(qm::mat4 const & vm, qm::mat3 const & um, qhandle_t sh);
	
	namespace model {
		
		extern GLuint fullquad;
		extern GLuint unitquad;
		
		struct model_set {
			GLuint vao_handle;
			model_t * model;
		};
		
		bool init();
		void term();
		
		qhandle_t reg (char const *);
	}
	
	namespace shader {
		
		enum struct gen_func {
			sine,
			square,
			triangle,
			sawtooth,
			inverse_sawtooth,
			noise,
			random,
		};
		
		struct texmod {
			
			enum struct tctype {
				turb,
				scale,
				scroll,
				stretch,
				transform,
				rotate,
				//entityTranslate, ???
			};
			
			tctype type;
			
			texmod() = delete;
			texmod(tctype t) : type(t) {
				
			}
			
			union {
				
				struct {
					float base;
					float amplitude;
					float phase;
					float frequency;
				} turb_data;
				
				struct {
					qm::vec2 scale;
				} scale_data;
				
				struct {
					qm::vec2 scroll;
				} scroll_data;
				
				struct {
					gen_func gfunc;
					float base;
					float amplitude;
					float phase;
					float frequency;
				} stretch_data;
				
				struct {
					qm::mat3 trans;
				} transform_data;
				
				struct {
					float rot;
				} rotate_data;
			};
		};
		
		struct stage {
			
			enum struct gen_type {
				none,
				constant,
				vertex,
				wave,
			};
			
			GLuint diffuse = 0; // GL Texture Handle
			bool clamp = false;
			GLenum blend_src = GL_ONE, blend_dst = GL_ZERO;
			
			gen_type gen_rgb = gen_type::none;
			gen_type gen_alpha = gen_type::none;
			
			qm::vec4 color {1, 1, 1, 1};
			
			struct {
				struct {
					gen_func func;
					float base;
					float amplitude;
					float phase;
					float frequency;
				} rgb;
				struct {
					gen_func func;
					float base;
					float amplitude;
					float phase;
					float frequency;
				} alpha;
			} wave;
			
			std::vector<texmod> texmods;
		};

		struct construct {
			std::vector<stage> stages;
		};
		
		bool init();
		void term();
		
		qhandle_t reg(char const *, bool trans = false);

		construct const * get(qhandle_t);
		void setup(construct const *);
		void setup(stage const *, qm::mat4 const & vert_mat, qm::mat3 const & uv_mat);

		void uniform_global_color(qm::vec4 const &);
		
	}
	
	namespace texture {
		
		bool init();
		void term();
		
		GLuint load(char const *);
		
	}
}
