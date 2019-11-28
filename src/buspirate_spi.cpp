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
#include "buspirate_spi.hpp"
#include "buspirate_binary.h"
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

// TODO: abstract fprintf away

BusPirateSPI::BusPirateSPI()
{
}

BusPirateSPI::~BusPirateSPI()
{
  if (fd_ > -1) {
    close(fd_);
  }
}

bool BusPirateSPI::Open(const char *ttydev)
{
  assert(fd_ < 0);

  int fd = open(ttydev, O_RDWR | O_NOCTTY);
  if (fd == -1) { perror("Unable to open device"); return false; }

  fd_ = fd;
  if (ConfigSerial() && EnableBinaryMode() && Powerup() && ConfigSPI()) {
    ttydev_ = ttydev;
    return true;
  }
  close(fd);
  fd_ = -1;
  return false;
}

bool BusPirateSPI::ConfigSerial()
{
  return bp_setup_serial(fd_, B115200);
}

bool BusPirateSPI::EnableBinaryMode()
{
  printf("Connecting to BusPirate...\n");
  bool ok = bp_enable_binary_spi_mode(fd_);
  if (ok) { printf("OK.\n"); } else { printf("Error.\n"); }
  return ok;
}

// Presently this function is hard coded to support inAir9 SPI. Config should perhaps be an arg.
bool BusPirateSPI::ConfigSPI()
{
  return bp_spi_config(fd_);
}

bool BusPirateSPI::Powerup()
{
  return bp_power_on(fd_);
}

bool BusPirateSPI::ReadRegister(uint8_t reg, uint8_t& result)
{
  usleep(100);
  bool ok = bp_bitbang_spi_read_one(fd_, reg, &result);
  if (trace_reads_) { fprintf(stderr, "[R] %.2x --> %.2x\n", (int)reg, (int)result); }
  return ok;
}

bool BusPirateSPI::WriteRegister(uint8_t reg, uint8_t value)
{
  if (trace_writes_) { fprintf(stderr, "[W] %.2x <-- %.2x\n", (int)reg, (int)value); }
  usleep(100);
  return bp_bitbang_spi_write_one(fd_, reg | 0x80, value);
}
