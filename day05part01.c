#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#define MAX_RANGES 1000
#define MAX_IDS 10000
#define LINE_LEN 100

typedef struct {
    uint64_t start;
    uint64_t end;
} Range;

int main() {
    FILE *file = fopen("ingredientID.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    char line[LINE_LEN];
    Range ranges[MAX_RANGES];
    int range_count = 0;

    uint64_t available_ids[MAX_IDS];
    int id_count = 0;

    int reading_ranges = 1;

    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Blank line separates ranges from available IDs
        if (strlen(line) == 0) {
            reading_ranges = 0;
            continue;
        }

        if (reading_ranges) {
            // Parse a range like "3-5"
            if (sscanf(line, "%" SCNu64 "-%" SCNu64, &ranges[range_count].start, &ranges[range_count].end) == 2) {
                range_count++;
            } else {
                fprintf(stderr, "Invalid range format: %s\n", line);
                return 1;
            }
        } else {
            // Parse available IDs
            if (sscanf(line, "%" SCNu64, &available_ids[id_count]) == 1) {
                id_count++;
            } else {
                fprintf(stderr, "Invalid ID format: %s\n", line);
                return 1;
            }
        }
    }

    fclose(file);

    // Count fresh IDs
    int fresh_count = 0;
    for (int i = 0; i < id_count; i++) {
        uint64_t id = available_ids[i];
        for (int j = 0; j < range_count; j++) {
            if (id >= ranges[j].start && id <= ranges[j].end) {
                fresh_count++;
                break;  // no need to check other ranges
            }
        }
    }

    printf("Number of fresh ingredients: %d\n", fresh_count);

    return 0;
}
