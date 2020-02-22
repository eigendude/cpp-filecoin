/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "filecoin/crypto/bls/libbls_signatures.h"

AggregateResponse *aggregate(const uint8_t *flattened_signatures_ptr,
                             size_t flattened_signatures_len) { return NULL; }

void destroy_aggregate_response(AggregateResponse *ptr) { }

void destroy_hash_response(HashResponse *ptr) { }

void destroy_private_key_generate_response(PrivateKeyGenerateResponse *ptr) { }

void destroy_private_key_public_key_response(PrivateKeyPublicKeyResponse *ptr) { }

void destroy_private_key_sign_response(PrivateKeySignResponse *ptr) { }

HashResponse *hash(const uint8_t *message_ptr, size_t message_len) { return NULL; }

PrivateKeyGenerateResponse *private_key_generate(void) { return NULL; }

PrivateKeyPublicKeyResponse *private_key_public_key(const uint8_t *raw_private_key_ptr) { return NULL; }

PrivateKeySignResponse *private_key_sign(const uint8_t *raw_private_key_ptr,
                                         const uint8_t *message_ptr,
                                         size_t message_len) { return NULL; }

int verify(const uint8_t *signature_ptr,
           const uint8_t *flattened_digests_ptr,
           size_t flattened_digests_len,
           const uint8_t *flattened_public_keys_ptr,
           size_t flattened_public_keys_len) { return -1; }
