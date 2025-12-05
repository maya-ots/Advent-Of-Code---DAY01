#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    unsigned long long start;
    unsigned long long end;
} Range;

// Compare function for qsort
int compareRanges(const void *a, const void *b) {
    Range *r1 = (Range *)a;
    Range *r2 = (Range *)b;
    if (r1->start < r2->start) return -1;
    else if (r1->start > r2->start) return 1;
    else return 0;
}

int main() {
    FILE *file = fopen("ingredientID.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    Range ranges[1000];
    int range_count = 0;
    char line[100];

    // Read ranges until blank line
    while (fgets(line, sizeof(line), file)) {
        unsigned long long start, end;

        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Blank line → stop reading ranges
        if (line[0] == '\0') break;

        // Parse ranges with or without spaces around '-'
        if (sscanf(line, "%llu-%llu", &start, &end) < 2 &&
            sscanf(line, "%llu - %llu", &start, &end) < 2) {
            fprintf(stderr, "Invalid range format: %s\n", line);
            return 1;
        }

        ranges[range_count].start = start;
        ranges[range_count].end = end;
        range_count++;
    }

    fclose(file);

    // Sort ranges by start
    qsort(ranges, range_count, sizeof(Range), compareRanges);

    // Merge overlapping ranges and count unique IDs
    unsigned long long total_fresh = 0;
    if (range_count > 0) {
        unsigned long long current_start = ranges[0].start;
        unsigned long long current_end = ranges[0].end;

        for (int i = 1; i < range_count; i++) {
            if (ranges[i].start <= current_end + 1) {
                // Overlapping or contiguous range → extend current_end
                if (ranges[i].end > current_end)
                    current_end = ranges[i].end;
            } else {
                // Non-overlapping → add current range length
                total_fresh += current_end - current_start + 1;
                current_start = ranges[i].start;
                current_end = ranges[i].end;
            }
        }
        // Add last range
        total_fresh += current_end - current_start + 1;
    }

    printf("Total unique fresh ingredient IDs: %llu\n", total_fresh);

    return 0;
}
