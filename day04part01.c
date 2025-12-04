#include <stdio.h>
#include <string.h>

#define MAXR 3000
#define MAXC 3000

int main() {
    FILE *f = fopen("paperRoll.txt", "r");
    if (!f) {
        printf("Error: cannot open paperRoll.txt\n");
        return 1;
    }

    char grid[MAXR][MAXC];
    int rows = 0, cols = 0;

    // Read the grid line-by-line
    while (rows < MAXR && fgets(grid[rows], MAXC, f)) {
        int L = strlen(grid[rows]);
        if (L > 0 && (grid[rows][L-1] == '\n' || grid[rows][L-1] == '\r'))
            grid[rows][--L] = '\0';

        if (cols == 0) cols = L;
        rows++;
    }
    fclose(f);

    // Directions for 8 neighbors
    int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};

    long long accessible = 0;

    // OUT GRID (copy)
    char out[MAXR][MAXC];
    for (int r = 0; r < rows; r++)
        strcpy(out[r], grid[r]);

    // Check all '@'
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r][c] != '@') continue;

            int adj = 0;

            for (int k = 0; k < 8; k++) {
                int nr = r + dr[k];
                int nc = c + dc[k];

                if (nr < 0 || nr >= rows) continue;
                if (nc < 0 || nc >= cols) continue;

                if (grid[nr][nc] == '@')
                    adj++;
            }

            if (adj < 4) {
                accessible++;
                out[r][c] = 'x';
            }
        }
    }

    printf("Accessible rolls: %lld\n\n", accessible);

    // Print marked grid
    for (int r = 0; r < rows; r++) {
        printf("%s\n", out[r]);
    }

    return 0;
}
