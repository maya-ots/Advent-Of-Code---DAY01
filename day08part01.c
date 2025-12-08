#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    int u, v;
    unsigned long long d;
} Edge;

static int *parent;
static int *comp_size;

int findp(int x) {
    int r = x;
    while (parent[r] != r) r = parent[r];
    while (x != r) {
        int nxt = parent[x];
        parent[x] = r;
        x = nxt;
    }
    return r;
}

void unite(int a, int b) {
    a = findp(a); b = findp(b);
    if (a == b) return;
    if (comp_size[a] < comp_size[b]) { int t = a; a = b; b = t; }
    parent[b] = a;
    comp_size[a] += comp_size[b];
}

int cmp_edge(const void *A, const void *B) {
    const Edge *a = (const Edge*)A;
    const Edge *b = (const Edge*)B;
    if (a->d < b->d) return -1;
    if (a->d > b->d) return 1;
    return 0;
}

int cmp_int_desc(const void *A, const void *B) {
    const int a = *(const int*)A;
    const int b = *(const int*)B;
    if (a > b) return -1;
    if (a < b) return 1;
    return 0;
}

int main(void) {
    FILE *f = fopen("junctionboxes.txt", "r");
    if (!f) {
        fprintf(stderr, "Cannot open junctionboxes.txt\n");
        return 1;
    }

    int *xs = NULL, *ys = NULL, *zs = NULL;
    int n = 0, cap = 0;
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        int x, y, z;
        if (sscanf(line, " %d , %d , %d", &x, &y, &z) == 3) {
            if (n == cap) {
                int newcap = cap ? cap * 2 : 256;
                int *tx = realloc(xs, newcap * sizeof(int));
                int *ty = realloc(ys, newcap * sizeof(int));
                int *tz = realloc(zs, newcap * sizeof(int));
                if (!tx || !ty || !tz) { fprintf(stderr, "alloc fail\n"); return 1; }
                xs = tx; ys = ty; zs = tz;
                cap = newcap;
            }
            xs[n] = x; ys[n] = y; zs[n] = z; n++;
        }
    }
    fclose(f);

    if (n <= 0) {
        printf("0\n");
        free(xs); free(ys); free(zs);
        return 0;
    }

    unsigned long long total_pairs = (unsigned long long)n * (unsigned long long)(n - 1) / 2ULL;
    Edge *edges = malloc((size_t)total_pairs * sizeof(Edge));
    if (!edges) {
        fprintf(stderr, "Not enough memory to store %llu edges\n", total_pairs);
        free(xs); free(ys); free(zs);
        return 1;
    }

    unsigned long long idx = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            long long dx = (long long)xs[i] - (long long)xs[j];
            long long dy = (long long)ys[i] - (long long)ys[j];
            long long dz = (long long)zs[i] - (long long)zs[j];
            unsigned long long dist = (unsigned long long)(dx * dx) + (unsigned long long)(dy * dy) + (unsigned long long)(dz * dz);
            edges[idx].u = i;
            edges[idx].v = j;
            edges[idx].d = dist;
            idx++;
        }
    }

    qsort(edges, (size_t)total_pairs, sizeof(Edge), cmp_edge);

    parent = malloc(n * sizeof(int));
    comp_size = malloc(n * sizeof(int));
    if (!parent || !comp_size) { fprintf(stderr, "alloc fail\n"); return 1; }
    for (int i = 0; i < n; i++) { parent[i] = i; comp_size[i] = 1; }

    unsigned long long take = 1000ULL;
    if (take > total_pairs) take = total_pairs;

    for (unsigned long long e = 0; e < take; ++e) {
        unite(edges[e].u, edges[e].v);
    }

    for (int i = 0; i < n; i++) parent[i] = findp(i);

    int *map_index = calloc(n, sizeof(int));
    int *sizes = malloc(n * sizeof(int));
    if (!map_index || !sizes) { fprintf(stderr, "alloc fail\n"); return 1; }
    for (int i = 0; i < n; i++) map_index[i] = -1;

    int map_count = 0;
    for (int i = 0; i < n; i++) {
        int r = parent[i];
        if (map_index[r] == -1) {
            map_index[r] = map_count;
            sizes[map_count] = 0;
            map_count++;
        }
        sizes[ map_index[r] ]++;
    }

    qsort(sizes, map_count, sizeof(int), cmp_int_desc);

    unsigned long long result = 1;
    for (int i = 0; i < 3 && i < map_count; ++i) result *= (unsigned long long)sizes[i];

    printf("%llu\n", result);

    free(edges);
    free(xs); free(ys); free(zs);
    free(parent); free(comp_size);
    free(map_index); free(sizes);
    return 0;
}
