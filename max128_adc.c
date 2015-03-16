#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>

/* from i2c.h */
#define I2C_SLAVE	0x0703	/* Change slave address			*/

static void help(void)
{
	fprintf(stderr,
		"usage: adc [-b] [-r] AI\n"
		"-b - bipolar input\n"
		"-r - extended range (10V instead of 5V)\n");
	exit(1);
}

int main(int argc, char *argv[]) {

	int file;
	int adapter_nr = 0; /* probably dynamically determined */
	char filename[20];
	unsigned long addr;
	int rc;
	int cmd;
	unsigned char data[32];
	int flags = 0;
	int bipolar = 0;
	int ext_range = 0;

	while (1+flags < argc && argv[1+flags][0] == '-') {
		switch (argv[1+flags][1]) {
		case 'b': bipolar = 1; break;
		case 'r': ext_range = 1; break;
		case 'h': help();
			break;
		default:
			fprintf(stderr, "Error: Unsupported option "
				"\"%s\"!\n", argv[1+flags]);
			help();
			exit(1);
		}
		flags++;
	}

	if (argc < flags + 2) {
		fprintf(stderr, "usage: %s [-b] [-r] AImask (8 bit int bitmask)\n",argv[0]);
		exit(1);
	}  
	sprintf(filename,"/dev/i2c-%d",adapter_nr);

	if ((file = open(filename,O_RDWR)) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		fprintf(stderr, "%s: failed to open %s, err=%d\n",argv[0],filename,file);
		exit(1);
	}

	addr = 0x28;
	if ((rc=ioctl(file,I2C_SLAVE,addr)) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		fprintf(stderr, "%s: addr ioctl failed, addr=0x%03lX, err=%d\n",argv[0],addr,rc);
		exit(1);
	}

	float scale = 0.0012207;
	if ( ext_range )
		scale = 0.0024414;
	if ( bipolar )
		scale *= 2;

	int sel = 0xff & (atoi(argv[1+flags]));
	int i;
	for(i = 0; i < 8; ++i) {
		if ( ! ((0x1 << i) & sel) )
			continue;
//		int sel = (0x1 << i) & (atoi(argv[1+flags]));
		data[0] = (i << 4) | (bipolar << 2) | (ext_range << 3) | 0x80;

		if ( write(file,data,1) != 1) {
			/* ERROR HANDLING: i2c transaction failed */
			fprintf(stderr, "%s: i2c write failed\n",argv[0]);
			exit(1);
		}

		if ( read(file,data,2) != 2) {
			/* ERROR HANDLING: i2c transaction failed */
			fprintf(stderr, "%s: i2c write failed\n",argv[0]);
			exit(1);
		} else {
			int rval=((data[0] << 4) | (data[1] >> 4));
			if ( bipolar && ( rval & (0x1 << 11) ) )
					rval -= (0x1 << 12);
			fprintf(stdout, "ADC[%d]: %3.9fV\n", i, rval * scale);
		}
	}
	return(0);
}
