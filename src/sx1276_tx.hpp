#ifndef SX1276_TX_HPP__

#define SX1276_TX_HPP__

#include <stdlib.h> 
#include <cstring>
#include <thread> 
#include <boost/noncopyable.hpp>
#include <json/json.h>

class SX1276Tx : boost::noncopyable
{

public:
	SX1276Tx(Json::Value conf);
	~SX1276Tx();
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
	std::thread thd(){
		return std::thread(&SX1276Tx::threadtx,this);
	}
private:


	void threadtx();
};

#endif