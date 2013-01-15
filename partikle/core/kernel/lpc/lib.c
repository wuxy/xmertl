#include "lib.h"

void
delay(int d){
	int i,j,k;

	i=j=k=d;
	while (i-- > 0)
		while (j-- > 0)
			while (k-- > 0)
				; 

}

/*
void
rmemdmp(ulong* a, int n, char * nfo){
	int i;
	
	rprintf("memdmp: %s", nfo);
	for (i=0; i<n; i++)
		rprintf("a[%d] %08lx = %08lx\n", i, &a[i], a[i]);
}

void*
rmemset(ulong *d, int c, int n){
	int i;
	for (i=0; i<n; i++)
		d[i] = c;

       return (void*) d;
}

void*
rmemcpy(ulong *d, ulong *s, int n){
	int i;
	for (i=0; i<n; i++)
		d[i] = s[i];

	return (void*)d;
}

int
rmemchk(ulong *d, ulong *s, int n){
	int i, bads;

	bads=0;
	for (i=0; i<n; i ++){
		
		if (d[i] != s[i]){
			if (!bads)
				rprintf("\n");
				
			bads++;
			rprintf("rmemchk fails (%d) at %08lx\n", bads, &d[i]);
		}
		if (0) rprintf("\nrmemchk debug %08lx %08lx %08lx %08lx\n", &d[i], &s[i], d[i], s[i]);
	}
	return bads;
}
*/

