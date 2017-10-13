#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "SmartAlloc.h"
#include "CodeSet.h"

typedef struct CodeEntry {
   struct CodeEntry *pre;
   unsigned char *code;
   unsigned char final;
   unsigned int size;
   int has;
} CodeEntry;

typedef struct CodeSet {
   CodeEntry *myCodes;
   int current;
} CodeSet;

void *CreateCodeSet(int numCodes) {
   CodeSet *cs = calloc(1, sizeof(CodeSet));
   cs->myCodes = calloc(numCodes, sizeof(CodeEntry));
   cs->current = 0;
   return cs;
}

int NewCode(void *codeSet, char val) {
   CodeSet *cs = codeSet;
   CodeEntry *entry = cs->myCodes;
   int cur = cs->current;

   cs->current = cur + 1;
   entry += cur;

   entry->pre = NULL;
   entry->final = val;
   entry->size = 1;
   entry->has = 0;
   return cur;
}

int ExtendCode(void *codeSet, int oldCode) {
   CodeSet *cs = codeSet;
   int cur = cs->current;
   cs->current = cur + 1;

   CodeEntry *entry = cs->myCodes;
   CodeEntry *new = cs->myCodes;
   entry += oldCode;
   new += cur;

   new->pre = entry;
   new->final = 0U;
   new->size = entry->size + 1;
   new->has = 0;
   return cur;
}

void SetSuffix(void *codeSet, int code, char suffix) {
   CodeSet *cs = codeSet;
   CodeEntry *entry = cs->myCodes;

   entry += code;
   entry->final = suffix;
}

Code GetCode(void *codeSet, int code) {
   CodeSet *cs = codeSet;
   CodeEntry *ce = cs->myCodes + code;
   CodeEntry *temp = ce;
   int size = ce->size - 1;
   unsigned char *data = malloc(sizeof(char) * ce->size);

   while (size) {
      *(data + size--) = temp->final;
      temp = temp->pre;
   }
   *data = temp->final;
   
   Code entry = { data, ce->size };
   ce->code = data;
   ce->has = 1;
   return entry;
}

void FreeCode(void *codeSet, int code) {
   CodeSet *cs = codeSet;
   CodeEntry *entry = cs->myCodes;

   entry += code;
   if (entry->has) {
      free(entry->code);
   }
   entry->has = 0;
}

void DestroyCodeSet(void *codeSet) {
   CodeSet *cs = codeSet;

   CodeEntry *ce = cs->myCodes;
   int num = cs->current;
   while (num--) {
      CodeEntry entry = *ce++;
      if (entry.has) {
         free(entry.code);
      }
   }
   free(cs->myCodes);
   free(cs);
}
