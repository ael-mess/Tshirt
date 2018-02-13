#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"

#define MAC_SIZE	6
#define IPV4_SIZE	4

int main(void)
{
    init_serial(9600);
    ad_init(0x01);
    while(1) 
    {
        int sm = ad_sample();
        send_serial((sm/100)+48);
        send_serial((sm/10-(sm/100)*10)+48);
        send_serial((sm-((sm/10-(sm/100)*10))*10-(sm/100)*100)+48);
        send_serial(' ');
    }
    return 0;
}
