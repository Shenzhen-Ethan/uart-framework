#include <stdio.h>
#include <stdint.h>

uint8_t read_uint8(const uint8_t* data) 
{
	return *data;
}

uint16_t read_uint16(const uint8_t* data) 
{
	return (data[0] << 8) | data[1];
}

uint32_t read_uint32(const uint8_t* data) 
{
	return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

uint64_t read_uint64(const uint8_t* data) 
{
	uint64_t high_part = ((uint64_t)data[0] << 56) | ((uint64_t)data[1] << 48) | ((uint64_t)data[2] << 40) | ((uint64_t)data[3] << 32) |
		((uint64_t)data[4] << 24) | ((uint64_t)data[5] << 16) | ((uint64_t)data[6] << 8) | (uint64_t)data[7];
	return high_part;
}

void write_uint8(uint8_t* data, uint8_t value) 
{
	*data = value;
}

void write_uint16(uint8_t* data, uint16_t value) 
{
	data[0] = value >> 8;
	data[1] = value & 0xFF;
}

void write_uint32(uint8_t* data, uint32_t value) 
{
	data[0] = value >> 24;
	data[1] = (value >> 16) & 0xFF;
	data[2] = (value >> 8) & 0xFF;
	data[3] = value & 0xFF;
}

void write_uint64(uint8_t* data, uint64_t value) 
{
	data[0] = value >> 56;
	data[1] = (value >> 48) & 0xFF;
	data[2] = (value >> 40) & 0xFF;
	data[3] = (value >> 32) & 0xFF;
	data[4] = (value >> 24) & 0xFF;
	data[5] = (value >> 16) & 0xFF;
	data[6] = (value >> 8) & 0xFF;
	data[7] = value & 0xFF;
}
