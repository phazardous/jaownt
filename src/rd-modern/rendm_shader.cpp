#include "rendm_local.h"

#include <unordered_map>
#include <experimental/string_view> // FIXME -- string_view was accepted into C++17, but was not available at the time of writing, switch when available

static char const * sh_glsl_v = 
R"GLSL(

#version 450
	
layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

uniform mat3 mm;
uniform mat3 um;

out vec2 f_uv;

void main() {
	f_uv = (um * vec3(uv, 1)).xy;
	gl_Position = vec4(mm * vec3(vert.xy, 1), 1);
}

)GLSL";

static char const * sh_glsl_f = 
R"GLSL(

#version 450
	
in vec2 f_uv;
out vec4 color;

layout(binding = 0) uniform sampler2D tex;

void main() {
	color = texture(tex, f_uv);
}

)GLSL";

static GLuint program, vsh, fsh;

static GLuint u_mm_loc;
static GLuint u_um_loc;

static std::unordered_map<std::string, qhandle_t> shader_map;
static std::unordered_map<qhandle_t, GLuint> shader_image_map;
static qhandle_t handle_incrementor = 0;

static std::string shader_buffer;
static std::unordered_map<std::experimental::string_view, char const *> shader_lookup_table;

static bool preparse_shaders() {
	
	int num_shader_files;
	char * * shfiles = ri->FS_ListFiles("shaders", ".shader", &num_shader_files);
	for (int i = 0; i < num_shader_files; i++) {
		fileHandle_t f;
		int len = ri->FS_FOpenFileRead(va("shaders/%s", shfiles[i]), &f, qfalse);
		std::string ff;
		ff.resize(len);
		ri->FS_Read(&ff[0], len, f);
		shader_buffer += ff;
		ri->FS_FCloseFile(f);
	}
	ri->FS_FreeFileList(shfiles);
	
	Com_Printf("%i shader files loaded, creating a shader buffer %zu bytes long.\n", num_shader_files, shader_buffer.size());
	
	return true;
}

bool rendm::shader::init() {
	
	if (!preparse_shaders()) return false;
	
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
	u_um_loc = glGetUniformLocation(program, "um");
	
	return true;
}

void rendm::shader::term() {
	glDeleteShader(vsh);
	glDeleteShader(fsh);
	glDeleteProgram(program);
	handle_incrementor = 0;
}

qhandle_t rendm::shader::reg(char const * path) {
	auto f = shader_map.find(path);
	if (f != shader_map.end()) return f->second;
	
	GLuint tex = texture::load(path);
	if (tex == -1) return -1;
	
	qhandle_t nh = handle_incrementor++;
	shader_image_map[nh] =  tex;
	
	return nh;
}

GLuint rendm::shader::get(qhandle_t h) {
	return shader_image_map[h];
}

void rendm::shader::uniform_mm(qm::mat3 const & m) {
	glUniformMatrix3fv(u_mm_loc, 1, GL_FALSE, m);
}

void rendm::shader::uniform_um(qm::mat3 const & m) {
	glUniformMatrix3fv(u_um_loc, 1, GL_FALSE, m);
}

