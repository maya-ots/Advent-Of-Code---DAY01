#include <stdio.h>
#include <string.h>

#define MAX 3000

char grid[MAX][MAX];
int rows = 0, cols = 0;

int main() {
    FILE *f = fopen("paperRoll.txt", "r");
    if (!f) {
        printf("Error: cannot open paperRoll.txt\n");
        return 1;
    }

    // Read the grid line by line
    while (fgets(grid[rows], MAX, f)) {
        int len = strlen(grid[rows]);
        if (grid[rows][len - 1] == '\n') grid[rows][len - 1] = '\0';
        if (cols == 0) cols = strlen(grid[rows]);
        rows++;
    }
    fclose(f);

    long long accessible = 0;

    // Check each '@'
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {

            if (grid[r][c] != '@') continue;

            int adj = 0;

            // Check 8 neighbors
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {

                    if (dr == 0 && dc == 0) continue;

                    int nr = r + dr;
                    int nc = c + dc;

                    if (nr < 0 || nr >= rows) continue;
                    if (nc < 0 || nc >= cols) continue;

                    if (grid[nr][nc] == '@')
                        adj++;
                }
            }

            if (adj < 4)
                accessible++;
        }
    }

    printf("Accessible rolls: %lld\n", accessible);
    return 0;
}

