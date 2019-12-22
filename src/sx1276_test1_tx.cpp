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
#include "misc.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <stdlib.h>  

using boost::shared_ptr;
using boost::format;
using std::cout;

inline std::string safe_str(const char *m)
{
  std::string result;
  for (; *m; ) { char c = *m++; result += iscntrl(c) ? '.' : c; }
  return result;
}

void usage(void) {
  printf("-d          spi device path for node(/dev/spidev0.0)\n");
  printf("-f          LoRa Frequency(430000000-928000000\n");
  printf("-s          LoRa spread factor(7~12)\n");
  printf("-t          interval time between two transmit\n");
}

int main(int argc, char* argv[])
{
  //if (argc < 3) { fprintf(stderr, "Usage: %s <spidev> <frequency>", argv[0]); return 1; }
  /* parse command line options */

    int i;
    int xd=0;
    uint32_t xl=0;
    uint32_t freq = 0;
    uint32_t freq_hz;
    uint32_t if_hz;
    int sf = 0;
    int sff = 0;
    int timeout = 0;
    char device[10];
    int sfs[25] = {7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,9,9,9,9,10,10,11,12};
     while ((i = getopt (argc, argv, "hf:d:s:t:")) != -1){
      switch(i){
            case 'h':
                usage();
                return EXIT_FAILURE;
                break;

            case 'f': /* <float> Target frequency in MHz */
                i = sscanf(optarg, "%li", &xl);
                if ((i != 1) || (xl < 430000000) || (xl > 928000000)) {
                    printf("ERROR: invalid TX frequency\n");
                    usage();
                    return EXIT_FAILURE;
                } else {
                    freq = xl; 
                }
                break;

             case 'd': 
                i = sscanf(optarg, "%s", device);
                if (i != 1) {
                    printf("ERROR: invalid spi device\n");
                    usage();
                    return EXIT_FAILURE;
                } 
                break;

            case 's': 
                i = sscanf(optarg, "%d", &sf);
                if ((i != 1) || (sf < 7) || (sf > 12))
                  sf = 0;
                break;
            
            case 't':
                i = sscanf(optarg, "%d", &timeout);
                if (i!=1)
                  timeout = 0;
                break;

            default:
                printf("ERROR: argument parsing\n");
                usage();
                return EXIT_FAILURE;
        }
    }                
  shared_ptr<SX1276Platform> platform = SX1276Platform::GetInstance(device);
  if (!platform) { PR_ERROR("Unable to create platform instance. Note, use /dev/tty... for BusPirate, otherwise, /dev/spidevX.Y\n"); return 1; }

  shared_ptr<SPI> spi = platform->GetSPI();
  if (!spi) { PR_ERROR("Unable to get SPI instance\n"); return 1; }

  // Pass a small value in for RTL-SDK spectrum analyser to show up
  unsigned inter_msg_delay_us = 20000;
  if (getenv("BEACON_INTERVAL")) { inter_msg_delay_us = atoi(getenv("BEACON_INTERVAL")); }

  Misc::UserTraceSettings(spi);

  // TODO work out how to run without powering off / resetting the module

  usleep(100);
  SX1276Radio radio(spi);

  cout << format("SX1276 Version: %.2x\n") % radio.version();

  platform->ResetSX1276();

  
  radio.ChangeCarrier(freq);
  radio.ApplyDefaultLoraConfiguration(9);
  cout << format("Check read Carrier Frequency: %uHz\n") % radio.carrier();

  if (radio.fault()) return 1;

  char msg[128]="TX start now!";
  printf("Beacon message: '%s'\n", safe_str(msg).c_str());
  printf("Predicted time on air: %fs\n", radio.PredictTimeOnAir(msg));

  int total = 0;
  int faultCount = 0;
  while (true) {
    total++;

    time_t rawt;
    struct tm *ti;
    char buft[80];
    time(&rawt);
    ti = localtime(&rawt);
    strftime(buft,80,"%d-%m-%Y %I:%M:%S", ti);
    snprintf(msg, sizeof(msg), "TX BEACON %6d %s\n", total, buft);
	
    if (radio.SendSimpleMessage(msg)) { printf("%d ", total); fflush(stdout); radio.Standby(); usleep(timeout); }
    radio.Standby();
    printf("\n");
    faultCount++;
    //PR_ERROR("Fault on send detected: %d of %d\n", faultCount, total);
    printf("Beacon message: '%s'\n", safe_str(msg).c_str());
    printf("Predicted time on air: %fs\n", radio.PredictTimeOnAir(msg));
    radio.reset_fault();
    platform->ResetSX1276();
    
    srand((uint16_t)time(NULL));
    if_hz  =  200000 * (rand() % 8);
    freq_hz = freq + if_hz;
    radio.ChangeCarrier(freq_hz);

    printf("spread factor origin:%d \n",sf);
    if(sf == 0){
	    srand((uint16_t)time(NULL));
	    sff =  sfs[rand() % 25];
    }else{
	    sff = sf;
    }
    	
    radio.ApplyDefaultLoraConfiguration(sff);
    printf("spread factor:%d frequency:%d \n",sff,freq_hz);
     cout << format("Check read Carrier Frequency: %uHz\n") % radio.carrier();
    usleep(timeout);
  }
  return 1;
}

