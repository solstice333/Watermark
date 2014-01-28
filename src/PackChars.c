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

// convenience function for printing - ignore
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
   int i = 0;
   while (i < NUM_CHARS) {
      short flagIdx = i/LSBS + NUM_CHARS*2, flagPos = 1 << i%LSBS, 
       insertIdx = i*2;

      // clear bits to set
      vals[flagIdx] &= ~flagPos;
      vals[insertIdx] &= ~BITS_PER_CHAR;
      vals[insertIdx + 1] &= ~BITS_PER_CHAR;

      // handle least sig. nibble (max range = 0-F)
      char r_nibble = msg[i] & NUM_VALS - NUM_CHARS; 
      if (r_nibble > MAX_DIFF) { 
         r_nibble -= MAX_DIFF + 1; 
         vals[flagIdx] |= flagPos; 
      }
      vals[insertIdx] |= r_nibble; 

      // handle left nibble (max range = 3-6) 
      vals[insertIdx + 1] |= msg[i++] >> LSBS + 1 & BITS_PER_CHAR; 
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
