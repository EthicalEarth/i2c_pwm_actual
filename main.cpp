/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
 * Copyright (C) 2018 Andrey Ivanov <EthicalTerminal@gmail.com>
 * 
 * I2C_Relay is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * I2C_Relay is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <bitset>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>
#include <ctype.h>
#include <type_traits>



using namespace std;

//#define I2C_ADDR 0x40           //Адрес устройства

#define	PCA9685_MODE1               0x00
#define	PCA9685_MODE2               0x01
#define	PCA9685_SUBADR1             0x02
#define	PCA9685_SUBADR2             0x03
#define	PCA9685_SUBADR3             0x04
#define	PCA9685_PRESCALE            0xFE
#define	PCA9685_LED0_ON_L           0x06
#define	PCA9685_LED0_ON_H           0x07
#define	PCA9685_LED0_OFF_L          0x08
#define	PCA9685_LED0_OFF_H          0x09
#define	PCA9685_ALL_LED_ON_L        0xFA
#define	PCA9685_ALL_LED_ON_H        0xFB
#define	PCA9685_ALL_LED_OFF_L       0xFC
#define	PCA9685_ALL_LED_OFF_H       0xFD

// Bits
#define	PCA9685_RESTART             0x80
#define	PCA9685_SLEEP               0x10
#define	PCA9685_ALLCALL             0x05
#define	PCA9685_INVRT               0x10
#define	PCA9685_OUTDRV              0x04

#define BUFFER_SIZE 0x01
#define CLOCK_FREQ 25000000.0
// DEFINE BASICAL VARIABLES
std::string  HUB;
int FREQ;
int I2C_ADDR;
int DEVICE;
int i2cfd;
int TIME =1;

/////////////////////////////////////////////////////////////////////////////////

void openfd() {
    char* c = &*HUB.begin();
    //int i2cfd;
    //i2cfd = open(c, O_RDWR);
    
	if (( i2cfd = open(c, O_RDWR)) < 0) {
/*		syslog(LOG_ERR, "Couldn't open I2C Bus %d [openfd():open %d]", _i2cbus,
				errno);
*/
	}
	if (ioctl(i2cfd, I2C_SLAVE, I2C_ADDR) < 0) {
/*		syslog(LOG_ERR, "I2C slave %d failed [openfd():ioctl %d]", _i2caddr,
				errno);
*/
	}
}
/////////////////////////////////////////////////////////////////////////////////
void closefd(){
    
    close(i2cfd);
    
}    




/////////////////////////////////////////////////////////////////////////////////
uint8_t write_byte(uint8_t address, uint8_t data) {
    openfd();
	if (i2cfd != -1) {
		uint8_t buffer[2];
		buffer[0] = address;
		buffer[1] = data;
		if (write(i2cfd, buffer, sizeof(buffer)) != 2) {
/*			syslog(LOG_ERR,
					"Failed to write to I2C Slave 0x%x @ register 0x%x [write_byte():write %d]",
					_i2caddr, address, errno);*/
            closefd();
			return (-1);
		} else {
/*			syslog(LOG_INFO, "Wrote to I2C Slave 0x%x @ register 0x%x [0x%x]",
					_i2caddr, address, data);*/
            closefd();
			return (-1);
		}
	} else {
/*		syslog(LOG_INFO, "Device File not available. Aborting write");*/
        closefd();
		return (-1);
	}
	closefd();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////



void initial(){
        int prescale = ((CLOCK_FREQ/4096/FREQ - 0.5));
        cout<<prescale<<endl;
		write_byte(PCA9685_MODE2, PCA9685_OUTDRV);
		write_byte(PCA9685_MODE1, PCA9685_SLEEP);
		sleep(0.005);
		write_byte(PCA9685_PRESCALE, prescale);
		write_byte(PCA9685_MODE1, PCA9685_ALLCALL | PCA9685_RESTART);
		sleep(0.005);
        
}    
        

void setPWM(uint8_t channel, int on, int off){
        write_byte(PCA9685_LED0_ON_L+4*channel, on & 0xFF);
		write_byte(PCA9685_LED0_ON_H+4*channel, on >> 8);
		write_byte(PCA9685_LED0_OFF_L+4*channel, off & 0xFF);
		write_byte(PCA9685_LED0_OFF_H+4*channel, off >> 8);
}

void off(){
		write_byte(PCA9685_ALL_LED_ON_L, 0);
		write_byte(PCA9685_ALL_LED_ON_H, 0);
		write_byte(PCA9685_ALL_LED_OFF_L, 0);
		write_byte(PCA9685_ALL_LED_OFF_H, 0);

}


/////////////////////////////////////////////

uint8_t read_byte(uint8_t address) {
    openfd();    
	if (i2cfd != -1) {
        uint8_t dataBuffer[BUFFER_SIZE];
		uint8_t buffer[BUFFER_SIZE];
		buffer[0] = address;
        
		if (write(i2cfd, buffer, BUFFER_SIZE) != BUFFER_SIZE) {
//			syslog(LOG_ERR,
//					"I2C slave 0x%x failed to go to register 0x%x [read_byte():write %d]",
//					_i2caddr, address, errno);

			return (-1);
            closefd();
		} else {
			if (read(i2cfd, dataBuffer, BUFFER_SIZE) != BUFFER_SIZE) {
//				syslog(LOG_ERR,
//						"Could not read from I2C slave 0x%x, register 0x%x [read_byte():read %d]",
//						_i2caddr, address, errno);

				return (-1);
			} else {
				return dataBuffer[0];
                closefd();
			}
		}
	} else {

//		syslog(LOG_ERR, "Device File not available. Aborting read");

		return (-1);
        closefd();
	}

}

////////////////////////////////////////////

int getPWM(uint8_t channel){
	int ledval = 0;
	ledval = read_byte(PCA9685_LED0_OFF_H+4*(channel));
	ledval = ledval & 0x0F;
	ledval <<= 8;
	ledval += read_byte(PCA9685_LED0_OFF_L+4*(channel));
	return ledval;
}

//////////////////////////////////////////////



/****Main****/
int main(int argc, char **argv){
I2C_ADDR=0x40;
FREQ=250;
HUB="/dev/i2c-1";
initial();
//off();
int chanel[16];
float amp[16]={0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
float delta[16];
float current[16];
int delay = 10; //ms

//SETTINGS
int newchanel[16];
newchanel[0]=100; //RED
newchanel[1]=100; //GREEN
newchanel[2]=100; //BLUE
newchanel[3]=100; //WHITE
newchanel[4]=0;
newchanel[5]=0;
newchanel[6]=0;
newchanel[7]=0;
newchanel[8]=0;
newchanel[9]=0;
newchanel[10]=0;
newchanel[11]=0;
newchanel[12]=0;
newchanel[13]=0;
newchanel[14]=0;
newchanel[15]=0;

//*** Set up values ***
for (int ch = 0; ch < 16; ch++){
    chanel[ch]=getPWM(ch);
    current[ch]=chanel[ch];
    while (1){
        delta[ch]= (newchanel[ch]-current[ch])*amp[ch];
        //cout<<"chanel="<<ch<<" "<<"delta="<<delta[ch]<<"amp="<<amp[ch]<<endl;
        current[ch]=current[ch]+delta[ch];
        //cout<<"Current value = "<< current[ch]<<endl;
        setPWM(ch,0,(int)current[ch]);
        usleep(delay*1000);
        if (abs(delta[ch])<=0.01){
            //cout<<"Chanel N="<<ch<<" exit"<<endl;
            break;    
        }
    }
}
//*** Set up values ***   

}

