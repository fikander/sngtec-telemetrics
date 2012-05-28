#include "crc-count.h"

unsigned short init_rem = 0xFFFF;
unsigned short final_xor = 0x0000;
unsigned short byte_size = 8 * sizeof(short);
unsigned short topbit = 1 << (byte_size - 1);
unsigned short some_pol = 0x8005;


unsigned long reflect(unsigned long data, unsigned char bitsNumber) {
        unsigned long  reflection = 0x00000000;
        unsigned char  bit;

        for (bit = 0; bit < bitsNumber; ++bit) {
                if (data & 0x01){
                        reflection |= (1 << ((bitsNumber - 1) - bit));
                }
                data = (data >> 1);
        }
        return (reflection);
}



unsigned short modbusCRC(unsigned char* message, int bytesNumber) {
    unsigned short crc_rem = init_rem;

    for (int byte = 0; byte < bytesNumber; ++byte) {
        crc_rem ^= (reflect(message[byte], 8) << (byte_size - 8));
        for (int bit = 8; bit > 0; --bit) {
            if (crc_rem & topbit){
                crc_rem = (crc_rem << 1) ^ some_pol;
            } else {
                crc_rem = (crc_rem << 1);
            }
        }
    }
    return (reflect(crc_rem, byte_size) ^ final_xor);
}

