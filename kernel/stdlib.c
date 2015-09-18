
#include <kernel.h>


int k_strlen(const char* str)
{	int x = 0;
	while(str[x] != '\0'){
		x++;
	}
	return x;
}

void* k_memcpy(void* dst, const void* src, int len)
{	unsigned char *dst2 = dst;
	const unsigned char *src2 = src;
	while(len--){
		*dst2 = *src2;
		dst2++;
		src2++;
	}
	return dst;
}

int k_memcmp(const void* b1, const void* b2, int len){	
	const unsigned char *t1 = b1, *t2 = b2;
    while(len--){
        if( *t1 != *t2 ){
            return *t1 - *t2;
        }
        t1++;
        t2++;
    }
    return 0;
}