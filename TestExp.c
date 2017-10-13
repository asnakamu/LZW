#include <stdio.h>
#include "SmartAlloc.h"
#include "LZWExp.h"
#include "CodeSet.h"

int main() {
   unsigned int input = 0;
   LZWExp *lzw = calloc(sizeof(LZWExp), 1);
   BitUnpacker *bu;
   DataSink ds;
   Code myCode;
   UInt ret = 0, prev = 0;

   LZWExpInit(lzw, ds, NULL, DEFAULT_RECYCLE_CODE);
   bu = &(lzw->bitUnpacker);

   while (scanf("%08X", &input) != EOF) {
      BuTakeData(bu, input);

      while (BuUnpack(bu, lzw->numBits, &ret)) {
         lzw->lastCode = ExtendCode(lzw->dict, ret);
         myCode = GetCode(lzw->dict, ret);
         if (prev) {
            SetSuffix(lzw->dict, lzw->lastCode - 1, *myCode.data);
         }
         prev = 1;
         printf("%.*s", myCode.size, myCode.data);
      }
   }

   DestroyCodeSet(lzw->dict);

   return 0;
}

