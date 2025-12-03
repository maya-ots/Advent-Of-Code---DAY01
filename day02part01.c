#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_invalid(long long x) {
    char s[32];
    sprintf(s, "%lld", x);

    int len = strlen(s);
    if (len % 2 != 0) return 0;   // must have even length

    int half = len / 2;
    return strncmp(s, s + half, half) == 0;
}

int main() {
    FILE *f = fopen("ranges.txt", "r");
    if (!f) {
        printf("Error: could not open ranges.txt\n");
        return 1;
    }

    char line[5000];
    long long total_sum = 0;

    // Read entire file line by line
    while (fgets(line, sizeof(line), f)) {

        // Break each line into comma-separated ranges
        char *tok = strtok(line, ",");
        while (tok) {
            long long start, end;

            if (sscanf(tok, "%lld-%lld", &start, &end) == 2) {

                // Iterate this range
                for (long long id = start; id <= end; id++) {
                    if (is_invalid(id)) {
                        total_sum += id;
                    }
                }
            }

            tok = strtok(NULL, ",");
        }
    }

    fclose(f);

    printf("Final sum of invalid IDs: %lld\n", total_sum);
    return 0;
}



