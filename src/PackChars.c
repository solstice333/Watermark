/*
 ============================================================================
 Name        : BitOpsLab.c
 Author      : Kevin Navero
 Version     :
 Copyright   : 
 Description : Watermark
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define NUM_VALS 26
#define NUM_CHARS 11
#define MAX_DIFF 7
#define BITS_PER_CHAR 7
#define LSBS 3

void PrintHex(short *vals, char *msg) {
   int i;

   printf("PrintHex vals:\n");
   for (i = 0; i < NUM_VALS; i++) {
      printf("vals[%d]: 0x%08X\n", i, vals[i]); 
   }
   printf("\n\n");

   printf("PrintHex msg:\n");
   for (i = 0; i < NUM_CHARS; i++) {
      printf("msg[%d]: 0x%02X, %c\n", i, msg[i], msg[i]);
   }
   printf("\n\n");
}

// block 1
void EmbedWatermark(short *vals, char *msg) {
   int i;   
   unsigned int mask = LSBS | LSBS << 2;  // 0xF
   for (i = 0; i < NUM_CHARS; i++) {
      short flagIdx = i/LSBS + NUM_VALS - LSBS - 1;
      short flagPos = i%LSBS;

      // clear the value at the flag index and mapped position
      vals[flagIdx] &= ~(1 << flagPos);

      // handle right most nibble (least sig. nibble)
      char r_nibble = msg[i] & mask;

      if (r_nibble > MAX_DIFF) {
         r_nibble -= MAX_DIFF + 1;
         vals[flagIdx] |= 1 << flagPos;
      }

      vals[i*2] = (vals[i*2] & ~(mask >> 1)) | r_nibble;

      // handle left nibble (left of the least sig. nibble)
      char l_nibble = (msg[i] & mask << LSBS + 1) >> LSBS + 1;
      vals[i*2 + 1] = (vals[i*2 + 1] & ~(mask >> 1)) | l_nibble;
   }
}

// block 2
void PrintWatermark(short *copy) {
   int i;

   char extract;
   for (i = 0; i < NUM_VALS - LSBS - 1; i++) {
      short flagIdx = i/2/LSBS + NUM_VALS - LSBS - 1;
      short flagPos = i/2%LSBS;

      if (i % 2) {
         extract |= (copy[i] & BITS_PER_CHAR) << LSBS + 1;
         printf("%c", extract);
      }
      else {
         extract = 0;
         if (copy[flagIdx] >> flagPos & 1)
            extract = MAX_DIFF + 1;
         extract |= copy[i] & BITS_PER_CHAR;
      }
   }

   printf("\n");
}


/*
The main for this program reads the first 11 characters from input, saving them. It then reads 26 short integers into |vals|, and passes them and the characters to EmbedWatermark, which subtly alters the contents of vals to contain the characters. After EmbedWatermark returns, main checks that no value in |vals| has changed by more than 7. (Note this would be the case if the bottom three bits had been changed, but no other bits.)
The main then passes a copy of |vals| to PrintWatermark, which prints out the original characters by extracting them from |vals|, with no other data needed.
The point of the exercise is to elegantly pack the bits of the chars into the LSBs of the values, and then unpack them, as a means of learning bitwise operations. But, it also shows how subtle "watermarks" can be embedded in data via small adjustments to its LSBs. Image data, in particular, is suitable for this, and watermarks are sometimes used to identify different copies of an image for copyright purposes.
*/
int main() {
   short *vals = calloc(NUM_VALS, sizeof(short));
   char msg[NUM_CHARS];
   int i, diff;
   short copy[NUM_VALS];
   
   for (i = 0; i < NUM_CHARS; i++)
      msg[i] = getchar();
      
   for (i = 0; i < NUM_VALS; i++) {
      scanf("%hd", &vals[i]);
      copy[i] = vals[i];
   }
   
   EmbedWatermark(vals, msg);
   for (i = 0; i < NUM_CHARS; i++)
      msg[i] = 0;

   for (i = 0; i < NUM_VALS; i++) {
      diff = vals[i] - copy[i];
      if (diff > MAX_DIFF || diff < -MAX_DIFF)
         printf("Val %d differs too much\n", i);
      copy[i] = vals[i];
   }
   
   PrintWatermark(copy);
}