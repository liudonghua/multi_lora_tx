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

using boost::shared_ptr;
using boost::format;
using std::cout;

inline std::string safe_str(const char *m)
{
  std::string result;
  for (; *m; ) { char c = *m++; result += iscntrl(c) ? '.' : c; }
  return result;
}

int main(int argc, char* argv[])
{
  if (argc < 2) { fprintf(stderr, "Usage: %s <spidev>", argv[0]); return 1; }

  shared_ptr<SX1276Platform> platform = SX1276Platform::GetInstance(argv[1]);
  if (!platform) { PR_ERROR("Unable to create platform instance. Note, use /dev/tty... for BusPirate, otherwise, /dev/spidevX.Y\n"); return 1; }

  shared_ptr<SPI> spi = platform->GetSPI();
  if (!spi) { PR_ERROR("Unable to get SPI instance\n"); return 1; }

  // Pass a small value in for RTL-SDK spectrum analyser to show up
  unsigned inter_msg_delay_us = 200000;
  if (getenv("BEACON_INTERVAL")) { inter_msg_delay_us = atoi(getenv("BEACON_INTERVAL")); }

  Misc::UserTraceSettings(spi);

  // TODO work out how to run without powering off / resetting the module

  usleep(100);
  SX1276Radio radio(spi);

  cout << format("SX1276 Version: %.2x\n") % radio.version();

  platform->ResetSX1276();

  radio.ChangeCarrier(919000000);
  radio.ApplyDefaultLoraConfiguration();
  cout << format("Check read Carrier Frequency: %uHz\n") % radio.carrier();

  if (radio.fault()) return 1;

  char msg[128];
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
	
    if (radio.SendSimpleMessage(msg)) { printf("%d ", total); fflush(stdout); radio.Standby(); usleep(inter_msg_delay_us); continue; }
    radio.Standby();
    printf("\n");
    faultCount++;
    PR_ERROR("Fault on send detected: %d of %d\n", faultCount, total);
    printf("Beacon message: '%s'\n", safe_str(msg).c_str());
    printf("Predicted time on air: %fs\n", radio.PredictTimeOnAir(msg));
    radio.reset_fault();
    platform->ResetSX1276();
    radio.ChangeCarrier(919000000);
    radio.ApplyDefaultLoraConfiguration();
    usleep(500000);
  }
  return 1;
}
