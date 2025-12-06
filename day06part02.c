#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 6000
#define MAX_LEN   6000

long long mul_all(long long *arr, int n) {
    long long r = 1;
    for (int i = 0; i < n; i++) r *= arr[i];
    return r;
}

int main() {
    FILE *f = fopen("math.txt", "r");
    if (!f) {
        perror("Error opening math.txt");
        return 1;
    }

    char *lines[MAX_LINES];
    int line_count = 0;
    char buf[MAX_LEN];

    // Read all lines
    while (fgets(buf, sizeof(buf), f)) {
        lines[line_count] = malloc(strlen(buf)+1);
        strcpy(lines[line_count], buf);
        line_count++;
    }
    fclose(f);

    // Strip newlines + find max width
    int maxw = 0;
    for (int i = 0; i < line_count; i++) {
        size_t L = strlen(lines[i]);
        if (L && lines[i][L-1] == '\n')
            lines[i][L-1] = '\0';
        int W = strlen(lines[i]);
        if (W > maxw) maxw = W;
    }

    // Pad all lines to equal width
    for (int i = 0; i < line_count; i++) {
        int W = strlen(lines[i]);
        if (W < maxw) {
            char *nstr = malloc(maxw+1);
            strcpy(nstr, lines[i]);
            for (int k = W; k < maxw; k++) nstr[k] = ' ';
            nstr[maxw] = '\0';
            free(lines[i]);
            lines[i] = nstr;
        }
    }

    int bottom = line_count - 1;

    // Identify separator columns
    int *sep = calloc(maxw, sizeof(int));
    for (int c = 0; c < maxw; c++) {
        int allsp = 1;
        for (int r = 0; r < line_count; r++) {
            if (lines[r][c] != ' ') { allsp = 0; break; }
        }
        sep[c] = allsp;
    }

    long long total = 0;

    // Process blocks right-to-left
    int c = maxw - 1;
    while (c >= 0) {

        if (sep[c]) { c--; continue; }

        // find block [start..end]
        int end = c;
        while (c >= 0 && !sep[c]) c--;
        int start = c + 1;

        // operator at bottom row
        char op = 0;
        for (int k = start; k <= end; k++) {
            char ch = lines[bottom][k];
            if (ch == '+' || ch == '*') {
                op = ch;
                break;
            }
        }

        long long numbers[5000];
        int ncnt = 0;

        // For each column (start..end), build a vertical number
        for (int col = end; col >= start; col--) {

            char digits[5000];
            int dcount = 0;

            // read digits top→bottom
            for (int r = 0; r < bottom; r++) {
                char ch = lines[r][col];
                if (isdigit((unsigned char)ch)) {
                    digits[dcount++] = ch;
                }
            }
            digits[dcount] = '\0';

            if (dcount > 0) {
                numbers[ncnt++] = atoll(digits);
            }
        }

        long long result = 0;
        if (op == '+') {
            for (int i = 0; i < ncnt; i++) result += numbers[i];
        } else {
            result = mul_all(numbers, ncnt);
        }

        total += result;
    }

    printf("%lld\n", total);

    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(sep);

    return 0;
}
