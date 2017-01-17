#include "rendm_local.h"
#include "rd-common/tr_common.h"

#include <unordered_map>
#include <vector>

std::unordered_map<std::string, GLuint> open_textures;

static GLuint whiteimage;

bool rendm::texture::init() {
	R_ImageLoader_Init();
	
	glCreateTextures(GL_TEXTURE_2D, 1, &whiteimage);
	glTextureParameteri(whiteimage, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(whiteimage, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(whiteimage, 1, GL_RGBA8, 1, 1);
	glTextureSubImage2D(whiteimage, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, "\xFF\xFF\xFF\xFF");
	
	return true;
}

void rendm::texture::term() {
	glDeleteTextures(1, &whiteimage);
	for (auto & i : open_textures) {
		glDeleteTextures(1, &i.second);
	}
	open_textures.clear();
}

GLuint rendm::texture::load(char const * name) {
	
	if (!strcmp(name, "*white")) {
		return whiteimage;
	}
	if (!strcmp(name, "$whiteimage")) {
		return whiteimage;
	}
	
	char sname [MAX_QPATH];
	COM_StripExtension(name, sname, MAX_QPATH);
	
	auto f = open_textures.find(sname);
	if (f != open_textures.end()) return f->second;
	
	byte * idat;
	int width, height;
	
	R_LoadImage( sname, &idat, &width, &height );
	if (!idat) return 0;
	
	GLuint tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(tex, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(tex, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, idat);
	
	open_textures[sname] = tex;
	return tex;
}
