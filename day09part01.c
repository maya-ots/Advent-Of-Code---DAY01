#include <stdio.h>
#include <stdlib.h>

typedef struct { int x, y; } Point;

int main(void){
    FILE *f = fopen("input.txt","r");
    if (!f) return 1;
    Point a[200000];
    int n = 0;
    while (fscanf(f,"%d,%d",&a[n].x,&a[n].y) == 2) n++;
    fclose(f);
    long long best = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            long long dx = llabs((long long)a[i].x - a[j].x);
            long long dy = llabs((long long)a[i].y - a[j].y);
            if (dx == 0 || dy == 0) continue;
            long long area = (dx + 1) * (dy + 1);
            if (area > best) best = area;
        }
    }
    printf("%lld\n", best);
    return 0;
}
