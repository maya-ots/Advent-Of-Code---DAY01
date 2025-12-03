#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    FILE *fp = fopen("batterybanks.txt", "r");
    if (!fp) {
        printf("Could not open batterybanks.txt\n");
        return 1;
    }

    char line[5000];
    long long total_sum = 0;
    int line_number = 1;

    while (fgets(line, sizeof(line), fp)) {

        // Remove newline if present
        line[strcspn(line, "\n")] = 0;

        int len = strlen(line);
        int best = -1;

        // Try every ordered pair of digits (i < j)
        for (int i = 0; i < len; i++) {
            if (!isdigit(line[i])) continue;

            for (int j = i + 1; j < len; j++) {
                if (!isdigit(line[j])) continue;

                int first = line[i] - '0';
                int second = line[j] - '0';

                int number = first * 10 + second;

                if (number > best)
                    best = number;
            }
        }

        if (best < 0) best = 0;

        printf("Line %d → %d\n", line_number, best);

        total_sum += best;
        line_number++;
    }

    fclose(fp);

    printf("\nTOTAL SUM = %lld\n", total_sum);

    return 0;
}
