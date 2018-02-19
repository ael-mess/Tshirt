#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "serial.h"
#include "analog.h"

#define MAC_SIZE	6
#define IPV4_SIZE	4

typedef struct TShirt {
    uint8_t interr, boutton, nom[32];
    float accelx, accely, accelz, temp;
} TShirt;

char hip[20] = {0x45,0x00,,,0x00,0x00,0x40,0x00,0x40,0x11,,,0xAC,0x1A,0x4F,0xCB,0xAC,0x1A,0x4F,0xCA};
char hudp[8] = {,,,,,,,};

int main(void)
{
    init_serial(9600);
    while(1) 
    {
        ad_init(0x0);
        int sm = ad_sample();
        send_serial('x');
        send_serial((sm/100)+48);
        send_serial((sm/10-(sm/100)*10)+48);
        send_serial((sm-((sm/10-(sm/100)*10))*10-(sm/100)*100)+48);
        send_serial(' ');
        
        ad_init(0x1);
        sm = ad_sample();
        send_serial('y');
        send_serial((sm/100)+48);
        send_serial((sm/10-(sm/100)*10)+48);
        send_serial((sm-((sm/10-(sm/100)*10))*10-(sm/100)*100)+48);
        send_serial(' ');
        
        ad_init(0x2);
        sm = ad_sample();
        send_serial('z');
        send_serial((sm/100)+48);
        send_serial((sm/10-(sm/100)*10)+48);
        send_serial((sm-((sm/10-(sm/100)*10))*10-(sm/100)*100)+48);
        send_serial(' ');
        
    }
    return 0;
}
