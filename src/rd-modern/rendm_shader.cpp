#include "rendm_local.h"

#include <unordered_map>
#include <experimental/string_view> // FIXME -- string_view was accepted into C++17, but was not available at the time of writing, switch when available

static char const * sh_glsl_v = 
R"GLSL(

#version 450
	
layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

uniform mat4 mm;
uniform mat3 um;

out vec2 f_uv;

void main() {
	f_uv = (um * vec3(uv, 1)).xy;
	gl_Position = mm * vec4(vert, 1);
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

static GLuint tex_sampler;

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
	if (!str) return GL_ONE;
	auto const & i = blendfunc_map.find(str);
	if (i == blendfunc_map.end()) {
		ri->Printf(PRINT_WARNING, "WARNING: invalid blendfunc requested: (\"%s\"), defaulting to GL_ONE.\n", str);
		return GL_ONE;
	}
	else return i->second;
}

static std::unordered_map<std::string, rendm::shader::gen_func> genfunc_map = {
	{"sin",					rendm::shader::gen_func::sine},
	{"square",				rendm::shader::gen_func::square},
	{"triangle",			rendm::shader::gen_func::triangle},
	{"sawtooth",			rendm::shader::gen_func::sawtooth},
	{"inverse_sawtooth",	rendm::shader::gen_func::inverse_sawtooth},
	{"noise",				rendm::shader::gen_func::noise},
	{"random",				rendm::shader::gen_func::random},
};

static rendm::shader::gen_func genfunc_str2enum(char const * str) {
	if (!str) return rendm::shader::gen_func::random;
	auto const & i = genfunc_map.find(str);
	if (i == genfunc_map.end()) {
		ri->Printf(PRINT_WARNING, "WARNING: invalid genfunc requested: (\"%s\"), defaulting to random.\n", str);
		return rendm::shader::gen_func::random;
	}
	else return i->second;
}

static float gen_func_do(rendm::shader::gen_func func, float in, float base, float amplitude, float phase, float frequency) {
	switch (func) {
		default: 
			break;
		case rendm::shader::gen_func::sine:
			return base + sin((in + phase) * frequency * qm::t_pi<float>(2)) * amplitude;
	}
	return 0;
}

static void parse_texmod(char const * name, rendm::shader::stage & stg, char const * p) {
	
	char const * token = COM_ParseExt( &p, qfalse );
	
	if (!Q_stricmp( token, "turb")) {
		
		stg.texmods.emplace_back(rendm::shader::texmod::tctype::turb);
		auto & tc = stg.texmods.back();
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod turb parm \"base\" in shader (\"%s\")\n", name);
			return;
		}
		tc.turb_data.base = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod turb parm \"amplitude\" in shader (\"%s\")\n", name);
			return;
		}
		tc.turb_data.amplitude = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod turb parm \"phase\" in shader (\"%s\")\n", name);
			return;
		}
		tc.turb_data.phase = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod turb parm \"frequency\" in shader (\"%s\")\n", name);
			return;
		}
		tc.turb_data.frequency = strtof(token, nullptr);
		
	} else if (!Q_stricmp( token, "scale")) {
		
		stg.texmods.emplace_back(rendm::shader::texmod::tctype::scale);
		auto & tc = stg.texmods.back();
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod scale parm \"x\" in shader (\"%s\")\n", name);
			return;
		}
		tc.scale_data.scale[0] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod scale parm \"y\" in shader (\"%s\")\n", name);
			return;
		}
		tc.scale_data.scale[1] = strtof(token, nullptr);
		
	} else if (!Q_stricmp( token, "scroll")) {
		
		stg.texmods.emplace_back(rendm::shader::texmod::tctype::scroll);
		auto & tc = stg.texmods.back();
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod scroll parm \"x\" in shader (\"%s\")\n", name);
			return;
		}
		tc.scroll_data.scroll[0] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod scroll parm \"y\" in shader (\"%s\")\n", name);
			return;
		}
		tc.scroll_data.scroll[1] = strtof(token, nullptr);
		
	} else if (!Q_stricmp( token, "stretch")) {
		
		stg.texmods.emplace_back(rendm::shader::texmod::tctype::stretch);
		auto & tc = stg.texmods.back();
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"genfunc\" in shader (\"%s\")\n", name);
			return;
		}
		tc.stretch_data.gfunc = genfunc_str2enum(token);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"base\" in shader (\"%s\")\n", name);
			return;
		}
		tc.stretch_data.base = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"amplitude\" in shader (\"%s\")\n", name);
			return;
		}
		tc.stretch_data.amplitude = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"phase\" in shader (\"%s\")\n", name);
			return;
		}
		tc.stretch_data.phase= strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"frequency\" in shader (\"%s\")\n", name);
			return;
		}
		tc.stretch_data.frequency = strtof(token, nullptr);
		
	} else if (!Q_stricmp( token, "transform")) {

		stg.texmods.emplace_back(rendm::shader::texmod::tctype::transform);
		auto & tc = stg.texmods.back();
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod transform parm \"[0][0]\" in shader (\"%s\")\n", name);
			return;
		}
		tc.transform_data.trans[0][0] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod transform parm \"[0][1]\" in shader (\"%s\")\n", name);
			return;
		}
		tc.transform_data.trans[0][1] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod transform parm \"[1][0]\" in shader (\"%s\")\n", name);
			return;
		}
		tc.transform_data.trans[1][0] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod transform parm \"[1][1]\" in shader (\"%s\")\n", name);
			return;
		}
		tc.transform_data.trans[1][1] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod transform parm \"[2][0]\" in shader (\"%s\")\n", name);
			return;
		}
		tc.transform_data.trans[2][0] = strtof(token, nullptr);
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod transform parm \"[2][1]\" in shader (\"%s\")\n", name);
			return;
		}
		tc.transform_data.trans[2][1] = strtof(token, nullptr);
		
	} else if (!Q_stricmp( token, "rotate")) {

		stg.texmods.emplace_back(rendm::shader::texmod::tctype::rotate);
		auto & tc = stg.texmods.back();
		token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
			ri->Printf(PRINT_WARNING, "WARNING: missing tcMod rotate parm \"value\" in shader (\"%s\")\n", name);
			return;
		}
		tc.rotate_data.rot = strtof(token, nullptr);
		tc.rotate_data.rot = qm::t_deg2rad(tc.rotate_data.rot);
	} else {
		ri->Printf(PRINT_WARNING, "WARNING: unknown tcMod parameter \"%s\" in shader (\"%s\")\n", token, name);
		return;
	}
}

static qboolean parse_vector ( const char **text, int count, float *v ) {
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
		
		else if (!Q_stricmp("clampmap", token)) {
			token = COM_ParseExt(&p, qfalse);
			stg.diffuse = rendm::texture::load(token);
			if (!stg.diffuse) {
				ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") has invalid clampmap (\"%s\"), could not find this image.\n", name, token);
				return false;
			}
			stg.clamp = true;
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
				parse_vector(&p, 3, color);
				stg.gen_rgb = rendm::shader::stage::gen_type::constant;
				stg.color[0] = color[0];
				stg.color[1] = color[1];
				stg.color[2] = color[2];
			} else if (!Q_stricmp("identity", token)) {
				stg.gen_rgb = rendm::shader::stage::gen_type::constant;
				stg.color[0] = 1;
				stg.color[1] = 1;
				stg.color[2] = 1;
			} else if (!Q_stricmp("vertex", token)) {
				stg.gen_rgb = rendm::shader::stage::gen_type::vertex;
			} else if (!Q_stricmp("wave", token)) {
				stg.gen_rgb = rendm::shader::stage::gen_type::wave;
				token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
					ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"genfunc\" in shader (\"%s\")\n", name);
				}
				stg.wave.rgb.func = genfunc_str2enum(token);
				token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
					ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"base\" in shader (\"%s\")\n", name);
				}
				stg.wave.rgb.base = strtof(token, nullptr);
				token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
					ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"amplitude\" in shader (\"%s\")\n", name);
				}
				stg.wave.rgb.amplitude = strtof(token, nullptr);
				token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
					ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"phase\" in shader (\"%s\")\n", name);
				}
				stg.wave.rgb.phase= strtof(token, nullptr);
				token = COM_ParseExt( &p, qfalse ); if (!token[0]) {
					ri->Printf(PRINT_WARNING, "WARNING: missing tcMod stretch parm \"frequency\" in shader (\"%s\")\n", name);
				}
				stg.wave.rgb.frequency = strtof(token, nullptr);
			} else {
				ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") has unknown/invalid rgbGen (\"%s\").\n", name, token);
				SkipRestOfLine(&p);
			}
		}
		
		else if (!Q_stricmp("alphaGen", token)) {
			token = COM_ParseExt(&p, qfalse);
			if (!Q_stricmp("const", token)) {
				token = COM_ParseExt(&p, qfalse);
				stg.gen_alpha = rendm::shader::stage::gen_type::constant;
				stg.color[3] = strtof(token, nullptr);
			} else if (!Q_stricmp("identity", token)) {
				stg.gen_alpha = rendm::shader::stage::gen_type::constant;
				stg.color[3] = 1;
			} else if (!Q_stricmp("vertex", token)) {
				stg.gen_alpha = rendm::shader::stage::gen_type::vertex;
			} else {
				ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") has unknown/invalid alphaGen (\"%s\").\n", name, token);
				SkipRestOfLine(&p);
			}
		}
		
		else if (!Q_stricmp(token, "tcMod")) {
			char buffer [1024] = "";
			while (true) {
				token = COM_ParseExt(&p, qfalse);
				if (!token[0]) break;
				Q_strcat( buffer, sizeof( buffer ), token );
				Q_strcat( buffer, sizeof( buffer ), " " );
			}
			parse_texmod(name, stg, buffer);
			
		} else if (!Q_stricmp(token, "glow")) {
			//TODO
		} else {
			
			ri->Printf(PRINT_WARNING, "WARNING: shader stage for (\"%s\") has unknown/invalid key (\"%s\").\n", name, token);
			SkipRestOfLine(&p);
			
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
		
		else if (!Q_stricmp(token, "nopicmip")) {
			SkipRestOfLine(&p);
			// TODO
		}
		
		else if (!Q_stricmp(token, "nomipmaps")) {
			SkipRestOfLine(&p);
			// TODO
		}
		
		else if (!Q_stricmp(token, "qer_editorimage")) {
			SkipRestOfLine(&p);
		}
		
		else {
			
			ri->Printf(PRINT_WARNING, "WARNING: shader (\"%s\") has unknown/invalid key (\"%s\").\n", name, token);
			SkipRestOfLine(&p);
			
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
	
	glCreateSamplers(1, &tex_sampler);
	glBindSampler(0, tex_sampler);
	
	return true;
}

void rendm::shader::term() {
	glDeleteShader(vsh);
	glDeleteShader(fsh);
	glDeleteProgram(program);
	glDeleteSamplers(1, &tex_sampler);
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
		shader_map[path] = nh;
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
		shader_map[path] = nh;
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

void rendm::shader::setup(stage const * stg, qm::mat4 const & vert_mat, qm::mat3 const & uv_mat) {
	glBindTextureUnit(0, stg->diffuse);
	glBlendFunc(stg->blend_src, stg->blend_dst);
	
	qm::vec4 uc {1, 1, 1, 1};
	
	if (stg->gen_rgb == stage::gen_type::constant) {
		uc[0] = stg->color[0];
		uc[1] = stg->color[1];
		uc[2] = stg->color[2];
	} else if (stg->gen_rgb == stage::gen_type::vertex) {
		uc[0] = globals.color_mod[0];
		uc[1] = globals.color_mod[1];
		uc[2] = globals.color_mod[2];
	} else if (stg->gen_rgb == stage::gen_type::wave) {
		float mult = gen_func_do(stg->wave.rgb.func, globals.shader_time, stg->wave.rgb.base, stg->wave.rgb.amplitude, stg->wave.rgb.phase, stg->wave.rgb.frequency);
		uc[0] = mult;
		uc[1] = mult;
		uc[2] = mult;
	} else {
		uc[0] = stg->color[0] * globals.color_mod[0];
		uc[1] = stg->color[1] * globals.color_mod[1];
		uc[2] = stg->color[2] * globals.color_mod[2];
	}
	
	if (stg->gen_alpha == stage::gen_type::constant) {
		uc[3] = stg->color[3];
	} else if (stg->gen_alpha == stage::gen_type::vertex) {
		uc[3] = globals.color_mod[3];
	} else {
		uc[3] = stg->color[3] * globals.color_mod[3];
	}
	
	uniform_global_color(uc);

	glUniformMatrix4fv(u_mm_loc, 1, GL_FALSE, vert_mat);
	if (stg->texmods.size() > 0) {
		qm::mat3 m = uv_mat;
		for (texmod const & t : stg->texmods) {
			switch (t.type) {
				case texmod::tctype::scale:
					m *= qm::mat3::scale(t.scale_data.scale[0], t.scale_data.scale[1]);
					break;
				case texmod::tctype::scroll:
					m *= qm::mat3::translate(t.scroll_data.scroll[0] * globals.shader_time, t.scroll_data.scroll[1] * globals.shader_time);
					break;
				case texmod::tctype::transform:
					m *= t.transform_data.trans;
					break;
				case texmod::tctype::rotate:
					m *= qm::mat3::translate(-0.5, -0.5);
					m *= qm::mat3::rotate(t.rotate_data.rot * globals.shader_time);
					m *= qm::mat3::translate(0.5, 0.5);
					break;
				default:
					ri->Printf(PRINT_WARNING, "WARNING: Unknown texmod type (%i) in shader stage!\n", static_cast<int>(t.type));
					break;
			}
		}
		glUniformMatrix3fv(u_um_loc, 1, GL_FALSE, m);
	} else glUniformMatrix3fv(u_um_loc, 1, GL_FALSE, uv_mat);
	if (stg->clamp) {
		glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	} else {
		glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

void rendm::shader::uniform_global_color(qm::vec4 const & c) {
	glUniform4fv(u_gcol_loc, 1, c);
}
