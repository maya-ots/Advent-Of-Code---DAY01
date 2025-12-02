#include <stdio.h>

int main() {
    FILE *f = fopen("list.txt", "r");
    if (!f) {
        printf("Error: could not open list.txt\n");
        return 1;
    }

    char rot;
    int val;
    int pos = 50;          // starting position
    int totalZeros = 0;    // counts zeros landed on or passed by

    while (fscanf(f, " %c%d", &rot, &val) == 2) {
        if (rot == 'L') {
            for (int i = 0; i < val; i++) {
                pos = (pos - 1 + 100) % 100;
                if (pos == 0) totalZeros++;
            }
        } else if (rot == 'R') {
            for (int i = 0; i < val; i++) {
                pos = (pos + 1) % 100;
                if (pos == 0) totalZeros++;
            }
        }
    }

    fclose(f);

    printf("Total zeros passed or landed on: %d\n", totalZeros);

    return 0;
}

