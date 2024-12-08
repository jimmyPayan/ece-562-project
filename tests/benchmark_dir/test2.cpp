#include <string>
#include <iostream>
#include <stdint.h>
#include <stdio.h>

int main(){
	int64_t intData[8];
	
	// debug
	std::cout << "IntData = 0x";
	for(int i = 0 ; i < 8 ; i++) {
		intData[i] = 0x9 - i;
		printf("%016llx", (long long)intData[i]);
	}
	printf("\n");

	void* data = &intData;
    int64_t* wordData = (int64_t*)data;
	
	int8_t v[7];
	
	int64_t compressedData[2];
	
	compressedData[0] = wordData[0];
	compressedData[1] = 0;
	
	for (int i = 0 ; i < 7 ; i++) {
		v[i] = wordData[0] - wordData[i+1];	
		compressedData[1] = (compressedData[1] << 8 | v[i]);
	}
	compressedData[1] = compressedData[1] << 8 | 0x00;
	
	printf("Base + Deltas: ");
	
	for (int i = 0 ; i < 2 ; i++) {
		printf( "%016llx", (long long)compressedData[i]);
	}
	printf("\n");
	
	return 0;
}
/*
test2.cpp results:
IntData = 0x 0000000000000009 0000000000000008 0000000000000007 0000000000000006 
			 0000000000000005 0000000000000004 0000000000000003 0000000000000002
			 
Base: 0000000000000009 Deltas: 0102030405060700

Note: This does not point to the correct spots in memory.
compression_test.cpp results:
IntData = 0x 0000000000000009 0000000000000008 0000000000000007 0000000000000006 
		     0000000000000005 0000000000000004 0000000000000003 0000000000000002
			 
Base: 0000000000000009 Deltas: 0102030405060700
*/