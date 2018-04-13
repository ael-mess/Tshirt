/** Functions for network **/

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include "serial.h"
#include "analog.h"

//#include "network.h"

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
/*
uint16_t ip_checksum(void* vdata, size_t length) {
    // Cast the data pointer to one that can be indexed.
    char* data=(char*)vdata;

    // Initialise the accumulator.
    uint32_t acc=0xffff;

    // Handle complete 16-bit blocks.
    for (size_t i=0;i+1<length;i+=2) {
        uint16_t word;
        memcpy(&word,data+i,2);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (length&1) {
        uint16_t word=0;
        memcpy(&word,data+length-1,1);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Return the checksum in network byte order.
    return htons(~acc);
}
*/
/*uint16_t checksum_udp(uint16_t len_udp, uint16_t src_addr[],uint16_t dest_addr[], bool padding, uint16_t buff[])
{
    uint16_t prot_udp=17;
    uint16_t padd=0;
    uint16_t word16;
    uint32_t sum;
	
	// Find out if the length of data is even or odd number. If odd,
	// add a padding byte = 0 at the end of packet
	if (padding&1==1){
		padd=1;
		buff[len_udp]=0;
	}
	
	//initialize sum to zero
	sum=0;
	
	// make 16 bit words out of every two adjacent 8 bit words and 
	// calculate the sum of all 16 vit words
	for (i=0;i<len_udp+padd;i=i+2){
		word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
		sum = sum + (unsigned long)word16;
	}	
	// add the UDP pseudo header which contains the IP source and destinationn addresses
	for (i=0;i<4;i=i+2){
		word16 =((src_addr[i]<<8)&0xFF00)+(src_addr[i+1]&0xFF);
		sum=sum+word16;	
	}
	for (i=0;i<4;i=i+2){
		word16 =((dest_addr[i]<<8)&0xFF00)+(dest_addr[i+1]&0xFF);
		sum=sum+word16; 	
	}
	// the protocol number and the length of the UDP packet
	sum = sum + prot_udp + len_udp;

	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
    	while (sum>>16)
		sum = (sum & 0xFFFF)+(sum >> 16);
		
	// Take the one's complement of sum
	sum = ~sum;

    return ((uint16_t) sum);
}*/


