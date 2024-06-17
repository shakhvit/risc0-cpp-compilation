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
  char* result = (char*)malloc(sizeof(ciphertext) * 2 + 1);
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

#ifndef NO_SODIUM
  auto res = performCryptoOperations();
#endif

  env_commit(hasher, a.buffer, sizeof(a.buffer));
  env_exit(hasher, 0);

  return 0;
}
