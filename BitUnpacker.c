/*
 * Copyright Software Innovations
 */

#include <stdio.h>
#include <string.h>

#define UINT_SIZE 32
#define UINT_MASK 0xFFFFFFFF

typedef unsigned int UInt;

typedef struct {
   UInt curData;
   UInt nextData;
   int bitsLeft;
   int validNext;
} BitUnpacker;

void BuInit(BitUnpacker *bu) {
   bu->curData = 0;
   bu->nextData = 0;
   bu->bitsLeft = 0;
   bu->validNext = 0;
}                              

void BuTakeData(BitUnpacker *bu, UInt next) {
   bu->nextData = next;
   bu->validNext = 1;
}                                             

int BuUnpack(BitUnpacker *bu, int size, UInt *out) {
   int ret = 0, check = bu->bitsLeft - size, shift = UINT_SIZE - bu->bitsLeft;
   *out = (unsigned int)(UINT_MASK << shift) >> shift;
   if (bu->bitsLeft) {
      *out &= bu->curData;
      if (check >= 0) {
         *out >>= check;
         bu->bitsLeft -= size;
         ret = 1;
      }                                                                        
      else {                                             
         size -= bu->bitsLeft;
         *out <<= size;
                                                                               
         if (bu->validNext) {
            bu->curData = bu->nextData;
            bu->validNext = 0;
                                                                               
            *out |= (unsigned int)(bu->curData >> (UINT_SIZE - size));
                                                                               
            bu->bitsLeft = UINT_SIZE - size;
            ret = 1;
         }                                                                     
      }                                                                        
   }                                                                           
   else {
      if (bu->validNext) {                       
         bu->curData = bu->nextData;
         bu->validNext = 0;
                                                                               
         *out &= bu->curData;
         *out >>= check;
                                                                               
         bu->bitsLeft = UINT_SIZE - size;
         ret = 1;
      }                                                                        
   }                                                                           
   return ret;
}                                                                              
