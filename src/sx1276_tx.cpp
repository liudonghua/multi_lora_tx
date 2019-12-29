/*
	Copyright (c) 2016 Andrew McDonnell <bugs@andrewmcdonnell.net>

	This file is part of the simple sx1276 Linux library.

	SentriFarm Radio Relay is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	SentriFarm Radio Relay is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with SentriFarm Radio Relay.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "sx1276.hpp"
#include "sx1276_platform.hpp"
#include "LoRaMacCrypto.h"
#include "misc.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <stdlib.h> 

#include <cstring>
#include "sx1276_tx.hpp"

#define     PREME   0x40
#define     FCNTL   0x00
#define     FPORT   0x02

#define    MAX_NODES	8

using boost::shared_ptr;
using boost::format;
using std::cout;

#if 1
#define   DBG 	printf
#else
#define DBG
#endif


inline std::string safe_str(const char *m)
{
	std::string result;
	for (; *m; ) { char c = *m++; result += iscntrl(c) ? '.' : c; }
	return result;
}

void SX1276Tx::threadtx()
{


	int i;
	int xd=0;
	int sf=9;
	uint32_t xl=0;
	uint32_t freq_hz;


	shared_ptr<SX1276Platform> platform = SX1276Platform::GetInstance(device);
	if (!platform) { PR_ERROR("Unable to create platform instance. Note, use /dev/tty... for BusPirate, otherwise, /dev/spidevX.Y\n"); return; }
	shared_ptr<SPI> spi = platform->GetSPI();

	if (!spi) { PR_ERROR("Unable to get SPI instance\n"); return; }
	unsigned inter_msg_delay_us = 20000;

	//if (getenv("BEACON_INTERVAL")) { inter_msg_delay_us = atoi(getenv("BEACON_INTERVAL")); }
	// Pass a small value in for RTL-SDK spectrum analyser to show up
	if(timeout) inter_msg_delay_us = 1000*timeout;

	Misc::UserTraceSettings(spi);

	// TODO work out how to run without powering off / resetting the module

	usleep(100);
	SX1276Radio radio(spi);

	cout << format("SX1276 Version: %.2x\n") % radio.version();

	platform->ResetSX1276();

	
	radio.ChangeCarrier(freqs[0]);
	radio.ApplyDefaultLoraConfiguration(sf,txpow);
	cout << format("Check read Carrier Frequency: %uHz\n") % radio.carrier();

	if (radio.fault()) return;

	char msg[240]="";
	printf("Beacon message: '%s'\n", safe_str(msg).c_str());
	printf("Predicted time on air: %fs\n", radio.PredictTimeOnAir(msg));

	uint16_t total = 1;
	uint32_t devaddr = 0 ;
	int faultCount = 0;
	while (true) {
		devaddr++;
		if (devaddr > maxaddr){
			total++;
			devaddr = 0;

		}
		
		time_t rawt;
		struct tm *ti;
		char enbuffer[255];

		uint8_t preme = PREME;

		uint8_t fcntl = FCNTL;

		uint8_t fport = FPORT;

		snprintf(msg, plen, "1abcdefghijklmnopqrstuvwxyzABCDEFGIJKLMNOPQRSTUVWXYZabcdefgijklmnopqrstuvwxyz01234567890abcdefghijklmnopqrstuvwxyzABCDEFGIJKLMNOPQRSTUVWXYZabcdefgijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyzABCDEFGIJKLMNOPQRSTUVWXYZabcdefgijklmnopqrstuvwxyz");
		
		memcpy((uint8_t *)enbuffer,(uint8_t *)&preme,1);
		//baddr = (((devaddr>>24) & 0x000000ff) | ((devaddr>>8) & 0x0000ff00) | ((devaddr<<8) & 0x00ff0000) | ((devaddr<<24) & 0xff000000));

		memcpy((uint32_t *)(enbuffer+1),(uint32_t *)&devaddr,4);

		memcpy((uint8_t *)(enbuffer+5),(uint8_t *)&fcntl,1);

		//uint16_t count = (cycle_count<<8 & 0xff00)|(cycle_count>>8 & 0x00ff);
		memcpy((uint16_t *)(enbuffer+6),(uint16_t *)&total,2);

		memcpy((uint8_t *)(enbuffer+8),(uint8_t *)&fport,1);

		memcpy((uint8_t *)(enbuffer+9),msg,plen);

		uint32_t mic  = 0;

		const uint8_t appskey[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		LoRaMacComputeMic((uint8_t *)enbuffer,uint16_t(plen+9),appskey,devaddr,0,total,&mic);

		memcpy((uint32_t *)(enbuffer+plen+9),&mic,4);
		
		if (radio.SendSimpleMessage(enbuffer,plen+13)) { printf("%d ", total); fflush(stdout); radio.Standby(); usleep(inter_msg_delay_us); }
		radio.Standby();
		printf("\n");
		faultCount++;
		//PR_ERROR("Fault on send detected: %d of %d\n", faultCount, total);
		printf("Beacon message: '%s'\n", safe_str(msg).c_str());
		printf("Predicted time on air: %fs\n", radio.PredictTimeOnAir(msg));
		radio.reset_fault();
		platform->ResetSX1276();
		
		srand((uint16_t)time(NULL));
		if (freq_size > 1)
			freq_hz = freqs[rand()%freq_size];
		else
			freq_hz = freqs[0];
		

		radio.ChangeCarrier(freq_hz);

		

		if (sf_size>1)
			sf = sfs[rand()%sf_size];
		else
			sf = sfs[0];
		
		printf("spread factor origin:%d \n",sf);

		radio.ApplyDefaultLoraConfiguration(sf,txpow);
		printf("spread factor:%d frequency:%d \n",sf,freq_hz);
		cout << format("Check read Carrier Frequency: %uHz\n") % radio.carrier();
		usleep(inter_msg_delay_us);
	}
}