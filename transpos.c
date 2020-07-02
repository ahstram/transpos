/*----------------------------------------------------------------------*\
 | Program to transpose a tab-delimited text file.                       |
 |                                                                       |
 | Usage: transpos [-h n] [-d c] input_file                              |
 | Options:   -h n  means there are n header lines, which are copied     |
 |                    as is to the output file.                          |
 |            -d c  means the delimiter is c rather than tab.            |
 | Input: a tab-delimited text file, named on the command line.          |
 | Output: a tab-delimited text file, sent to stdout.                    |
 |                                                                       |
 | Peter N. Schweitzer (U.S. Geological Survey, Reston, VA 20192)        |
\*----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main (int argc, char *argv[]) {

    char *input_file;
    FILE *in;
    long size,n;
    char *buffer,*end;
    char *s,*t;
    int header_line_count = 0;
    int line_count;
    char **L;
    int i,j;
    int more_to_do;
    char delimiter = '\t';

    input_file = NULL;
    for (i=1; i < argc; i++)
        if (memcmp (argv[i],"-h",2) == 0) {
            i++;
            header_line_count = atoi (argv[i]);
            }
        else
            if (memcmp (argv[i],"-d",2) == 0) {
                i++;
                if (*argv[i]) delimiter = *argv[i];
                }
            else
                input_file = argv[i];

    if (input_file) {
        if ((in = fopen (input_file,"rb"))) {
            fseek (in,0L,SEEK_END);
            size = ftell (in);
            rewind (in);

            if ((buffer = (char *) malloc (1 + size))) {
                n = fread (buffer,1,size,in);
                if (n < size)
                    fprintf (stderr,"Warning: expected %ld, got %ld bytes from file %s\n",size,n,input_file);

                /* Count lines */
                line_count = 0;
                end = buffer + n;
                *end = 0;
                s = buffer;
                while (s < end) {
                    if (*s == '\r') {
                        if (*(s+1) == '\n') s++;
                        line_count++;
                        }
                    else
                        if (*s == '\n') line_count++;
                    s++;
                    }

                /* If the last character is not a newline, increment line_count */
                if (*(s-1) != '\n' && *(s-1) != '\r') line_count++;

                /* Allocate line pointer array */
                if ((L = (char **) malloc ((long)line_count * sizeof (char *)))) {

                    /* Locate lines and initialize line pointers */
                    i = 0;
                    s = buffer;
                    L[i++] = s;
                    while (s < end) {
                        if (*s == '\r') {
                            *s = 0;
                            if (*(s+1) == '\n') {
                                s++;
                                *s = 0;
                                }
                            if (s+1 < end) L[i++] = s+1;
                            }
                        else
                            if (*s == '\n') {
                                *s = 0;
                                if (s+1 < end) L[i++] = s+1;
                                }
                        s++;
                        }

                    /* Output header lines if any */
                    for (i=0; i < header_line_count; i++) puts (L[i]);

                    /* Transpose the remaining lines */
                    j = i;
                    for (i=j, more_to_do=0; i < line_count; i++) more_to_do += *L[i];
                    while (more_to_do) {
                        for (i=j; i < line_count; i++) {
                            for (t=L[i]; *t && *t != delimiter; t++) putc (*t,stdout);
                            if (i < line_count - 1) putc (delimiter,stdout);
                            if (*t) L[i] = t+1;
                            else    L[i] = t;
                            }
                        putc ('\n',stdout);
                        for (i=j, more_to_do=0; i < line_count; i++) more_to_do += *L[i];
                        }
                    free (L);
                    }
                else {
                    fprintf (stderr,"Error: could not allocate %ld bytes for line pointer array\n",(long) line_count * sizeof (char *));
                    exit (1);
                    }
                free (buffer);
                }
            else {
                fprintf (stderr,"Error: could not allocate %ld bytes for file %s\n",size,input_file);
                exit (1);
                }

            fclose (in);
            }
        else {
            fprintf (stderr,"Error: could not open input file %s\n",input_file);
            exit (1);
            }
        }
    else {
        fprintf (stderr,"Usage: %s [-h 00] input_file\n",argv[0]);
        exit (0);
        }

    return 0;
    }

/*----------------------------------------------------------------------*\
\*----------------------------------------------------------------------*/
