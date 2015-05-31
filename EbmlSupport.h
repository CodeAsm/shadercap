#ifndef __EBML_SUPPORT_H__
#define __EBML_SUPPORT_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

typedef FILE* RTFILE;

#define RT_FAILURE(r) ((r)!=0)
#define RT_SUCCESS(r) ((r)==0)

#define RTFILE_SEEK_BEGIN SEEK_SET
#define RTFILE_SEEK_CUR SEEK_CUR
#define RTFILE_SEEK_END SEEK_END

enum VinfCodes {
  VINF_SUCCESS = 0
};

#define RT_H2BE_U16(x) ntoh16(x)
#define RT_H2BE_U32(x) ntoh32(x)
#define RT_H2BE_U64(x) ntoh64(x)

uint16_t ntoh16(const uint16_t x);
uint32_t ntoh32(const uint32_t x);
uint64_t ntoh64(const uint64_t x);


size_t RTFileWrite(RTFILE file, const void* buffer, size_t count, void* param);
size_t RTFileTell(RTFILE file);
size_t RTFileSeek(RTFILE file, size_t offset, int seek, void* param);

#define RTMemRealloc(x,y) realloc((x),(y))

#endif // __EBML_SUPPORT_H__
