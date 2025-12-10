// solve_openfile.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXB 25
#define MAXL 64

// Parse one machine line
int parse_line(char *line, int *nLights, unsigned int *targetMask,
                int *nButtons, unsigned int buttonMask[]) {
    *nLights = 0;
    *targetMask = 0;
    *nButtons = 0;

    char *p = strchr(line, '[');
    char *q = strchr(line, ']');
    if (!p || !q) return 0;

    for (char *t = p + 1; t < q; ++t) {
        if (*t == '.' || *t == '#') {
            if (*t == '#') *targetMask |= (1u << (*nLights));
            (*nLights)++;
            if (*nLights >= MAXL) return 0;
        }
    }

    p = q + 1;
    while ((p = strchr(p, '('))) {
        q = strchr(p, ')');
        if (!q) break;

        unsigned int mask = 0;
        int num = 0;
        int inNum = 0;
        for (char *t = p + 1; t < q; ++t) {
            if (isdigit((unsigned char)*t)) {
                num = num * 10 + (*t - '0');
                inNum = 1;
            } else {
                if (inNum) {
                    if (num < 32) mask |= 1u << num; // assume lights < 32
                    num = 0;
                    inNum = 0;
                }
            }
        }
        if (inNum) {
            if (num < 32) mask |= 1u << num;
        }

        if (*nButtons < MAXB) {
            buttonMask[(*nButtons)++] = mask;
        }
        p = q + 1;
    }

    return 1;
}

// Compute minimal presses for one machine (bruteforce)
int solve_machine(int nLights, unsigned int targetMask,
                  int nButtons, unsigned int buttonMask[]) {
    if (nButtons >= 31) {
        // too many buttons for simple bitmask brute force in this simple solution
        return -2;
    }

    int best = 1e9;
    int total = 1 << nButtons;

    for (int s = 0; s < total; s++) {
        unsigned int cur = 0;
        int bits = 0;
        for (int i = 0; i < nButtons; i++) {
            if (s & (1 << i)) {
                cur ^= buttonMask[i];
                bits++;
            }
        }
        if (cur == targetMask && bits < best)
            best = bits;
    }

    if (best == (int)1e9) return -1; // unsolvable
    return best;
}

int main(void) {
    FILE *f = fopen("input.txt", "r");
    if (!f) {
        // fallback to stdin
        f = stdin;
    }

    char line[1024];
    long long totalPresses = 0;

    while (fgets(line, sizeof(line), f)) {
        // skip empty/short lines
        int len = (int)strlen(line);
        if (len < 3) continue;

        int nLights = 0, nButtons = 0;
        unsigned int targetMask = 0;
        unsigned int buttonMask[MAXB] = {0};

        if (!parse_line(line, &nLights, &targetMask, &nButtons, buttonMask)) {
            // couldn't parse - ignore
            continue;
        }

        int best = solve_machine(nLights, targetMask, nButtons, buttonMask);
        if (best == -2) {
            fprintf(stderr, "Too many buttons to brute force (>=31). Exiting.\n");
            return 2;
        }
        if (best == -1) {
            printf("IMPOSSIBLE\n");
            return 0;
        }
        totalPresses += best;
    }

    if (f != stdin) fclose(f);

    printf("%lld\n", totalPresses);
    return 0;
}
