#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "serial.h"
#include "analog.h"
#include "network.h"

#define MAC_SIZE	6
#define IPV4_SIZE	4

/*typedef struct data {
    char data[5][5000]={
        {
            0x01, // id
            0x00, // acceleration x
            0x00, // acceleration y
            0x00, // acceleration z
            0x00 // temperature
        }, {{0}}
    };
    int fin = 0;
} data ; /*/

uint8_t len_udp = 0x0D;
uint16_t src_addr[2] = {0xAC1A, 0x4FCB};
uint16_t des_addr[2] = {0xAC1A, 0x4FCA};

char data[5]={
    0x01, // id
    0x00, // acceleration x
    0x00, // acceleration y
    0x00, // acceleration z
    0x00 // temperature
};

char hip[20]={
    0x45,0x00, // Ipv4, Taille entêtes, Type service
    0x00,0x21, // Longueur totale
    0x00,0x00,0x40,0x00, // Fragmentation
    0x40,0x11, // Durée de vie, Protocole
    0x43,0x02, // Somme de contrôle
    0xAC,0x1A,0x4F,0xCB, // ip source
    0xAC,0x1A,0x4F,0xCA // ip destination
};
char hudp[8]={
    0x04,0x00, // Port source
    0x04,0x00, // Port destination
    0x00,0x0D, // Longueur
    0x00,0x00 // Somme de contrôle
};


int main(void)
{
    init_serial(9600);
    
    while(1) 
    {
        
        ad_init(0x0);
        data[1] = ad_sample(); // accX
        ad_init(0x1);
        data[2] = ad_sample(); // accY
        ad_init(0x2);
        data[3] = ad_sample(); //accZ
        ad_init(0x3);
        data[4] = ad_sample(); //temp
        
        trame_slip(hip,hudp,data); //envoi trame slip
    }
    return 0;
}
