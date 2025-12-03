#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_invalid(long long x) {
    char s[32];
    sprintf(s, "%lld", x);
    int len = strlen(s);

    for (int sublen = 1; sublen <= len / 2; sublen++) {
        if (len % sublen != 0) continue; // only lengths that divide evenly
        int repeats = len / sublen;
        int valid = 1;
        for (int i = 0; i < repeats; i++) {
            if (strncmp(s, s + i * sublen, sublen) != 0) {
                valid = 0;
                break;
            }
        }
        if (valid && repeats >= 2) return 1; // repeated at least twice
    }

    return 0;
}

int main() {
    FILE *f = fopen("ranges.txt", "r");
    if (!f) {
        printf("Error: could not open ranges.txt\n");
        return 1;
    }

    char line[5000];
    long long total_sum = 0;

    while (fgets(line, sizeof(line), f)) {
        char *tok = strtok(line, ",");
        while (tok) {
            long long start, end;
            if (sscanf(tok, "%lld-%lld", &start, &end) == 2) {
                for (long long id = start; id <= end; id++) {
                    if (is_invalid(id)) total_sum += id;
                }
            }
            tok = strtok(NULL, ",");
        }
    }

    fclose(f);

    printf("Final sum of invalid IDs: %lld\n", total_sum);
    return 0;
}
