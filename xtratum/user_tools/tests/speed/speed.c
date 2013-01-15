#include <stdio.h>

#define LOOP 500
#define fname "tmp"

int main(int argc, char *argvs[])
{
	FILE *fl;
	int s, sn, en;
	int count = 0;
	float size = 59*8*100*1.0;
	float tm, tms = 0.0, max = 0.0, min=999999999.0;
	
	if(argc == 2)
		fl = fopen(argvs[1], "r");
	else 
		fl = fopen(fname, "r");

	if(fl == NULL) {
		fprintf(stderr, "failed to open file!\n");
		return -1;
	}

	while(!feof(fl)) {
		
		fscanf(fl, "%*");
		fscanf(fl, "%x", &s);
		fscanf(fl, "%x", &sn);
		fscanf(fl, "%x", &en);
		
		count ++;

		tm = (s*1000000000.0+sn*1.0-en*1.0)/1000000000.0;
		tm = size/tm;

		if(tm > max) max = tm;
		if(tm < min) min = tm;
		tms += tm;
		
		if(count == LOOP) break;
	}

	printf("LOOP: = %d Average SPEED = %f Max Speed = %f Min Speed = %f \n", count, tms/count, max, min);

	fclose(fl);

	return 0;
}
