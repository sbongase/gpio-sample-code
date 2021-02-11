#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

typedef volatile uint32_t vuint32_t;

#define BCM2708_PERI_BASE        0x3F000000  //0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

/* Timer Peripheral Device Structure Overlay */
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


typedef union {
        struct {
        vuint32_t fsel0:3;
        vuint32_t fsel1:3;
        vuint32_t fsel2:3;
        vuint32_t fsel3:3;
        vuint32_t fsel4:3;
        vuint32_t fsel5:3;
        vuint32_t fsel6:3;
        vuint32_t fsel7:3;
        vuint32_t fsel8:3;
        vuint32_t fsel9:3;
        vuint32_t reserved:2;
        } Bits;
        vuint32_t wGPFSel0;
} UGPFSel0, *pUGPFSel0;

typedef union {
        struct {
        vuint32_t fsel10:3;
        vuint32_t fsel11:3;
        vuint32_t fsel12:3;
        vuint32_t fsel13:3;
        vuint32_t fsel14:3;
        vuint32_t fsel15:3;
        vuint32_t fsel16:3;
        vuint32_t fsel17:3;
        vuint32_t fsel18:3;
        vuint32_t fsel19:3;
        vuint32_t reserved:2;
        } Bits;
        vuint32_t wGPFSel1;
} UGPFSel1, *pUGPFSel1;


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
        //set the value using a bit field
        pUGPFSel1 pGPFSel1 = (pUGPFSel1)(&(pgpio->GPFSel[1]));
        pGPFSel1->Bits.fsel18 = 1;
        
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
