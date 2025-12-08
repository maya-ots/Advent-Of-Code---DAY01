#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct { int u, v; unsigned long long d; } Edge;

static int *parent, *size;

int findp(int x) {
    while (x != parent[x]) {
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

void unite(int a, int b) {
    a = findp(a); 
    b = findp(b);
    if (a == b) return;
    if (size[a] < size[b]) { int t = a; a = b; b = t; }
    parent[b] = a;
    size[a] += size[b];
}

int cmp_edge(const void *A, const void *B) {
    const Edge *a = A, *b = B;
    if (a->d < b->d) return -1;
    if (a->d > b->d) return 1;
    return 0;
}

int main() {
    FILE *f = fopen("junctionboxes.txt", "r");
    if (!f) return 1;

    int *X = NULL, *Y = NULL, *Z = NULL;
    int n = 0, cap = 0;
    char line[128];

    while (fgets(line, sizeof(line), f)) {
        int x, y, z;
        if (sscanf(line, "%d,%d,%d", &x, &y, &z) == 3) {
            if (n == cap) {
                cap = cap ? cap * 2 : 256;
                X = realloc(X, cap * sizeof(int));
                Y = realloc(Y, cap * sizeof(int));
                Z = realloc(Z, cap * sizeof(int));
            }
            X[n] = x;
            Y[n] = y;
            Z[n] = z;
            n++;
        }
    }
    fclose(f);

    if (n < 2) return 0;

    unsigned long long total = (unsigned long long)n * (n - 1) / 2ULL;
    Edge *edges = malloc(total * sizeof(Edge));

    unsigned long long idx = 0;
    for (int i = 0; i < n; i++)
        for (int j = i+1; j < n; j++) {
            long long dx = (long long)X[i] - X[j];
            long long dy = (long long)Y[i] - Y[j];
            long long dz = (long long)Z[i] - Z[j];
            unsigned long long d = dx*dx + dy*dy + dz*dz;
            edges[idx].u = i;
            edges[idx].v = j;
            edges[idx].d = d;
            idx++;
        }

    qsort(edges, total, sizeof(Edge), cmp_edge);

    parent = malloc(n * sizeof(int));
    size   = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) { parent[i] = i; size[i] = 1; }

    int components = n;

    for (unsigned long long i = 0; i < total; i++) {
        int a = edges[i].u;
        int b = edges[i].v;
        int pa = findp(a);
        int pb = findp(b);

        if (pa != pb) {
            unite(pa, pb);
            components--;

            if (components == 1) {
                long long result = (long long)X[a] * (long long)X[b];
                printf("%lld\n", result);
                free(edges);
                free(X); free(Y); free(Z);
                free(parent); free(size);
                return 0;
            }
        }
    }

    return 0;
}
