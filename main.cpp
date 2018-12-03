#include <iostream>

using namespace std;

#define NUM_BITS (8 * sizeof(unsigned int))

const unsigned int data_length = 64;

//const unsigned int THREADS = 256;
const unsigned int FILTER_LENGTH = 3;
const unsigned int NUM_FILTERS = 2;
const unsigned int FILTERS = (0b111 << FILTER_LENGTH) | 0b011;

char nums[2] = { '0', '1' };

#define dataArrayLength ((data_length + NUM_BITS - 1) / NUM_BITS)
#define resultArrayLength ((data_length * NUM_FILTERS + NUM_BITS - 1) / NUM_BITS)

void printBitString(const unsigned int *data, int length);


// This function is specifically structured to mimic the execution of a CUDA kernel,
// which is why the entire operation is contained in two for loops.
// The variables "start" and "idx" will be derived from the block index and thread index, respectively.
void convEncode(const unsigned int *data, unsigned int *output) {
    for (unsigned int start = 0; start < (data_length + NUM_BITS - 1) / (NUM_BITS * 2); start++)   {
        for (unsigned int idx = 0; idx < 2 * NUM_BITS; idx++)  {

            unsigned int val = (((idx / NUM_BITS) | start
                        ? data[idx / NUM_BITS + start - 1] << (1 + (idx % NUM_BITS)) : 0)
                                | (data[idx / NUM_BITS + start] >> (NUM_BITS - 1 - (idx % NUM_BITS))))
                                        & ((1 << FILTER_LENGTH) - 1 );

            #pragma unroll
            for (unsigned int i = 0; i < NUM_FILTERS; i++) {

                unsigned int n = val & ((FILTERS >> ((NUM_FILTERS - i - 1) * FILTER_LENGTH))
                        & ((1 << FILTER_LENGTH) - 1 ));

                n = n - ((n >> 1) & 0x55555555);
                n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
                unsigned int newVal = (((((n + (n >> 4) & 0x0F0F0F0F) * 0x01010101) >> 24) & 1));

                output[NUM_FILTERS * start + (NUM_FILTERS * idx) / NUM_BITS] |=
                        (newVal << ((NUM_BITS * NUM_FILTERS - 1 - (NUM_FILTERS * (idx % NUM_BITS) + i))));
            }
        }
    }
}

void printBitString(const unsigned int *data, int length) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < NUM_BITS; j++) {
            cout << nums[(data[i] >> (NUM_BITS - 1 - j)) & 1];
        }
    }
    cout << endl;
}


int main() {
    unsigned int input[dataArrayLength] = {0xFFFFFFFF, 0xFFFFFFFF};
    unsigned int output[resultArrayLength];

    for (unsigned int &i : output) { i = 0; }

    convEncode(input, output);
    printf("%x\n%x\n", output[0], output[1]);
    printBitString(output, resultArrayLength);
}
