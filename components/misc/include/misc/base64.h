/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#pragma once

unsigned char * base64_encode(const unsigned char *src, size_t len,
                  size_t *out_len);
unsigned char * base64_decode(const unsigned char *src, size_t len,
                  size_t *out_len);

// in-place variants to avoid malloc (bw-2019)
unsigned char * base64_encode2(const void *obj, size_t len, unsigned char *out,  size_t *out_len);
unsigned char * base64_decode2(const unsigned char *src, void  *obj, size_t *obj_size);

