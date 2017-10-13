#include <stdio.h>
#include "LZWExp.h"
#include "CodeSet.h"
#include "SmartAlloc.h"

#define EOD_CODE 256
#define START_NUM_BITS 9

/* Initialize a LZWExp the DataSink to which to send uncompressed symbol 
 * sequences and an anonymous state object for the DataSink.
 */
void LZWExpInit(LZWExp *exp, DataSink sink, void *sinkState, int recycleCode) {

   int i;

   exp->dict = CreateCodeSet(DEFAULT_RECYCLE_CODE);

   for (i = 0; i <= EOD_CODE; i++) {
      NewCode(exp->dict, i);
   }

   exp->sink = sink;
   exp->sinkState = sinkState;
   exp->lastCode = EOD_CODE;
   exp->numBits = START_NUM_BITS;
   exp->maxCode = (1 << START_NUM_BITS) - 1;
   exp->recycleCode = recycleCode;
   BuInit(&(exp->bitUnpacker));
   exp->EODSeen = 0;
}

/* Break apart compressed data in "bits" into one or more codes and send 
 * the corresponding symbol sequences to the DataSink.  Save any leftover 
 * compressed bits to combine with the bits from the next call of 
 * LZWExpEncode.  Return 0 on success or BAD_CODE if you receive a code not
 * in the dictionary.
 *
 * For this and all other methods, a code is "invalid" if it either could not
 * have been sent (e.g. is too high) or if it is a nonzero code following
 * the detection of an EOD code.
 */
int LZWExpDecode(LZWExp *exp, UInt bits) {
   UInt ret = 0;
   static UInt prev = 0;
   int result = 0, flag = 1;
   Code myCode;
   BitUnpacker *bup = &(exp->bitUnpacker);

   BuTakeData(bup, bits);
   while (BuUnpack(bup, exp->numBits, &ret) && flag) {
      if (ret > exp->lastCode || exp->EODSeen) {
         flag = 0;
         result = BAD_CODE;
      }
      else {
         if (exp->lastCode == exp->maxCode) {
            exp->numBits += 1;
            exp->maxCode = (1 << (exp->numBits)) - 1;
         }

         exp->lastCode = ExtendCode(exp->dict, ret);
         myCode = GetCode(exp->dict, ret);
         if (prev) {
            SetSuffix(exp->dict, exp->lastCode - 1, *myCode.data);
         }

         prev = 1;
         if (ret != EOD_CODE) {
            (exp->sink)(NULL, myCode.data, myCode.size);
         }
         else {
            exp->EODSeen = 1;
         }
         FreeCode(exp->dict, ret);
      }
   }
   return result;
}

int LZWExpStop(LZWExp *exp) {
   int ret = 0;

   if (!(exp->EODSeen)) {
      ret = MISSING_EOD;
   }
   return ret;
}

void LZWExpDestruct(LZWExp *exp) {
   DestroyCodeSet(exp->dict);
}
