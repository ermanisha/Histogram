/**********************************************************************
**  histogram.c
**
**	Calculating frequency of brightness level of raw data.
**
**	Version 1.0
**
**  Copyright (C) 2018 Iti Shree
**
**	This program is free software: you can redistribute it and/or
**	modify it under the terms of the GNU General Public License
**	as published by the Free Software Foundation, either version
**	2 of the License, or (at your option) any later version.
**
**********************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

static char *cmd_name = NULL;

static uint32_t map_base = 0x18000000;
static uint32_t map_size = 0x08000000;
static uint32_t map_addr = 0x00000000;

static char *dev_mem = "/dev/mem";

#define NUM_COLS 4096
#define NUM_ROWS 3072

#define ERR_WRITE 1

void write_data(FILE *fp, uint16_t hist[4096])
{
	if (fp == NULL)
	{
		printf("Error while opening the file.\n");
		exit(0);
	}

	for (unsigned i = 0; i < 4096; i++)
		fprintf(fp, "%u\t%u\n", i, hist[i]);
	fclose(fp);
}

void load_data(uint8_t *buf, uint64_t *ptr)
{

	for (unsigned i = 0; i < NUM_COLS / 2; i++)
	{
		unsigned ce = i * 3;
		unsigned co = (i + NUM_COLS / 2) * 3;

		uint64_t val = ptr[i];

		buf[ce + 0] = (val >> 56) & 0xFF;
		buf[ce + 1] = (val >> 48) & 0xFF;
		buf[ce + 2] = (val >> 40) & 0xFF;

		buf[co + 0] = (val >> 32) & 0xFF;
		buf[co + 1] = (val >> 24) & 0xFF;
		buf[co + 2] = (val >> 16) & 0xFF;
	}
}

int main(int argc, char **argv)
{
	extern int optind;
	extern char *optarg;
	int c, err_flag = 0;

#define OPTIONS "hB:S:"
#define VERSION "1.0"

	cmd_name = argv[0];
	while ((c = getopt(argc, argv, OPTIONS)) != EOF)
	{
		switch (c)
		{
		case 'h':
			fprintf(stderr, "This is %s " VERSION "\n"
							"options are:\n"
							"-h        print this help message\n"
							"-B <val>  memory mapping base\n"
							"-S <val>  memory mapping size\n",
					cmd_name);
			exit(0);
			break;

		case 'B':
			map_base = strtoll(optarg, NULL, 0);
			break;

		case 'S':
			map_size = strtoll(optarg, NULL, 0);
			break;

		default:
			err_flag++;
			break;
		}
	}

	if (err_flag)
	{
		fprintf(stderr, "Usage: %s -[" OPTIONS "] [file]\n"
						"%s -h for help.\n",
				cmd_name, cmd_name);
		exit(1);
	}

	int fd = open(dev_mem, O_RDWR | O_SYNC);
	if (fd == -1)
	{
		fprintf(stderr,
				"error opening >%s<.\n%s\n", dev_mem, strerror(errno));
		exit(2);
	}

	if (map_addr == 0)
		map_addr = map_base;

	void *base = mmap((void *)map_addr, map_size, PROT_READ | PROT_WRITE,
					  MAP_SHARED, fd, map_base);
	if (base == (void *)-1)
	{
		fprintf(stderr,
				"error mapping 0x%08X+0x%08X @0x%08X.\n%s\n", map_base,
				map_size, map_addr, strerror(errno));
		exit(2);
	}

	fprintf(stderr,
			"mapped 0x%08lX+0x%08lX to 0x%08lX.\n",
			(long unsigned)map_base, (long unsigned)map_size,
			(long unsigned)base);

	if (argc > optind)
	{
		close(0);

		int fd = open(argv[optind], O_RDONLY, 0);
		if (fd == -1)
		{
			fprintf(stderr,
					"error opening >%s< for reading.\n%s\n", argv[optind],
					strerror(errno));
			exit(2);
		}
	}

	uint16_t buf[NUM_COLS * 3];
	uint64_t *ptr = base;

	uint16_t hist[4096] = {0};

	for (unsigned j = 0; j < NUM_ROWS / 2; j++)
	{
		for (unsigned i = 0; i < NUM_ROWS / 2; i++)
		{
			unsigned ce = i * 4;

			uint64_t val = ptr[i];

			buf[ce + 0] = (val >> 52) & 0xFFF;
			buf[ce + 1] = (val >> 40) & 0xFFF;
			buf[ce + 2] = (val >> 28) & 0xFFF;
			buf[ce + 3] = (val >> 16) & 0xFFF;
		}

		for (unsigned i = 0; i < NUM_ROWS * 2; i++)
		{
			hist[buf[i]]++;
		}

		ptr += NUM_COLS / 2;
	}

	unsigned sum = 0;

	for (unsigned i = 0; i < 10; i++)
		sum += hist[i];

	printf("%u", sum);

	FILE *fp;
	char *fileName = "sample.txt";
	write_data(fp = fopen(fileName, "w+"), hist);

	exit(0);
}
