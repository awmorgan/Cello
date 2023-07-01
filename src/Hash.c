#include "Cello.h"

var Hash = Cello(Hash);

uint64_t hash_data(const void *data, size_t size) {

  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;
  const uint64_t *d = (const uint64_t *)data;
  const uint64_t *end = d + (size / 8);

  uint64_t h = 0xCe110 ^ (size * m);

  while (d != end) {
    uint64_t k = *d++;
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
  }

  const unsigned char *data2 = (const unsigned char *)d;

  switch (size & 7) {
  case 7:
    h ^= (uint64_t)(data2[6]) << 48;
  case 6:
    h ^= (uint64_t)(data2[5]) << 40;
  case 5:
    h ^= (uint64_t)(data2[4]) << 32;
  case 4:
    h ^= (uint64_t)(data2[3]) << 24;
  case 3:
    h ^= (uint64_t)(data2[2]) << 16;
  case 2:
    h ^= (uint64_t)(data2[1]) << 8;
  case 1:
    h ^= (uint64_t)(data2[0]);
    h *= m;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

uint64_t hash(var self) {

  struct Hash *h = instance(self, Hash);
  if (h && h->hash) {
    return h->hash(self);
  }

  return hash_data(self, size(type_of(self)));
}
