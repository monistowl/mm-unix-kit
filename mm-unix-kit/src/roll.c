\
/* roll.c: tiny POSIX dice roller with append-only logging.
 * build: cc -O2 -std=c99 -Wall -Wextra roll.c -o roll
 */
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static unsigned long xorshift64(unsigned long *s) {
  unsigned long x = *s;
  x ^= x << 13; x ^= x >> 7; x ^= x << 17;
  return *s = x;
}
static int d(unsigned long *s, int sides){ return (int)(xorshift64(s) % sides) + 1; }

static void logline(const char *session, const char *who, const char *expr, const char *out, unsigned long seed){
  const char *logpath = "rng.log";
  FILE *f = fopen(logpath, "a");
  if(!f) return;
  time_t now=time(NULL);
  fprintf(f, "%ld\t%s\t%s\t%s\tseed=%lu\t%s\n", (long)now, session?session:"-", who?who:"-", expr, seed, out);
  fclose(f);
}

int main(int argc, char **argv){
  if(argc < 2){ fprintf(stderr,"usage: %s EXPR [who] [session]\n", argv[0]); return 2; }
  const char *expr = argv[1];
  const char *who = argc>2? argv[2] : getenv("USER");
  const char *session = argc>3? argv[3] : getenv("MM_SESSION");
  unsigned long seed = (unsigned long)time(NULL) ^ (getpid()<<16);
  const char *sd = getenv("MM_SEED"); if(sd) seed ^= strtoul(sd,NULL,10);
  char out[256]={0};

  if(!strncmp(expr,"n6:",3)){ // n-in-6 skill
    int thr = atoi(expr+3); // expects like "n6:5+" → 5
    unsigned long s=seed; int r = d(&s,6);
    int ok = (r>=thr);
    snprintf(out,sizeof(out),"n6=%d thr=%d %s", r, thr, ok?"SUCCESS":"FAIL");
    printf("%s\n", out);
    logline(session, who, expr, out, seed); return ok?0:1;
  }
  if(!strcmp(expr,"coin")){
    unsigned long s=seed; int r=d(&s,2);
    snprintf(out,sizeof(out),"%s", r==1?"HEADS":"TAILS");
    printf("%s\n", out); logline(session, who, expr, out, seed); return 0;
  }
  // parse NdM±K and optional kh1/kl1
  int N=1,M=20,K=0, adv=0, disadv=0;
  char tail[32]={0};
  if(sscanf(expr,"%dd%d%31s",&N,&M,tail)<2){ // maybe "d20"
    if(sscanf(expr,"d%d%31s",&M,tail)<1){ fprintf(stderr,"bad expr\n"); return 2; }
    N=1;
  }
  if(strstr(tail,"kh1")) adv=1;
  else if(strstr(tail,"kl1")) disadv=1;
  else if(tail[0]=='+'||tail[0]=='-') K = atoi(tail);
  unsigned long s=seed; int total=0;
  char buf[128]={0}; size_t bl=0;
  if(adv||disadv){
    int a=0,b=0; // roll 2 dM, keep high/low
    a=d(&s,M); b=d(&s,M);
    int pick = adv ? (a>b?a:b) : (a<b?a:b);
    total = pick + K;
    bl+=snprintf(buf+bl,sizeof(buf)-bl,"[%d,%d]%s%d", a,b, K>=0?"+":"",K);
  }else{
    bl+=snprintf(buf+bl,sizeof(buf)-bl,"[");
    for(int i=0;i<N;i++){ int r=d(&s,M); total+=r; bl+=snprintf(buf+bl,sizeof(buf)-bl,"%d%s",r,(i==N-1?"]":" ")); }
    total += K;
    if(K) bl+=snprintf(buf+bl,sizeof(buf)-bl," %s%d", K>=0?"+":"",K);
  }
  snprintf(out,sizeof(out),"%s = %d", buf, total);
  printf("%s\n", out);
  logline(session, who, expr, out, seed);
  return 0;
}
