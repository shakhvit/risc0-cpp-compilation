// Copyright 2024 RISC Zero, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "platform.h"
#include <stdint.h>
#include <cstring>

#define POOL_SIZE 10 * 1024 * 1024 // 10 MB for example
static uint8_t memory_pool[POOL_SIZE];
static size_t current_offset = 0;

// Simple custom allocation function
static void* custom_alloc(size_t size) {
  if (current_offset + size > POOL_SIZE)
    return NULL; // Out of memory
  void* block = memory_pool + current_offset;
  current_offset += size; // Move the offset
  return block;
}

// Simple custom free function
static void custom_free(void* ptr) {
  // In this simple example, we do not actually reclaim memory.
  // A more complex version could manage free blocks.
}

// Simple custom realloc function
static void* custom_realloc(void* ptr, size_t old_size, size_t new_size) {
  if (new_size <= old_size)
    return ptr; // If reducing size or same size, return the original pointer.
  void* new_ptr = custom_alloc(new_size);
  if (new_ptr) {
    memcpy(new_ptr, ptr, old_size); // Copy old data to new location
    custom_free(ptr);               // Free old block
  }
  return new_ptr;
}


#ifndef NO_UTF
#include <utf8proc.h>


// Initialization function that sets custom memory allocators
static void utf8proc_initializer() {
  utf8proc_set_custom_allocators(custom_alloc, custom_realloc, custom_free);
}


char* normalize_utf8(const char* input) {
  // Convert the input string to UTF-8 normalized form NFC (Normalization Form Composed)
  utf8proc_uint8_t* result;
  utf8proc_ssize_t result_len;
  result_len =
      utf8proc_map((const utf8proc_uint8_t*)input,
                   0,
                   &result,
                   (utf8proc_option_t)(UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_COMPOSE));

  if (result_len < 0) {
    return NULL;
  }

  // The result is already allocated by utf8proc_map, so just return it
  return (char*)result;
}

#endif

#ifndef NO_SODIUM
#include <sodium.h>
#include <cstring>

char* performCryptoOperations() {
  // Initialize libsodium
  if (sodium_init() == -1) {
    return nullptr;
  }

  // Generate keypair
  unsigned char pk[crypto_box_PUBLICKEYBYTES];
  unsigned char sk[crypto_box_SECRETKEYBYTES];
  crypto_box_keypair(pk, sk);

  // Define message and hash it
  const char* message = "Hello world";
  unsigned char hash[crypto_generichash_BYTES];
  crypto_generichash(
      hash, sizeof(hash), (const unsigned char*)message, strlen(message), nullptr, 0);

  // Encrypt the hash
  unsigned char nonce[crypto_box_NONCEBYTES];
  unsigned char ciphertext[sizeof(hash) + crypto_box_MACBYTES];
  randombytes_buf(nonce, sizeof(nonce));
  auto res = crypto_box_easy(ciphertext, hash, sizeof(hash), nonce, pk, sk);

  // Prepare hexadecimal string for output
  char* result = (char*)custom_alloc(sizeof(ciphertext) * 2 + 1);
  if (result == nullptr) {
    return nullptr; // Allocation failed
  }
  for (size_t i = 0; i < sizeof(ciphertext); i++) {
    sprintf(result + i * 2, "%02x", ciphertext[i]);
  }

  return result; // Caller must free this memory
}

#endif

union u32_cast {
  int32_t value;
  uint8_t buffer[4];
};
int main() {
  // TODO introduce entropy into memory image (for zk)
  sha256_state* hasher = init_sha256();

  // Read two u32 values from the host, assuming LE byte order.
  union u32_cast a;
  union u32_cast b;
  env_read(a.buffer, 4);
  env_read(b.buffer, 4);

  a.value *= b.value;

#ifndef NO_UTF
  utf8proc_initializer();
  const char* input = "Hello world!";
  auto res_utf = normalize_utf8( input );
#endif



#ifndef NO_SODIUM
  auto res = performCryptoOperations();
#endif


  env_commit(hasher, a.buffer, sizeof(a.buffer));
  env_exit(hasher, 0);

  return 0;
}
