#include "crc-count.h"

unsigned short polpol = 0x8005;
unsigned short init_rem = 0xFFFF;
unsigned short final_xor = 0x0000;
unsigned short width = 8 * sizeof(short);
unsigned short topbit = 1 << (width - 1);


static unsigned long reflect(unsigned long data, unsigned char nBits) {
        unsigned long  reflection = 0x00000000;
        unsigned char  bit;

        for (bit = 0; bit < nBits; ++bit) {
                if (data & 0x01){
                        reflection |= (1 << ((nBits - 1) - bit));
                }
                data = (data >> 1);
        }
        return (reflection);
}



unsigned short modbusCRC(unsigned char const message[], int nBytes){
    unsigned short remainder = init_rem;
    int            byte;
    unsigned char  bit;

    for (byte = 0; byte < nBytes; ++byte) {
        remainder ^= (reflect(message[byte], 8) << (width - 8));
        for (bit = 8; bit > 0; --bit) {
            if (remainder & topbit){
                remainder = (remainder << 1) ^ polpol;
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    return (reflect(remainder, width) ^ final_xor);
}

