#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

typedef volatile uint32_t vuint32_t;

#define BCM2708_PERI_BASE        0x3F000000  //Different for RPi4
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

/* GPIO Peripheral Device Structure Overlay */
typedef struct {
  vuint32_t GPFSel[6];
  vuint32_t wReserved0; 
  vuint32_t GPSet0;
  vuint32_t GPSet1;
  vuint32_t wReserved1; 
  vuint32_t GPClr0;
  vuint32_t GPClr1;
  vuint32_t wReserved2; 
  vuint32_t GPLev0;
  vuint32_t GPLev1;
  vuint32_t wReserved3; 
} BCM2835_GPIO_Type, *pBCM2835_GPIO_Type;

int main(int argc, char **argv)
{
        int fd ;

        //Obtain handle to physical memory
        if ((fd = open ("/dev/mem", O_RDWR | O_SYNC) ) < 0) {
                printf("Unable to open /dev/mem: %s\n", strerror(errno));
                return -1;
        }


        //map a page of memory to gpio at offset 0x3F200000 which is where GPIO goodnessstarts
        pBCM2835_GPIO_Type pgpio = (pBCM2835_GPIO_Type)mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

        if (pgpio == MAP_FAILED){
                printf("Mmap failed: %s\n", strerror(errno));
                return -1;
        }

        //set gpio18 as an output
        //add 1 to the pointer to point at 0x3F200004
        //set the value through a little bit twiddling where we only modify the bits 24-26 in the register
        pgpio->GPFSel[1] = (pgpio->GPFSel[1] & ~(7 << 24)) | (1 << 24);
        
        //toggle gpio18 every second
        while(1){
                //set the pin high
                //increment the pointer to 0x3F20001C
                //Set Bit 18 of GPSet0
                pgpio->GPSet0 |= 1 << 18;

                //sleep
                sleep(1);

                //set the pin to low
                //increment the pointer to 0x3F200028
                //Set Bit 18 of GPClr0
                pgpio->GPClr0 |= 1 << 18;

                sleep(1);
        }
}
