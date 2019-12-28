#ifndef SX1276_TX_HPP__

#define SX1276_TX_HPP__

#include <stdlib.h> 
#include <cstring>
#include <thread> 


class SX1276Tx
{
public:
	char device[10];
	uint64_t *freqs;
	uint8_t freq_size;
	uint8_t *sfs;
	uint8_t sf_size;
	uint16_t timeout;
	uint8_t plen;
	uint16_t maxaddr;
	uint32_t max_count;
    void threadtx();
};

#endif