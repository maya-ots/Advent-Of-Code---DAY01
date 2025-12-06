#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 5000
#define MAX_LEN   5000

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
    char buffer[MAX_LEN];

    // Read all lines from input.txt
    while (fgets(buffer, sizeof(buffer), f)) {
        lines[line_count] = malloc(strlen(buffer) + 1);
        strcpy(lines[line_count], buffer);
        line_count++;
    }
    fclose(f);

    if (line_count == 0) {
        printf("0\n");
        return 0;
    }

    // Remove newline + find max width
    int maxw = 0;
    for (int i = 0; i < line_count; i++) {
        size_t len = strlen(lines[i]);
        if (len && lines[i][len - 1] == '\n')
            lines[i][len - 1] = '\0';
        int L = strlen(lines[i]);
        if (L > maxw) maxw = L;
    }

    // Pad all lines to same width
    for (int i = 0; i < line_count; i++) {
        int L = strlen(lines[i]);
        if (L < maxw) {
            char *newstr = malloc(maxw + 1);
            strcpy(newstr, lines[i]);
            for (int k = L; k < maxw; k++) newstr[k] = ' ';
            newstr[maxw] = '\0';
            free(lines[i]);
            lines[i] = newstr;
        }
    }

    // Identify separator columns (all spaces)
    int *sep = calloc(maxw, sizeof(int));
    for (int c = 0; c < maxw; c++) {
        int all_space = 1;
        for (int r = 0; r < line_count; r++) {
            if (lines[r][c] != ' ') {
                all_space = 0;
                break;
            }
        }
        sep[c] = all_space;
    }

    long long grand_total = 0;
    int bottom = line_count - 1;

    // Process all blocks
    int c = 0;
    while (c < maxw) {
        if (sep[c]) { c++; continue; }

        int start = c;
        while (c < maxw && !sep[c]) c++;
        int end = c;

        // Find operator on last line
        char op = 0;
        for (int k = start; k < end; k++) {
            if (lines[bottom][k] == '+' || lines[bottom][k] == '*') {
                op = lines[bottom][k];
                break;
            }
        }

        long long nums[5000];
        int ncnt = 0;

        // Extract numbers above
        for (int r = 0; r < bottom; r++) {
            char part[MAX_LEN];
            int idx = 0;

            for (int k = start; k < end; k++) {
                if (lines[r][k] != ' ')
                    part[idx++] = lines[r][k];
            }
            part[idx] = '\0';

            if (idx > 0)
                nums[ncnt++] = atoll(part);
        }

        long long result;
        if (op == '+') {
            result = 0;
            for (int i = 0; i < ncnt; i++) result += nums[i];
        } else {
            result = mul_all(nums, ncnt);
        }

        grand_total += result;
    }

    printf("%lld\n", grand_total);

    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(sep);

    return 0;
}
