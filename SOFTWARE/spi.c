#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>


static const char *device = "/dev/spidev2.0";
static uint8_t spiMode = 3;
static uint8_t spiBPW = 8;
static uint32_t spiSpeed = 500000;
static uint16_t spiDelay = 0;

static int spiSendReceive(int spi_fd, uint8_t *TxBuf, int iTxLen, uint8_t *RxBuf, int iRxLen ) {
	int status;
    if( RxBuf == NULL || iRxLen == 0 ){
		
        struct spi_ioc_transfer spi;
        memset( &spi, 0, sizeof spi);
		
		/* 
		Define message parameters.
		Since RxBuf is NULL we what to write. spi.rx_buf should be 0
		*/
        spi.tx_buf = (unsigned long) TxBuf ;
        spi.rx_buf = 0;
        spi.len = iTxLen;
        spi.delay_usecs = spiDelay;
        spi.speed_hz = spiSpeed;
        spi.bits_per_word = spiBPW;
		
		/*
		Send message
		*/
        status = ioctl (spi_fd, SPI_IOC_MESSAGE(1), &spi) ;
        if (status < 0) {
            perror("can't send spi message");
            return -1;
        }
        return status;
    }else{
	
        struct spi_ioc_transfer	spi[2];
        memset(spi, 0, sizeof spi);
    
		/*
		Define message parameters. iTxLen must not be 0
		*/
		spi[0].tx_buf = (unsigned long) TxBuf;
        spi[0].len = iTxLen;
		spi[0].delay_usecs = spiDelay ;
        spi[0].speed_hz = spiSpeed ;
        spi[0].bits_per_word = spiBPW ;
        
		spi[1].rx_buf = (unsigned long) RxBuf;
        spi[1].len = iRxLen;		
        spi[1].delay_usecs = spiDelay ;
        spi[1].speed_hz = spiSpeed ;
        spi[1].bits_per_word = spiBPW ;
        
		/*
		Send the two messages
		*/
		status = ioctl(spi_fd, SPI_IOC_MESSAGE(2), spi);
        if (status < 0) {
            perror("SPI_IOC_MESSAGE");
            return -1;
        }
        return status;
	}
}

int main(int argc, char *argv[]) {

	int fd;
	int ret;
	/*
	Initialize SPI
	*/
	fd = open(device, O_RDWR);
	if (fd < 0){
		perror("can't open device");
		exit(1);
	}
	ret = ioctl(fd, SPI_IOC_WR_MODE, &spiMode);
	if (ret == -1){
		perror("can't set spi mode");
		exit(1);
	}
	ret = ioctl(fd, SPI_IOC_RD_MODE, &spiMode);
	if (ret == -1){
		perror("can't get spi mode");
		exit(1);
	}
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW);
	if (ret == -1){
		perror("can't set bits per word");
		exit(1);
	}
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spiBPW);
	if (ret == -1){
		perror("can't get bits per word");
		exit(1);
	}
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
	if (ret == -1){
		perror("can't set max speed hz");
		exit(1);
	}
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spiSpeed);
	if (ret == -1){
		perror("can't get max speed hz");
		exit(1);
	}
	printf("spi mode: %d\n", spiMode);
	printf("bits per word: %d\n", spiBPW);
	printf("max speed: %d Hz (%d KHz)\n", spiSpeed, spiSpeed/1000);


	uint8_t TxBuf[1];
	uint8_t RxBuf[10];

	/*
	Make read from register 0x02
	*/
	TxBuf[0] = 0x82;

	/*
	Read the 7 bytes containing data
	*/
	spiSendReceive(fd, TxBuf, 1, RxBuf, 7);

	/*
	Print temperature
	*/
	printf("Temperature: %.1fC\n", RxBuf[6]*0.5 - 30);

	/*
	Convert X, Y, Z
	*/
	uint16_t X, Y, Z;
	X = (RxBuf[1] << 2) | (RxBuf[0] >> 6);
	Y = (RxBuf[3] << 2) | (RxBuf[2] >> 6);
	Z = (RxBuf[5] << 2) | (RxBuf[4] >> 6);

	/*
	Calculate X, Y and Z
	*/
	double AccX, AccY, AccZ;
	AccX = (X & 0x200) ? (0 - (((~X & 0x3FF) + 1) * 0.004)) : (X * 0.004);
	AccY = (Y & 0x200) ? (0 - (((~Y & 0x3FF) + 1) * 0.004)) : (Y * 0.004);
	AccZ = (Z & 0x200) ? (0 - (((~Z & 0x3FF) + 1) * 0.004)) : (Z * 0.004);

	printf("X axis: %.3f\n", AccX);
	printf("Y axis: %.3f\n", AccY);
	printf("Z axis: %.3f\n", AccZ);

	close(fd);
	return 0;
}
