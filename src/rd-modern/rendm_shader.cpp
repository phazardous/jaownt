#include "rendm_local.h"

static char const * sh_glsl_v = 
R"GLSL_V(

#version 450
	
layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

uniform mat4 mm;

out vec2 f_uv;

void main() {
	f_uv = uv;
	gl_Position = mm * vec4(vert, 1);
}

)GLSL_V";

static char const * sh_glsl_f = 
R"GLSL(

#version 450
	
in vec2 f_uv;
out vec4 color;

void main() {
	color = vec4(f_uv.x, f_uv.y, 0, 1);
}

)GLSL";

static GLuint program, vsh, fsh;

static GLuint u_mm_loc;

bool rendm::shader::init() {
	
	GLint success;
	
	program = glCreateProgram();
	vsh = glCreateShader(GL_VERTEX_SHADER);
	fsh = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vsh, 1, &sh_glsl_v, 0);
	glCompileShader(vsh);
	glGetShaderiv(vsh, GL_COMPILE_STATUS, &success);
	if (!success) {
		char * err = va_next();
		GLsizei max_length = MAX_VA_STRING;
		glGetShaderInfoLog(vsh, max_length, &max_length, err);
		srcprintf_error("vertex shader failed to compile:\n%s", err);
		return false;
	}
	
	glShaderSource(fsh, 1, &sh_glsl_f, 0);
	glCompileShader(fsh);
	glGetShaderiv(fsh, GL_COMPILE_STATUS, &success);
	if (!success) {
		char * err = va_next();
		GLsizei max_length = MAX_VA_STRING;
		glGetShaderInfoLog(fsh, max_length, &max_length, err);
		srcprintf_error("fragment shader failed to compile:\n%s", err);
		return false;
	}
	
	glAttachShader(program, vsh);
	glAttachShader(program, fsh);
	
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char * err = va_next();
		GLsizei max_length = MAX_VA_STRING;
		glGetProgramInfoLog(program, max_length, &max_length, err);
		srcprintf_error("shader program failed to link:\n%s", err);
		return false;
	}
	
	glUseProgram(program);
	
	u_mm_loc = glGetUniformLocation(program, "mm");
	
	return true;
}

void rendm::shader::term() {
	glDeleteShader(vsh);
	glDeleteShader(fsh);
	glDeleteProgram(program);
}

void rendm::shader::uniform_mm(qm::mat4 const & m) {
	glUniformMatrix4fv(u_mm_loc, 1, GL_FALSE, m);
}
