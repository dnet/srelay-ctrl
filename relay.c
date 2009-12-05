#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>

#define BAUDRATE B9600
#define PORT "/dev/ttyUSB0"

int main(int argc, char** argv)
{
	struct termios oldtio, newtio;
	char buf, i;
	int fd;

	fd = open(PORT, O_RDWR | O_NOCTTY);
	if (fd < 0)
	{
		perror(PORT);
		return 1;
	}

	tcgetattr(fd, &oldtio);

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	buf = '?';
	write(fd, &buf, 1);
	read(fd, &buf, 1);

	if (argc == 3)
	{
		if (strlen(argv[2]) == 1)
		{
			if (!strcmp("on", argv[1]))
			{
				switch (argv[2][0])
				{
					case '4': buf |= 1; break;
					case '3': buf |= 2; break;
					case '2': buf |= 4; break;
					case '1': buf |= 8; break;
				}
			}
			else if (!strcmp("off", argv[1]))
			{
				switch (argv[2][0])
				{
					case '4': buf &= 254; break;
					case '3': buf &= 253; break;
					case '2': buf &= 251; break;
					case '1': buf &= 247; break;
				}
			}
		}
	}
	else if (argc == 2 && !strcmp("reset", argv[1]))
		buf = '@';

	write(fd, &buf, 1);
	read(fd, &buf, 1);

	for (i = 1; i != 5; i++)
		printf("relay%d: %s\n", i, (buf & (16 >> i)) == (16 >> i) ? "on" : "off");

	usleep(0);
	tcsetattr(fd, TCSANOW, &oldtio);
	return 0;
}
