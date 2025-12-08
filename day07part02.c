#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXR 5000
#define MAXC 5000

char grid[MAXR][MAXC];
long long memo[MAXR][MAXC];
int R, C;

long long dfs(int r, int c) {
    if (r < 0 || r >= R || c < 0 || c >= C) return 1;
    if (memo[r][c] != -1) return memo[r][c];

    if (grid[r][c] == '^') {
        long long L = dfs(r, c - 1);
        long long Rv = dfs(r, c + 1);
        memo[r][c] = L + Rv;
        return memo[r][c];
    }

    memo[r][c] = dfs(r + 1, c);
    return memo[r][c];
}

int main() {
    FILE *f = fopen("tachyon.txt", "r");
    if (!f) return 1;

    char line[6000];
    R = 0;
    C = 0;

    while (fgets(line, sizeof(line), f)) {
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = 0;
        if (len > C) C = len;

        for (int i = 0; i < len; i++) {
            char c = line[i];
            if (c == '^' || c == 'S') grid[R][i] = c;
            else grid[R][i] = '.';
        }
        for (int i = len; i < MAXC; i++)
            grid[R][i] = '.';

        R++;
    }
    fclose(f);

    int sr = -1, sc = -1;
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            if (grid[r][c] == 'S') { sr = r; sc = c; }

    if (sr == -1 || sc == -1) {
        printf("0\n");
        return 0;
    }

    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            memo[i][j] = -1;

    long long ans = dfs(sr + 1, sc);
    printf("%lld\n", ans);

    return 0;
}


//THIS IS THE CORRECT AND FINAL CODE
