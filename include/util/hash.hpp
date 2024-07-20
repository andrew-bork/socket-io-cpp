#pragma once

#include <openssl/evp.h>
#include <stdexcept>
#include <string>

namespace hash {

    struct sha1 {
        EVP_MD_CTX *_hasher_ctx = NULL;
        inline sha1() {

            if((_hasher_ctx = EVP_MD_CTX_new()) == NULL)
                throw std::runtime_error("Failed to initialize SHA1 Hasher.");

            if(1 != EVP_DigestInit_ex(_hasher_ctx, EVP_sha1(), NULL))
                throw std::runtime_error("Failed to initialize SHA1 Hasher.");

        } 

        inline void hash(const std::string& data) {
            if(1 != EVP_DigestUpdate(_hasher_ctx, data.data(), data.length()))
                throw std::runtime_error("Failed to hash using SHA1.");
        }

        inline std::string get() {
            std::string out;
            out.resize(EVP_MD_size(EVP_sha1()));
            if(1 != EVP_DigestFinal_ex(_hasher_ctx, reinterpret_cast<unsigned char*>(&out[0]), NULL))
		        throw std::runtime_error("Failed to retrieve SHA1 hash.");
            return out;
        }

        inline ~sha1() {
            if(_hasher_ctx != NULL) EVP_MD_CTX_free(_hasher_ctx);
        }
    };

};