#include "rendm_local.h"
#include "rd-common/tr_common.h"

#include <unordered_map>
#include <vector>

std::unordered_map<std::string, GLuint> open_textures;

bool rendm::texture::init() {
	R_ImageLoader_Init();
	return true;
}

void rendm::texture::term() {
	//if (open_textures.size()) glDeleteTextures(open_textures.size(), open_textures.data());
	for (auto & i : open_textures) {
		glDeleteTextures(1, &i.second);
	}
	open_textures.clear();
}

GLuint rendm::texture::load(char const * name) {
	
	char sname [MAX_QPATH];
	COM_StripExtension(name, sname, MAX_QPATH);
	
	auto f = open_textures.find(sname);
	if (f != open_textures.end()) return f->second;
	
	byte * idat;
	int width, height;
	
	R_LoadImage( sname, &idat, &width, &height );
	if (!idat) return -1;
	
	GLuint tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(tex, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(tex, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, idat);
	
	open_textures[sname] = tex;
	return tex;
}
