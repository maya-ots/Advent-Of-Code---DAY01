#include <stdio.h>
#include <string.h>

#define K 12        // we must pick exactly 12 digits
#define MAX_LEN 300

int main() {
    FILE *f = fopen("batterybanks.txt", "r");
    if (!f) {
        printf("Error: could not open batterybanks.txt\n");
        return 1;
    }

    char line[MAX_LEN];
    long long totalSum = 0;

    while (fgets(line, sizeof(line), f)) {
        // remove newline
        line[strcspn(line, "\n")] = 0;

        int n = strlen(line);
        if (n < K) {
            printf("Error: line too short: %s\n", line);
            continue;
        }

        int removeCount = n - K;         // how many digits we must remove
        char stack[MAX_LEN];
        int top = -1;

        // Greedy monotonic decreasing stack
        for (int i = 0; i < n; i++) {
            char d = line[i];

            while (top >= 0 && stack[top] < d && removeCount > 0) {
                top--;          // pop smaller digit to make room for bigger one
                removeCount--;
            }

            stack[++top] = d;   // push digit
        }

        // If still more digits need to be removed, trim from end
        top = K - 1;

        // Convert 12 digits to a 64-bit number
        long long value = 0;
        for (int i = 0; i < K; i++) {
            value = value * 10 + (stack[i] - '0');
        }

        totalSum += value;
    }

    fclose(f);

    printf("Total joltage sum: %lld\n", totalSum);
    return 0;
}
