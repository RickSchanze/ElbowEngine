/**
 * @file HashUtils.cpp
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#include "HashUtils.h"
#include "openssl/sha.h"
#include <openssl/evp.h>

Optional<AnsiString> HashUtils::ComputeSHA256(const Path& path)
{
    std::ifstream file(path.ToAbsoluteAnsiString(), std::ios::binary);
    if (!file)
    {
        return {};
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        return {};
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    const int     bufferSize = 1024;
    unsigned char buffer[bufferSize];

    while (file.read(reinterpret_cast<char*>(buffer), bufferSize) || file.gcount())
    {
        if (EVP_DigestUpdate(ctx, buffer, file.gcount()) != 1)
        {
            EVP_MD_CTX_free(ctx);
            return {};
        }
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int  hashLen;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}
