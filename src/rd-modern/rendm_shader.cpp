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

uniform vec4 gcol;
layout(binding = 0) uniform sampler2D tex;

void main() {
	color = texture(tex, f_uv) * gcol;
}

)GLSL";

static GLuint program, vsh, fsh;

static GLuint u_gcol_loc;
static GLuint u_mm_loc;
static GLuint u_um_loc;

static std::unordered_map<std::string, qhandle_t> shader_map;
static std::unordered_map<qhandle_t, rendm::shader::construct *> shader_construct_map;
static qhandle_t handle_incrementor = 0;

static std::unordered_map<std::string, std::string> shader_lookup_table;

static bool preparse_shaders() {
	
	int num_shader_files;
	char * * shfiles = ri->FS_ListFiles("shaders", ".shader", &num_shader_files);
	for (int i = 0; i < num_shader_files; i++) {
		
		fileHandle_t f;
		char const * token, * p, * pOld;
		std::string name, shdata;
		int line;
		
		int len = ri->FS_FOpenFileRead(va("shaders/%s", shfiles[i]), &f, qfalse);
		std::string ff;
		ff.resize(len);
		ri->FS_Read(&ff[0], len, f);
		
		COM_BeginParseSession(shfiles[i]);
		p = ff.c_str();
		while (true) {
			token = COM_ParseExt(&p, qtrue);
			line = COM_GetCurrentParseLine();
			if (!token || !token[0]) break;
			if (token[0] == '#' || (token[0] == '/' && token[1] == '/')) {
				SkipRestOfLine(&p);
				continue;
			}
			name = token;
			pOld = p;
			token = COM_ParseExt(&p, qtrue);
			if (token[0] != '{' || token[1] != '\0') {
				ri->Printf(PRINT_WARNING, "WARNING: Shader (\"%s\") without brace section on line %i, aborting.\n", name.c_str(), line);
				break;
			}
			if (!SkipBracedSection(&p, 1)) {
				ri->Printf(PRINT_WARNING, "WARNING: Shader (\"%s\") seems to be missing closing brace, aborting.\n", name.c_str());
				break;
			}
			while (true) {
				switch (pOld [0]) {
					default:
						pOld++;
						assert(pOld < p);
						continue;
					case '{':
						break;
				}
				break;
			}
			shader_lookup_table[name] = {pOld, p};
		}
		ri->FS_FCloseFile(f);
	}
	ri->FS_FreeFileList(shfiles);
	
	return true;
}

static qboolean ParseVector( const char **text, int count, float *v ) {
	char	*token;
	int		i;

	// FIXME: spaces are currently required after parens, should change parseext...
	token = COM_ParseExt( text, qfalse );
	if ( strcmp( token, "(" ) ) {
		//ri->Printf( PRINT_ALL, S_COLOR_YELLOW  "WARNING: missing parenthesis in shader '%s'\n", shader.name );
		return qfalse;
	}

	for ( i = 0 ; i < count ; i++ ) {
		token = COM_ParseExt( text, qfalse );
		if ( !token[0] ) {
			//ri->Printf( PRINT_ALL, S_COLOR_YELLOW  "WARNING: missing vector element in shader '%s'\n", shader.name );
			return qfalse;
		}
		v[i] = atof( token );
	}

	token = COM_ParseExt( text, qfalse );
	if ( strcmp( token, ")" ) ) {
		//ri->Printf( PRINT_ALL, S_COLOR_YELLOW  "WARNING: missing parenthesis in shader '%s'\n", shader.name );
		return qfalse;
	}

	return qtrue;
}

static std::unordered_map<std::string, GLenum> blendfunc_map = {
	{"GL_ONE",						GL_ONE},
	{"GL_ZERO",						GL_ZERO},
	{"GL_SRC_COLOR",				GL_SRC_COLOR},
	{"GL_DST_COLOR",				GL_DST_COLOR},
	{"GL_SRC_ALPHA",				GL_SRC_ALPHA},
	{"GL_DST_ALPHA",				GL_DST_ALPHA},
	{"GL_ONE_MINUS_SRC_COLOR",		GL_ONE_MINUS_SRC_COLOR},
	{"GL_ONE_MINUS_DST_COLOR",		GL_ONE_MINUS_DST_COLOR},
	{"GL_ONE_MINUS_SRC_ALPHA",		GL_ONE_MINUS_SRC_ALPHA},
	{"GL_ONE_MINUS_DST_ALPHA",		GL_ONE_MINUS_DST_ALPHA},
};

static GLenum blendfunc_str2enum(char const * str) {
	auto const & i = blendfunc_map.find(str);
	if (i == blendfunc_map.end()) {
		ri->Printf(PRINT_WARNING, "WARNING: invalid blendfunc requested: (\"%s\"), defaulting to GL_ONE.\n", str);
		return GL_ONE;
	}
	else return i->second;
}

static bool parse_stage(char const * name, rendm::shader::stage & stg, char const * & p) {
	
	char const * token;
	
	while(true) {
		token = COM_ParseExt(&p, qtrue);
		
		if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") ends abruptly.\n", name);
			return false;
		}
		
		else if (token[0] == '}') break;
		
		else if (!Q_stricmp("map", token)) {
			token = COM_ParseExt(&p, qfalse);
			stg.diffuse = rendm::texture::load(token);
			if (!stg.diffuse) {
				ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") has invalid map (\"%s\"), could not find this image.\n", name, token);
				return false;
			}
		}
		
		else if (!Q_stricmp("blendFunc", token)) {
			token = COM_ParseExt(&p, qfalse);
			stg.blend_src = blendfunc_str2enum(token);
			token = COM_ParseExt(&p, qfalse);
			stg.blend_dst = blendfunc_str2enum(token);
		}
		
		else if (!Q_stricmp("rgbGen", token)) {
			token = COM_ParseExt(&p, qfalse);
			if (!Q_stricmp("const", token)) {
				vec3_t color;
				ParseVector(&p, 3, color);
				stg.gen = rendm::shader::stage::gen_type::rgb_const;
				stg.gen_r = color[0];
				stg.gen_g = color[1];
				stg.gen_b = color[2];
			} else {
				SkipRestOfLine(&p);
			}
		}
	}
	
	return true;
}

static rendm::shader::construct * parse_shader(char const * name, char const * data) {
	
	rendm::shader::construct * c = new rendm::shader::construct;
	
	char const * token, * p = data;
	COM_BeginParseSession("shader");
	
	token = COM_ParseExt(&p, qtrue);
	if (token[0] != '{') {
		goto fail;
	}
	
	while (true) {
		
		token = COM_ParseExt(&p, qtrue);
		
		if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: shader (\"%s\") ends abruptly.\n", name);
			goto fail;
		}
		
		else if (token[0] == '}') break;
		
		else if (token[0] == '{') {
			
			rendm::shader::stage stg;
			if (!parse_stage(name, stg, p)) {
				ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") failed to parse.\n", name);
				goto fail;
			}
			c->stages.push_back(stg);
		}
		
		else {
			
		}
		
	}
	
	return c;
	
	fail:
	
	delete c;
	return nullptr;
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
	
	u_gcol_loc = glGetUniformLocation(program, "gcol");
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
	
	auto f2 = shader_lookup_table.find(path);
	if (f2 == shader_lookup_table.end()) goto checktex;
	
	{
		construct * c = parse_shader(path, f2->second.c_str());
		if (!c) {
			ri->Printf(PRINT_WARNING, "WARNING: shader (\"%s\") failed to parse.\n", path);
			return -1;
		}
		
		qhandle_t nh = handle_incrementor++;
		shader_construct_map[nh] = c;
		return nh;
	}
	
	checktex:

	GLuint tex = texture::load(path);
	if (tex > 0) {
		qhandle_t nh = handle_incrementor++;
		construct * nc = new construct;
		stage stg;
		stg.diffuse = tex;
		nc->stages.push_back(stg);
		shader_construct_map[nh] =  nc;
		return nh;
	}
	
	ri->Printf(PRINT_WARNING, "WARNING: shader (\"%s\") not found.\n", path);
	return -1;
}

rendm::shader::construct const * rendm::shader::get(qhandle_t h) {
	auto const & i = shader_construct_map.find(h);
	if (i == shader_construct_map.end()) return nullptr;
	return i->second;
}

void rendm::shader::setup(construct const *) {
	// nothing yet
}

void rendm::shader::setup(stage const * stg) {
	glBindTextureUnit(0, stg->diffuse);
	glBlendFunc(stg->blend_src, stg->blend_dst);
	switch (stg->gen) {
		case stage::gen_type::none:
			break;
		case stage::gen_type::rgb_const:
			uniform_global_color(stg->gen_r, stg->gen_g, stg->gen_b, 1);
			break;
	}
}

void rendm::shader::uniform_global_color(float r, float g, float b, float a) {
	glUniform4f(u_gcol_loc, r, g, b, a);
}

void rendm::shader::uniform_mm(qm::mat3 const & m) {
	glUniformMatrix3fv(u_mm_loc, 1, GL_FALSE, m);
}

void rendm::shader::uniform_um(qm::mat3 const & m) {
	glUniformMatrix3fv(u_um_loc, 1, GL_FALSE, m);
}

