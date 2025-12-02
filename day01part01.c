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
    int zeroCount = 0;     // how many times we land on 0

    // read until EOF
    while (fscanf(f, " %c%d", &rot, &val) == 2) {

        if (rot == 'L') {
            pos = (pos - val) % 100;
            if (pos < 0) pos += 100;
        }
        else if (rot == 'R') {
            pos = (pos + val) % 100;
        }

        if (pos == 0)
            zeroCount++;
    }

    fclose(f);

    printf("Final count of landing on 0: %d\n", zeroCount);
    return 0;
}
