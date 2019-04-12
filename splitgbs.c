#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <zlib.h>
#include <sys/stat.h>

#define BUFFER_SIZE          4 << 10
#define ADD 8 

static void die(const char *errstr, ...);

void
die(const char *errstr, ...) {
 va_list ap;

 va_start(ap, errstr);
 vfprintf(stderr, errstr, ap);
 va_end(ap);
 exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
 FILE *file;
 char buf[BUFFER_SIZE];
 char buf2[BUFFER_SIZE];
 char buf3[BUFFER_SIZE];
 char buf4[BUFFER_SIZE];
 char *dir, *sample;
 int i, j, l, n;
 char **barcodes, **p;
 char *s, *t;
 gzFile *f, u, *outfiles, *fastq, tooshort;
 struct stat sb;
 int *lengths, *ls;

 if(argc < 3)
  die("too few arguments\n");

 dir = argv[1];

 if(!(stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode)))
  die("%s : directory does not exist\n", dir);

 if(!(file = fopen(argv[2], "r")))
  die("error opening key file\n");

 if(!(fastq = gzopen(argv[3], "rb")))
  die("error opening fastq file\n");

 i = 0;
 while(fgets(buf, sizeof buf, file))
  i++;
 n = i;

 rewind(file);
 
 if(!(p = barcodes = malloc( n * sizeof(char*))))
  die("malloc error barcodes \n");
 if(!(f = outfiles = malloc( n * sizeof(gzFile))))
  die("malloc error files \n");
 if(!(ls = lengths = malloc( n * sizeof(int))))
  die("malloc error lengths \n");

 while(fgets(buf, sizeof buf, file)){
  buf[strlen(buf)-1] = '\0';
  t = strtok(buf, "\t");
  *(ls++) = l = strlen(t);
  if(!(s = *(p++) = malloc((l + ADD + 1) * sizeof(char))))
   die("malloc error tmp \n");
  for(i = 0; i < l; i++)
   *(s++) = *(t++);
  *(s++) = 'T';  *(s++) = 'G';  *(s++) = 'C';  *(s++) = 'A';   *s = '\0';

  sample = strdup(strtok(NULL, "\t"));
  t = buf2;
  strcpy(t, dir);
  t += strlen(dir);
  *(t++) = '/';
  strcpy(t, sample);
  t += strlen(sample);
  strcpy(t, ".fq.gz");
  *(f++) = gzopen(buf2, "wb");
 }

  t = buf;
  strcpy(t, dir);
  t += strlen(dir);
  strcpy(t, "/undetermined.fq.gz");
  u = gzopen(buf, "wb");

  t = buf;
  strcpy(t, dir);
  t += strlen(dir);
  strcpy(t, "/tooshort.fq.gz");
  tooshort = gzopen(buf, "wb");

 while(gzgets(fastq, buf, sizeof buf)){
  if(!gzgets(fastq, buf2, sizeof buf2))
   die("error reading stdin");
  if(!gzgets(fastq, buf3, sizeof buf3))
   die("error reading stdin");
  if(!gzgets(fastq, buf4, sizeof buf4))
   die("error reading stdin");
  if(strlen(buf2) - 1 < 30){
   gzprintf(tooshort, "%s%s%s%s", buf, buf2, buf3, buf4);
   goto nextline;
  }
  for(ls = lengths, p = barcodes, i = 0, f = outfiles; i < n; ls++, p++, i++, f++){
   l = *ls + 4;
   for( j = 0, t = *p, s = buf2; j < l; j++, t++, s++)
    if(*t != *s)
     goto next;
    for(s = buf2, t = buf4, j= 0; j < l; j++, s++, t++);
    if(strlen(s) - 1 >= 30)
     gzprintf(*f, "%s%s%s%s", buf, s, buf3, t);
    else
     gzprintf(tooshort, "%s%s%s%s", buf, s, buf3, t);
    goto nextline;
    next:;
  }
  gzprintf(u, "%s%s%s%s", buf, buf2, buf3, buf4);
 nextline:;
 }

 gzclose(fastq);

 for(i = 0, f = outfiles; i < n; i++, f++)
  gzclose(*f);
 gzclose(u);
 gzclose(tooshort);
}
