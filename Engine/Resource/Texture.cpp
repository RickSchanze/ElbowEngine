/**
 * @file Texture.cpp
 * @author Echo 
 * @Date 24-5-7
 * @brief 
 */

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "CoreGlobal.h"
#include "stb_image.h"

Resource::Texture::~Texture() {
    stbi_image_free(mData);
    mData = nullptr;
}

void Resource::Texture::LoadTexture() {
    if (!mPath.IsExist()) {
        LOG_ERROR_CATEGORY(Resource, L"{}不存在", mPath.ToString());
    }
    const AnsiString PathStr = mPath.ToAnsiString();

    mData = stbi_load(PathStr.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
    if (!mData) {
        LOG_ERROR_CATEGORY(Resource, L"加载纹理{}失败", mPath.ToString());
    }
}
