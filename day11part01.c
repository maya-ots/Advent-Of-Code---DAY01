#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 2000
#define MAX_NAME 50
#define MAX_EDGES 20

typedef struct {
    char name[MAX_NAME];
    int edges[MAX_EDGES];
    int edgeCount;
} Node;

Node graph[MAX_NODES];
int nodeCount = 0;

// Return index of node, create if not found
int getNodeIndex(const char *name) {
    for (int i = 0; i < nodeCount; i++) {
        if (strcmp(graph[i].name, name) == 0)
            return i;
    }
    strcpy(graph[nodeCount].name, name);
    graph[nodeCount].edgeCount = 0;
    return nodeCount++;
}

// DFS count paths
long long dfsCount(int current, int target) {
    if (current == target)
        return 1;

    long long total = 0;
    for (int i = 0; i < graph[current].edgeCount; i++) {
        int nxt = graph[current].edges[i];
        total += dfsCount(nxt, target);
    }
    return total;
}

int main() {
    FILE *f = fopen("path.txt", "r");   // <-- change filename here
    if (!f) {
        printf("Failed to open path.txt file.\n");
        return 1;
    }

    char line[256], device[MAX_NAME], outList[200];

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%[^:]: %[^\n]", device, outList) != 2)
            continue;

        int parent = getNodeIndex(device);

        char *tok = strtok(outList, " ");
        while (tok != NULL) {
            int child = getNodeIndex(tok);
            graph[parent].edges[graph[parent].edgeCount++] = child;
            tok = strtok(NULL, " ");
        }
    }

    fclose(f);

    int start = getNodeIndex("you");
    int end   = getNodeIndex("out");

    long long result = dfsCount(start, end);
    printf("%lld\n", result);

    return 0;
}
