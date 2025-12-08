#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 10000
#define MAX_LEN   10000

int main(void) {
    FILE *f = fopen("tachyon.txt", "r");
    if (!f) {
        perror("Error opening tachyon.txt");
        return 1;
    }

    char *lines[MAX_LINES];
    int line_count = 0;
    char buf[MAX_LEN];

    /* Read all lines */
    while (fgets(buf, sizeof(buf), f)) {
        lines[line_count] = malloc(strlen(buf) + 1);
        if (!lines[line_count]) { perror("malloc"); return 1; }
        strcpy(lines[line_count], buf);
        line_count++;
        if (line_count >= MAX_LINES) break;
    }
    fclose(f);

    if (line_count == 0) {
        printf("0\n");
        return 0;
    }

    /* Remove trailing newlines and find max width */
    int maxw = 0;
    for (int i = 0; i < line_count; ++i) {
        size_t L = strlen(lines[i]);
        if (L > 0 && lines[i][L - 1] == '\n') lines[i][L - 1] = '\0';
        int W = (int)strlen(lines[i]);
        if (W > maxw) maxw = W;
    }

    /* Pad lines to same width with spaces */
    for (int i = 0; i < line_count; ++i) {
        int W = (int)strlen(lines[i]);
        if (W < maxw) {
            char *nstr = malloc(maxw + 1);
            if (!nstr) { perror("malloc"); return 1; }
            strcpy(nstr, lines[i]);
            for (int k = W; k < maxw; ++k) nstr[k] = ' ';
            nstr[maxw] = '\0';
            free(lines[i]);
            lines[i] = nstr;
        }
    }

    /* Find S */
    int sr = -1, sc = -1;
    for (int r = 0; r < line_count && sr == -1; ++r) {
        for (int c = 0; c < maxw; ++c) {
            if (lines[r][c] == 'S') { sr = r; sc = c; break; }
        }
    }
    if (sr == -1) {
        fprintf(stderr, "Start 'S' not found in input\n");
        return 1;
    }

    /* Prepare occupancy arrays for beams (boolean grids) */
    int rows = line_count, cols = maxw;
    /* allocate as 1D arrays for convenience */
    char *cur = calloc(rows * cols, 1);
    char *next = calloc(rows * cols, 1);
    if (!cur || !next) { perror("calloc"); return 1; }

    /* initial beam starts immediately below S, if within bounds */
    if (sr + 1 < rows) cur[(sr + 1) * cols + sc] = 1;

    long long splits = 0;
    int bottom = rows - 1;

    /* Simulation loop: iterate until no active beams remain */
    while (1) {
        /* check if any beam is active */
        int any = 0;
        for (int i = 0; i < rows * cols; ++i) {
            if (cur[i]) { any = 1; break; }
        }
        if (!any) break;

        /* clear next */
        memset(next, 0, rows * cols);

        /* process all active beams */
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (!cur[r * cols + c]) continue;

                /* if beam already at or below bottom row, it exits */
                if (r >= bottom) {
                    /* beam leaves the grid */
                    continue;
                }

                int below_r = r + 1;
                int below_c = c;
                char below = lines[below_r][below_c];

                if (below == '^') {
                    /* split: beam encounters splitter below it */
                    splits++;

                    /* emit into same row as splitter at cols c-1 and c+1 (if in bounds) */
                    int emit_row = below_r;
                    if (below_c - 1 >= 0) next[emit_row * cols + (below_c - 1)] = 1;
                    if (below_c + 1 < cols)  next[emit_row * cols + (below_c + 1)] = 1;

                    /* original beam stops (does not continue down) */
                } else {
                    /* normal: beam moves down into below cell */
                    next[below_r * cols + below_c] = 1;
                }
            }
        }

        /* swap cur and next */
        char *tmp = cur; cur = next; next = tmp;
    }

    printf("%lld\n", splits);

    /* cleanup */
    for (int i = 0; i < line_count; ++i) free(lines[i]);
    free(cur);
    free(next);

    return 0;
}
