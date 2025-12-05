#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

int main() {
    FILE *f = fopen("paperRoll.txt", "r");
    if (!f) {
        perror("Error opening file");
        return 1;
    }

    // Read the grid into a dynamic array
    char **grid = NULL;
    size_t rows = 0;
    size_t cols = 0;
    char buf[MAX_LINE];

    while (fgets(buf, sizeof(buf), f)) {
        // Remove newline characters
        size_t len = strlen(buf);
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
            buf[--len] = '\0';
        }
        if (len == 0) continue; // skip empty lines

        char *line = malloc(len + 1);
        if (!line) { perror("malloc"); return 2; }
        strcpy(line, buf);

        if (cols == 0) cols = len;
        else if (len != cols) {
            fprintf(stderr, "Warning: inconsistent row length at line %zu\n", rows + 1);
        }

        grid = realloc(grid, (rows + 1) * sizeof(char *));
        grid[rows++] = line;
    }
    fclose(f);

    if (rows == 0) {
        printf("No input found.\n");
        return 0;
    }

    long long totalRemoved = 0;
    int dr[8] = {-1,-1,-1, 0,0, 1,1,1};
    int dc[8] = {-1, 0, 1,-1,1,-1,0,1};

    int removedInIteration;
    do {
        removedInIteration = 0;
        int **toRemove = malloc(rows * sizeof(int*));
        for (size_t r = 0; r < rows; ++r) {
            toRemove[r] = calloc(cols, sizeof(int));
        }

        // Mark accessible rolls
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                if (grid[r][c] != '@') continue;
                int adj = 0;
                for (int k = 0; k < 8; ++k) {
                    long nr = (long)r + dr[k];
                    long nc = (long)c + dc[k];
                    if (nr < 0 || nr >= (long)rows || nc < 0 || nc >= (long)cols) continue;
                    if (grid[nr][nc] == '@') ++adj;
                }
                if (adj < 4) {
                    toRemove[r][c] = 1;
                }
            }
        }

        // Remove rolls and count
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                if (toRemove[r][c]) {
                    grid[r][c] = '.';
                    ++totalRemoved;
                    ++removedInIteration;
                }
            }
            free(toRemove[r]);
        }
        free(toRemove);
    } while (removedInIteration > 0);

    printf("Total rolls removed: %lld\n", totalRemoved);

    // Clean up
    for (size_t r = 0; r < rows; ++r) free(grid[r]);
    free(grid);

    return 0;
}
