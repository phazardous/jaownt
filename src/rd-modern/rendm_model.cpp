#include "rendm_local.h"

#include <unordered_map>
#include <vector>

GLuint rendm::model::fullquad = 0;
GLuint rendm::model::unitquad = 0;

static GLfloat fullscreen_quad_verts [] = {
	-1, -1,  0,
	-1,  1,  0,
	 1, -1,  0,
	 1,  1,  0,
};

static GLfloat unit_quad_verts [] = {
	 0,  0,  0,
	 0,  1,  0,
	 1,  0,  0,
	 1,  1,  0,
};

static GLfloat quad_uvs [] = {
	 0,  0,
	 0,  1,
	 1,  0,
	 1,  1,
};

static qhandle_t handle_incrementor = 0;
#define nexthand (handle_incrementor++)

static std::unordered_map<qhandle_t, rendm::model::model_set> rendm_models;
static std::unordered_map<GLuint, std::vector<GLuint>> vbo_map;

static void setup_fullquad() {
	
	GLuint vao;
	model_t * fqm;
	qhandle_t handle = nexthand;
	
	fqm = new model_t {};
	fqm->type = MOD_MESH;
	strcpy(fqm->name, "Fullscreen Quad");
	fqm->index = handle;
	
	glCreateVertexArrays(1, &vao);
	std::vector<GLuint> & vbos = vbo_map[vao];
	vbos.resize(2);
	glCreateBuffers(2, vbos.data());
	
	rendm::model::fullquad = vao;
	
	GLuint vbo_v = vbos[0];
	glNamedBufferData(vbo_v, sizeof(fullscreen_quad_verts), fullscreen_quad_verts, GL_STATIC_DRAW);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo_v, 0, 12);
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	
	GLuint vbo_u = vbos[1];
	glNamedBufferData(vbo_u, sizeof(quad_uvs), quad_uvs, GL_STATIC_DRAW);
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayVertexBuffer(vao, 1, vbo_u, 0, 8);
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
}

static void setup_unitquad() {
	
	GLuint vao;
	model_t * uqm;
	qhandle_t handle = nexthand;
	
	uqm = new model_t {};
	uqm->type = MOD_MESH;
	strcpy(uqm->name, "Unit Quad");
	uqm->index = handle;
	
	glCreateVertexArrays(1, &vao);
	std::vector<GLuint> & vbos = vbo_map[vao];
	vbos.resize(2);
	glCreateBuffers(2, vbos.data());
	
	rendm::model::unitquad = vao;
	
	GLuint vbo_v = vbos[0];
	glNamedBufferData(vbo_v, sizeof(unit_quad_verts), unit_quad_verts, GL_STATIC_DRAW);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo_v, 0, 12);
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	
	GLuint vbo_u = vbos[1];
	glNamedBufferData(vbo_u, sizeof(quad_uvs), quad_uvs, GL_STATIC_DRAW);
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayVertexBuffer(vao, 1, vbo_u, 0, 8);
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
}

static void add_basic_meshes() {
	setup_fullquad();
	setup_unitquad();
}

bool rendm::model::init() {
	add_basic_meshes();
	return true;
}

void rendm::model::term() {
	for (auto & i : vbo_map) {
		glDeleteVertexArrays(1, &i.first);
		glDeleteBuffers(i.second.size(), i.second.data());
	}
	for (auto & i : rendm_models) {
		delete i.second.model;
	}
	vbo_map.clear();
	rendm_models.clear();
	handle_incrementor = 0;
}

qhandle_t rendm::model::reg(char const * path) {
	
	fileHandle_t f;
	char * buf;
	
	long len = ri->FS_FOpenFileRead(path, &f, qfalse);
	if (len <= 0) return 0;
	buf = new char [len];
	ri->FS_Read(buf, len, f);
	ri->FS_FCloseFile(f);
	int ident = *reinterpret_cast<int *>(buf);
	
	qhandle_t h = nexthand;
	
	switch (ident) {
		case MDXA_IDENT:
			Com_Printf("Ghoul2 Animation: %s\n", path);
			break;
		case MDXM_IDENT:
			Com_Printf("Ghoul2 Model: %s\n", path);
			break;
		case MD3_IDENT:
			Com_Printf("MD3 Model: %s\n", path);
			break;
	}
	
	delete [] buf;
	
	return h;
}









































