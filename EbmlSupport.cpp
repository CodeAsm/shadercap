#include "EbmlSupport.h"

uint16_t ntoh16p(const uint16_t *input)
{
  uint16_t rval;
  uint8_t *data = (uint8_t *)&rval;
  data[0] = uint8_t(*input >> 8);
  data[1] = uint8_t(*input >> 0);
  return rval;
}

uint32_t ntoh32p(const uint32_t *input)
{
  uint32_t rval;
  uint8_t *data = (uint8_t *)&rval;
  data[0] = uint8_t(*input >> 24);
  data[1] = uint8_t(*input >> 16);
  data[2] = uint8_t(*input >> 8);
  data[3] = uint8_t(*input >> 0);
  return rval;
}

uint64_t ntoh64p(const uint64_t *input)
{
  /* http://stackoverflow.com/questions/809902/64-bit-ntohl-in-c */
  uint64_t rval;
  uint8_t *data = (uint8_t *)&rval;
  data[0] = uint8_t(*input >> 56);
  data[1] = uint8_t(*input >> 48);
  data[2] = uint8_t(*input >> 40);
  data[3] = uint8_t(*input >> 32);
  data[4] = uint8_t(*input >> 24);
  data[5] = uint8_t(*input >> 16);
  data[6] = uint8_t(*input >> 8);
  data[7] = uint8_t(*input >> 0);
  return rval;
}

uint16_t ntoh16(const uint16_t input)
{
  return ntoh16p(&input);
}

uint32_t ntoh32(const uint32_t input)
{
  return ntoh32p(&input);
}

uint64_t ntoh64(const uint64_t input)
{
  return ntoh64p(&input);
}

size_t RTFileWrite(RTFILE file, const void* buffer, size_t count, void* param)
{
  (void)param;
  size_t written = fwrite(buffer, 1, count, file);
  return (written == count ? 0 : 1);
}

size_t RTFileTell(RTFILE file) 
{
  return ftell(file);
}

size_t RTFileSeek(RTFILE file, size_t offset, int seek, void* param)
{
  (void)param;
  return fseek(file, offset, seek);
}
