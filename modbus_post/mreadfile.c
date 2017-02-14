#include "mreadfile.h"

int mreadfile( double **x, double **y, char *filename ) {
	char	buf[1024];
	int		n;
	FILE	*infile;

	infile = fopen( filename, "r" );
	assert( infile != NULL );

	n=-1;
	while( !feof( infile) ) {
		fgets( buf, 1024, infile );
		/* Comments begin with a '#' */
		if (buf[0] != '#') {
			n++;
			*x = realloc( *x, sizeof( double )*(n+1) );
			*y = realloc( *y, sizeof( double )*(n+1) );
			sscanf( buf, "%lf %lf\n", *x+n, *y+n );
		}
	}
	fclose( infile );

	return( n );
}
