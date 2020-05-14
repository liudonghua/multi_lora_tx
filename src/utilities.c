/** ****************************************************************************
 * @copyright                  Nationz Co.,Ltd
 *               Copyright (c) 2013��2018 All Rights Reserved
 *******************************************************************************
 * @file    utilities.c
 * @author   
 * @date     2017/10/15
 * @version  v1.0
 * @brief    
 *
 * @defgroup 
 * @{
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utilities.h"



#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

void srand1( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end


void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while( size-- )
    {
        *dst++ = value;
    }
}

int8_t Nibble2HexChar( uint8_t a )
{
    if( a < 10 )
    {
        return '0' + a;
    }
    else if( a < 16 )
    {
        return 'A' + ( a - 10 );
    }
    else
    {
        return '?';
    }
}

void threadsleep(int ms)
{
    struct timespec tv;
    /* Construct the timespec from the number of whole seconds... */
    tv.tv_sec = 0;
    /* ... and the remainder in nanoseconds. */
    tv.tv_nsec = (long) 1000*ms;

    /* Sleep for the time specified in tv. If interrupted by a
    signal, place the remaining time left to sleep back into tv. */
    int rval = nanosleep (&tv, &tv);

}
/** @} */

