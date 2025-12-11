#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 3000
#define MAXCH 32
#define MAXEDGES 64

typedef struct {
    char name[MAXCH];
    int edges[MAXEDGES];
    int edge_count;
} Node;

Node nodes[MAXN];
int node_count = 0;

unsigned long long memo[MAXN][2][2];

int fft_idx = -1;
int dac_idx = -1;
int out_idx = -1;

int find_node(const char *name) {
    for (int i = 0; i < node_count; i++)
        if (strcmp(nodes[i].name, name) == 0)
            return i;
    strncpy(nodes[node_count].name, name, MAXCH - 1);
    nodes[node_count].name[MAXCH - 1] = '\0';
    nodes[node_count].edge_count = 0;
    return node_count++;
}
unsigned long long dfs_flags(int current, int fft_seen, int dac_seen) {
    int current_fft_seen = fft_seen || (current == fft_idx);
    int current_dac_seen = dac_seen || (current == dac_idx);

    if (current == out_idx) {
        return (current_fft_seen && current_dac_seen) ? 1 : 0;
    }

    if (memo[current][current_fft_seen][current_dac_seen] != -1) {
        return memo[current][current_fft_seen][current_dac_seen];
    }

    unsigned long long total = 0;
    for (int i = 0; i < nodes[current].edge_count; i++) {
        total += dfs_flags(nodes[current].edges[i], current_fft_seen, current_dac_seen);
    }

    return memo[current][current_fft_seen][current_dac_seen] = total;
}

int main() {
    FILE *f = fopen("input.txt", "r");
    if (!f) {
        printf("Cannot open input.txt\n");
        return 1;
    }

    memset(memo, -1, sizeof(memo));

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) < 3) continue;

        char src[MAXCH];
        char *p = strchr(line, ':');
        if (!p) continue;

        *p = '\0';
        
        strncpy(src, line, MAXCH - 1);
        src[MAXCH - 1] = '\0';
        
        int a = find_node(src);

        p++;
        char dest[MAXCH];
        while (sscanf(p, "%31s", dest) == 1) {
            int b = find_node(dest);
            nodes[a].edges[nodes[a].edge_count++] = b;

            char *next = strstr(p, dest);
            if (next) p = next + strlen(dest);
            while (*p && (*p == ' ' || *p == '\t')) p++;
        }
    }
    fclose(f);

    int start = find_node("svr");
    fft_idx = find_node("fft");
    dac_idx = find_node("dac");
    out_idx = find_node("out");

    unsigned long long total_paths = dfs_flags(start, 0, 0);

    printf("%llu\n", total_paths);
    return 0;
}