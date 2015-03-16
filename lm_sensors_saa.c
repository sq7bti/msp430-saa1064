#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
/* for strlen */
#include <string.h>

/* from i2c.h */
#define I2C_SLAVE	0x0703	/* Change slave address			*/

#define SEGF 0x01
#define SEGG 0x02
#define SEGA 0x04
#define SEGB 0x08
#define SEGP 0x10
#define SEGE 0x20
#define SEGC 0x40
#define SEGD 0x80

static void help(void)
{
	fprintf(stderr, "usage: saa 0123\n");
	exit(1);
}

int main(int argc, char *argv[]) {

	int file;
	int adapter_nr = 0; /* probably dynamically determined */
	char filename[20];
	unsigned long addr;
	int rc;
	int cmd;
	unsigned char data[5];
	int flags = 0;
	int bipolar = 0;
	int ext_range = 0;

	if (argc < 2) {
		fprintf(stderr, "usage: %s 0123\n",argv[0]);
		exit(1);
	}
	sprintf(filename,"/dev/i2c-%d",adapter_nr);

	if ((file = open(filename,O_RDWR)) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		fprintf(stderr, "%s: failed to open %s, err=%d\n",argv[0],filename,file);
		exit(1);
	}

	addr = 0x38;
	if ((rc=ioctl(file,I2C_SLAVE,addr)) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		fprintf(stderr, "%s: addr ioctl failed, addr=0x%03lX, err=%d\n",argv[0],addr,rc);
		exit(1);
	}

	int i;
	int j;
	data[0] = 0x01;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;

	for(i = 0, j = 4; ((i < strlen(argv[1])) && (j > 0)); ++i, --j) {
		switch ( argv[1][i] ) {
		case '0':
			data[j] = SEGA | SEGB | SEGC | SEGD | SEGE | SEGF;
			break;
		case '1':
			data[j] = SEGB | SEGC;
			break;
		case '2':
			data[j] = SEGA | SEGB | SEGD | SEGE | SEGG;
			break;
		case '3':
			data[j] = SEGA | SEGB | SEGC | SEGD | SEGG;
			break;
		case '4':
			data[j] = SEGB | SEGC | SEGF | SEGG;
			break;
		case '5':
			data[j] = SEGA | SEGC | SEGD | SEGF | SEGG;
			break;
		case '6':
			data[j] = SEGA | SEGC | SEGD | SEGE | SEGF | SEGG;
			break;
		case '7':
			data[j] = SEGA | SEGB | SEGC;
			break;
		case '8':
			data[j] = SEGA | SEGB | SEGC | SEGD | SEGE | SEGF | SEGG;
			break;
		case '9':
			data[j] = SEGA | SEGB | SEGC | SEGD | SEGF | SEGG;
			break;
		case 'A':
		case 'a':
			data[j] = SEGA | SEGB | SEGC | SEGE | SEGF | SEGG;
			break;
		case 'B':
		case 'b':
			data[j] = SEGC | SEGD | SEGE | SEGF | SEGG;
			break;
		case 'C':
		case 'c':
			data[j] = SEGA | SEGD | SEGE | SEGF;
			break;
		case 'D':
		case 'd':
			data[j] = SEGB | SEGC | SEGD | SEGE | SEGG;
			break;
		case 'E':
		case 'e':
			data[j] = SEGA | SEGD | SEGE | SEGF | SEGG;
			break;
		case 'F':
		case 'f':
			data[j] = SEGA | SEGE | SEGF | SEGG;
			break;
		case '-':
			data[j] = SEGG;
			break;
		case '_':
			data[j] = SEGD;
			break;
		case '^':
			data[j] = SEGA;
			break;
		case 'H':
			data[j] = SEGB | SEGC | SEGE | SEGF | SEGG;
			break;
		case 'h':
			data[j] = SEGC | SEGE | SEGF | SEGG;
			break;
		case 'T':
		case 't':
			data[j] = SEGD | SEGE | SEGF | SEGG;
			break;
		case 'N':
		case 'n':
			data[j] = SEGC | SEGE | SEGG;
			break;
		case 'U':
		case 'u':
			data[j] = SEGB | SEGC | SEGD | SEGE | SEGF;
			break;
		case 'P':
		case 'p':
			data[j] = SEGA | SEGB | SEGE | SEGF | SEGG;
			break;
		case 'L':
		case 'l':
			data[j] = SEGD | SEGE | SEGF;
			break;
		case 'R':
		case 'r':
			data[j] = SEGC | SEGE | SEGG;
			break;
		case 'Y':
		case 'y':
			data[j] = SEGB | SEGC | SEGF | SEGG;
			break;
		case '@':
			data[j] = SEGA | SEGB | SEGF | SEGG;
			break;
		default:
			data[j] = 0x00;
			break;
		}
		if (((i+1) < strlen(argv[1])) && (argv[1][i+1] == '.')) {
				data[j] |= SEGP;
/* 				fprintf(stderr, "argument field %d: %c : 0x%x with dot\n",j,argv[1][i],data[j]); */
				++i;
		}
/* 		else */
/* 			fprintf(stderr, "argument field %d: %c : 0x%x\n",j,argv[1][i],data[j]); */
	}

	if ( write(file,data,5) != 5) {
		/* ERROR HANDLING: i2c transaction failed */
		fprintf(stderr, "%s: i2c write failed\n",argv[0]);
		exit(1);
	}

	return(0);
}
