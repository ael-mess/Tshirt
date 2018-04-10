#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "serial.h"
#include "analog.h"

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
    0x00,len_udp, // Longueur
    0x00,0x00 // Somme de contrôle
};

void trame_slip(char hip[20], char hudp[8], char data[5]) //envoi de la trame avec protocole SLIP
{
    int i;
    for(i=0;i<33;i++)
    {
        if(i<20) {                                   //envoi ip sur le port serie
            if (hip[i] == 0xC0) {
                send_serial(0xDB); //ESC
                send_serial(0xDC); //ESC_END
            }
            else if (hip[i] == 0xDB) {
                send_serial(0xDB); //ESC
                send_serial(0xDD); //ESC_ESC
            }
            else {
                send_serial(hip[i]);
            }
        }
        
        else if(i<28 && i>19) {                     //envoi udp sur le port serie
            if (hudp[i-20] == 0xC0) {
                send_serial(0xDB); //ESC
                send_serial(0xDC); //ESC_END
            }
            else if (hudp[i-20] == 0xDB) {
                send_serial(0xDB); //ESC
                send_serial(0xDD); //ESC_ESC
            }
            else {
                send_serial(hudp[i-20]);
            }
        }
        
        else {                                      //envoi des data sur le port serie
            if (data[i-28] == 0xC0) {
                send_serial(0xDB); //ESC
                send_serial(0xDC); //ESC_END
            }
            else if (data[i-28] == 0xDB) {
                send_serial(0xDB); //ESC
                send_serial(0xDD); //ESC_ESC
            }
            else {
                send_serial(data[i-28]);
            }
        }
    }
    send_serial(0xC0);                              //E.N.D SLIP
}

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
