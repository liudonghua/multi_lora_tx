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
#include "sx1276_platform.hpp"
#include "buspirate_spi.hpp"
#include "buspirate_binary.h"
#include "spidev_spi.hpp"
#include "spi.hpp"
#include <string.h>

#ifdef HAVE_MY_UGPIO
#include <ugpio/ugpio.h>
#else
typedef void ugpio_t;
#endif

using boost::shared_ptr;

class BusPiratePlatform : public SX1276Platform
{
public:
  BusPiratePlatform(const char *device)
  : device_(device)
  {
    printf("Platform:BusPirate\n");
    spi_.reset(new BusPirateSPI);
    spi_->Open(device);
  }
  virtual ~BusPiratePlatform() {}

  virtual bool PowerSX1276(bool powered) { return powered ? bp_power_off(spi_->fd_) : bp_power_on(spi_->fd_); }
  virtual bool PowerCycleSX1276(bool powered) { return bp_power_cycle(spi_->fd_); }
  virtual bool ResetSX1276() { return bp_cycle_reset(spi_->fd_); }

  virtual boost::shared_ptr<SPI> GetSPI() const { return spi_; }

private:
  std::string device_;
  shared_ptr<BusPirateSPI> spi_;
};

class Carambola2Platform : public SX1276Platform
{
public:
  Carambola2Platform(const char *device)
  : device_(device), rst_gpio_(18), rst_gp_(NULL)
  {
    printf("Platform:Linux spidev\n");
    spi_.reset(new SpidevSPI);
    spi_->Open(device);
#ifdef HAVE_UGPIO
    rst_gp_ = ugpio_request_one(rst_gpio_, GPIOF_OUT_INIT_HIGH, "");
    if (!rst_gp_) {
      fprintf(stderr, "Unable to request GPIO %d for SX1276 RST\n", rst_gpio_);
    }
    int fd = ugpio_open(rst_gp_);
#endif
  }
  virtual ~Carambola2Platform() {
#ifdef HAVE_UGPIO
    if (rst_gp_) {
      ugpio_free(rst_gp_);
    }
#endif
  }

  virtual boost::shared_ptr<SPI> GetSPI() const { return spi_; }

  // FIXME : use GPIO to control power / reset
  virtual bool PowerSX1276(bool powered) { return true; }
  virtual bool PowerCycleSX1276(bool powered)  { return true; }
  virtual bool ResetSX1276()  {
#ifdef HAVE_UGPIO
    ugpio_set_value(rst_gp_, 0);
    usleep(1000);
    ugpio_set_value(rst_gp_, 1);
#endif
    return true;
  }

private:
  std::string device_;
  int rst_gpio_;
  ugpio_t* rst_gp_;
  shared_ptr<SpidevSPI> spi_;
};

shared_ptr<SX1276Platform> SX1276Platform::GetInstance(const char *device)
{
  shared_ptr<SX1276Platform> platform;
  // For the time being, use a simple heuristic:
  // if not /dev/spidev then tty for buspirate
  const char *PFX_SPIDEV = "/dev/spidev";
  if (strncmp(device, PFX_SPIDEV, strlen(PFX_SPIDEV))==0) {
    platform.reset(new Carambola2Platform(device));
  } else {
    platform.reset(new BusPiratePlatform(device));
  }
  if (platform->GetSPI()->IsOpen()) { return platform; }
  return shared_ptr<SX1276Platform>();
}

SX1276Platform::SX1276Platform()
{
}

SX1276Platform::~SX1276Platform()
{
}
