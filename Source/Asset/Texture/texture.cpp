#include "texture.h"
#include "basic.h"
#include "glad/glad.h"
#include "stb_image.h"

static void ClearTextureHandle(texture& Texture) {
	if (Texture.mRendererId != 0) {
		auto& Cache = texture::GetTextureCache();
		if (auto Found = Cache.Find(Texture.mPath)) {
			Found->mRefCount -= 1;
			if (Found->mRefCount == 0) {
				Cache.Remove(Texture.mPath);
				glDeleteTextures(1, &Texture.mRendererId);
			}
			Texture.mRendererId = 0;
		}
	}
}

texture::texture(texture&& Texture) noexcept {
	mRendererId = Texture.mRendererId;
	mPath = std::move(Texture.mPath);
	Texture.mRendererId = 0;
	Texture.mPath = {};
}

texture& texture::operator=(texture&& Texture) noexcept {
	ClearTextureHandle(*this);
	mRendererId = Texture.mRendererId;
	mPath = std::move(Texture.mPath);
	Texture.mRendererId = 0;
	Texture.mPath = {};
	return *this;
}

void texture::Load(const str& Path, bool SRGB) {
	ClearTextureHandle(*this);
	mPath = Path;
	auto& Cache = GetTextureCache();
	if (auto Found = Cache.Find(Path)) {
		Found->mRefCount += 1;
		mRendererId = Found->mResourceId;
	} else {
		s32 Width{0};
		s32 Height{0};
		s32 NumChannels{0};
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* mLocalBuffer = stbi_load(Path.GetRaw(), &Width, &Height, &NumChannels, 4);
		if (mLocalBuffer) {
			glGenTextures(1, &mRendererId);
			Cache[Path] = texture_record{mRendererId, 1};
			glBindTexture(GL_TEXTURE_2D, mRendererId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			u32 InternalType = GL_RGBA8;
			if (SRGB) {
				InternalType = GL_SRGB8_ALPHA8;
			}
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				static_cast<GLint>(InternalType),
				Width,
				Height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				mLocalBuffer);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(mLocalBuffer);
		}
	}
}

texture::~texture() {
	ClearTextureHandle(*this);
}

void texture::Bind(u32 Slot) const {
	glActiveTexture(GL_TEXTURE0 + Slot);
	glBindTexture(GL_TEXTURE_2D, mRendererId);
}
