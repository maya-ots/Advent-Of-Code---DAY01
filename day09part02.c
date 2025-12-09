#include <stdio.h>
#include <stdlib.h>

typedef struct { int x, y; } P;

#define MAX_RED 50000
P red[MAX_RED];
int n = 0;

typedef struct { int start, end; } Interval;
typedef struct Row { int y; Interval *iv; int cnt, cap; } Row;

Row *rows = NULL;
int rows_count = 0;
int rows_cap = 0;

// Find or add row structure
Row* get_row(int y){
    for(int i=0;i<rows_count;i++) if(rows[i].y==y) return &rows[i];
    if(rows_count>=rows_cap){
        rows_cap = rows_cap ? rows_cap*2 : 128;
        rows = realloc(rows,sizeof(Row)*rows_cap);
    }
    rows[rows_count].y=y;
    rows[rows_count].iv=NULL;
    rows[rows_count].cnt=0;
    rows[rows_count].cap=0;
    return &rows[rows_count++];
}

// Add interval to row
void add_interval(int y,int x1,int x2){
    Row *r = get_row(y);
    if(r->cnt>=r->cap){
        r->cap = r->cap ? r->cap*2 : 4;
        r->iv = realloc(r->iv,sizeof(Interval)*r->cap);
    }
    if(x1>x2){ int t=x1;x1=x2;x2=t; }
    r->iv[r->cnt].start=x1;
    r->iv[r->cnt].end=x2;
    r->cnt++;
}

// Sort intervals in a row
int cmp_iv(const void *a,const void *b){
    Interval *i1=(Interval*)a,*i2=(Interval*)b;
    return i1->start - i2->start;
}

// Check if [lx,rx] is fully covered by intervals in row
int covered(Row *r,int lx,int rx){
    qsort(r->iv,r->cnt,sizeof(Interval),cmp_iv);
    int pos = lx;
    for(int i=0;i<r->cnt;i++){
        if(r->iv[i].end < pos) continue;
        if(r->iv[i].start>pos) return 0;
        if(r->iv[i].end>=rx) return 1;
        pos = r->iv[i].end+1;
    }
    return 0;
}

int main(){
    FILE *f = fopen("input.txt","r");
    if(!f){ printf("Cannot open input.txt\n"); return 1; }

    int x,y;
    while(fscanf(f,"%d,%d",&x,&y)==2){
        if(n>=MAX_RED){ printf("Too many red tiles\n"); return 1; }
        red[n].x=x; red[n].y=y; n++;
    }
    fclose(f);

    // Add red tiles as intervals of length 1
    for(int i=0;i<n;i++) add_interval(red[i].y,red[i].x,red[i].x);

    // Add green connections between consecutive red tiles (wrap)
    for(int i=0;i<n;i++){
        int j=(i+1)%n;
        int x1=red[i].x, y1=red[i].y;
        int x2=red[j].x, y2=red[j].y;
        if(x1==x2){
            int a=y1,b=y2;if(a>b){int t=a;a=b;b=t;}
            for(int yy=a;yy<=b;yy++) add_interval(yy,x1,x1);
        } else if(y1==y2){
            int a=x1,b=x2;if(a>b){int t=a;a=b;b=t;}
            add_interval(y1,a,b);
        }
    }

    // Compute largest rectangle
    long long best=0;
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            int x1=red[i].x, y1=red[i].y;
            int x2=red[j].x, y2=red[j].y;
            if(x1==x2 || y1==y2) continue;
            int lx=x1<x2?x1:x2, rx=x1>x2?x1:x2;
            int ty=y1<y2?y1:y2, by=y1>y2?y1:y2;
            int ok=1;
            for(int yy=ty;yy<=by && ok;yy++){
                Row *r = get_row(yy);
                if(!covered(r,lx,rx)) ok=0;
            }
            if(ok){
                long long area = (long long)(rx-lx+1)*(long long)(by-ty+1);
                if(area>best) best=area;
            }
        }
    }

    printf("%lld\n",best);

    for(int i=0;i<rows_count;i++) free(rows[i].iv);
    free(rows);
    return 0;
}

//THE CORRECT OUTPUT FOR THE INPUT FILE IS: 1540060480
