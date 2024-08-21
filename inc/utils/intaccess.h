/*
 * Copyright 2023 Ethan. All rights reserved.
 */

#ifndef __INTACCESS_H__
#define __INTACCESS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /** __cplusplus*/

uint8_t read_uint8(const uint8_t* data);

uint16_t read_uint16(const uint8_t* data); 

uint32_t read_uint32(const uint8_t* data);

uint64_t read_uint64(const uint8_t* data);

void write_uint8(uint8_t* data, uint8_t value);

void write_uint16(uint8_t* data, uint16_t value);

void write_uint32(uint8_t* data, uint32_t value);

void write_uint64(uint8_t* data, uint64_t value);
#ifdef __cplusplus
}
#endif /** __cplusplus*/
#endif /** __INTACCESS_H__*/

