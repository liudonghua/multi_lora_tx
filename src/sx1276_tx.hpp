#ifndef SX1276_TX_HPP__

#define SX1276_TX_HPP__
#include <thread> 
#include <stdlib.h> 
#include <cstring>
#include <json/json.h>

#include <json/json.h>


class SX1276Tx
{

public:
	SX1276Tx(Json::Value conf);
	~SX1276Tx();
	
	void 		threadtx();
private:
	char		device[15];
	uint32_t	*freqs;
	uint8_t		freq_size;
	uint8_t		*sfs;
	uint8_t		sf_size;
	uint8_t		txpow;
	uint16_t	timeout;
	uint8_t		plen;
	uint16_t	maxaddr;
	uint32_t	max_count;
	
	
};

#endif