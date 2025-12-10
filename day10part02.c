// solve_part2_astar.c
// Compile: gcc -O2 solve_part2_astar.c -o solve_part2_astar
// Run: ./solve_part2_astar   (will open input..txt if present, else reads stdin)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#define MAX_BUTTONS 128
#define MAX_COUNTERS 20
// If encoded state space product fits in 32-bit, we can use array dist, else use hash map.
#define STATE_LIMIT_ARRAY 40000000U  // allow bigger if memory available
#define EXPLORE_LIMIT 20000000U      // safety cutoff for visited states

typedef struct {
    unsigned int state;   // encoded state id
    int g;                // cost so far (press count)
    int f;                // g + heuristic
} PQNode;

typedef struct {
    PQNode *data;
    int size;
    int cap;
} PriorityQueue;

PriorityQueue *pq_create(int cap) {
    PriorityQueue *q = malloc(sizeof(PriorityQueue));
    q->data = malloc(sizeof(PQNode)*cap);
    q->size = 0; q->cap = cap;
    return q;
}
void pq_push(PriorityQueue *q, unsigned int state, int g, int f) {
    if(q->size >= q->cap){
        q->cap *= 2;
        q->data = realloc(q->data, sizeof(PQNode)*q->cap);
    }
    int i = q->size++;
    q->data[i].state = state; q->data[i].g = g; q->data[i].f = f;
    while(i>0){
        int p = (i-1)/2;
        if(q->data[p].f < q->data[i].f) break; // min-heap by f
        if(q->data[p].f == q->data[i].f && q->data[p].g <= q->data[i].g) break;
        PQNode tmp = q->data[p]; q->data[p] = q->data[i]; q->data[i] = tmp;
        i = p;
    }
}
int pq_empty(PriorityQueue *q){ return q->size == 0; }
PQNode pq_pop(PriorityQueue *q){
    PQNode out = q->data[0];
    q->data[0] = q->data[--q->size];
    int i = 0;
    while(1){
        int l = 2*i+1, r = 2*i+2, s = i;
        if(l < q->size && (q->data[l].f < q->data[s].f ||
           (q->data[l].f == q->data[s].f && q->data[l].g < q->data[s].g))) s = l;
        if(r < q->size && (q->data[r].f < q->data[s].f ||
           (q->data[r].f == q->data[s].f && q->data[r].g < q->data[s].g))) s = r;
        if(s == i) break;
        PQNode tmp = q->data[i]; q->data[i] = q->data[s]; q->data[s] = tmp; i = s;
    }
    return out;
}
void pq_free(PriorityQueue *q){ free(q->data); free(q); }

// ---------------- parsing ----------------
int parse_line_buttons_and_targets(char *line,
    int *ncounters, int *nbuttons, int buttons[][MAX_COUNTERS], int button_sizes[], int targets[])
{
    *ncounters = 0;
    *nbuttons = 0;
    for(int i=0;i<MAX_COUNTERS;i++) targets[i]=0;

    char *pcurly = strchr(line, '{');
    char *qcurly = pcurly ? strchr(pcurly, '}') : NULL;
    if(!pcurly || !qcurly) return 0;

    int idx = 0, num = 0, innum = 0;
    for(char *t = pcurly+1; t < qcurly; ++t){
        if(isdigit((unsigned char)*t)){ num = num*10 + (*t - '0'); innum = 1; }
        else { if(innum){ targets[idx++] = num; num = 0; innum = 0; } }
    }
    if(innum) targets[idx++] = num;
    *ncounters = idx;
    if(*ncounters <= 0) return 0;
    if(*ncounters > MAX_COUNTERS) return 0;

    char *p = line;
    while((p = strchr(p, '('))){
        char *q = strchr(p, ')');
        if(!q || p > pcurly) break;
        int bsize = 0; num = 0; innum = 0;
        for(char *t = p+1; t < q; ++t){
            if(isdigit((unsigned char)*t)){ num = num*10 + (*t - '0'); innum = 1; }
            else { if(innum){ if(num < *ncounters) buttons[*nbuttons][bsize++] = num; num = 0; innum = 0; } }
        }
        if(innum){ if(num < *ncounters) buttons[*nbuttons][bsize++] = num; }
        button_sizes[*nbuttons] = bsize;
        (*nbuttons)++;
        p = q+1;
        if(*nbuttons >= MAX_BUTTONS) break;
    }
    return 1;
}

// ---------- mixed-radix encoding ----------
unsigned long long compute_state_space_size(int ncounters, int targets[]){
    unsigned long long prod = 1;
    for(int i=0;i<ncounters;i++){
        prod *= (unsigned long long)(targets[i] + 1);
        if(prod > ULLONG_MAX/2) return ULLONG_MAX;
    }
    return prod;
}

unsigned int encode_state(int ncounters, int targets[], int state[]){
    unsigned long long id = 0, mul = 1;
    for(int i=0;i<ncounters;i++){
        id += (unsigned long long)state[i] * mul;
        mul *= (unsigned long long)(targets[i] + 1);
    }
    return (unsigned int)id;
}

void decode_state(int ncounters, int targets[], unsigned int id, int state[]){
    unsigned int rem = id;
    for(int i=0;i<ncounters;i++){
        int base = targets[i] + 1;
        state[i] = rem % base;
        rem /= base;
    }
}

// --------- heuristic & greedy ub ----------
int heuristic_lowerbound(int nc, int targets[], int cur[]){
    int maxr = 0;
    long long sumr = 0;
    for(int i=0;i<nc;i++){
        int rem = targets[i] - cur[i];
        if(rem > maxr) maxr = rem;
        sumr += rem;
    }
    // compute max coverage per button will be provided by caller or recomputed externally;
    // for safety here just return maxr (caller will combine)
    if(sumr == 0) return 0;
    return maxr;
}

// greedy upper bound: simple heuristic to get initial UB (press best button repeatedly)
int greedy_upper_bound(int nc, int nbuttons, int buttons[][MAX_COUNTERS], int button_sizes[], int targets[]){
    int cur[MAX_COUNTERS] = {0};
    int presses = 0;
    int limit = 10000000;
    while(1){
        int done = 1;
        for(int i=0;i<nc;i++) if(cur[i] < targets[i]) { done = 0; break; }
        if(done) return presses;
        // select button that improves remaining the most (counts how many counters it will increment that still need increments)
        int best = -1, bestgain = 0;
        for(int b=0;b<nbuttons;b++){
            int gain = 0;
            for(int k=0;k<button_sizes[b];k++){
                int idx = buttons[b][k];
                if(cur[idx] < targets[idx]) gain++;
            }
            if(gain > bestgain){ bestgain = gain; best = b; }
        }
        if(bestgain == 0) return INT_MAX; // stuck/unreachable greedily
        // press that button once (greedy)
        for(int k=0;k<button_sizes[best];k++){
            int idx = buttons[best][k];
            if(cur[idx] < targets[idx]) cur[idx]++;
        }
        presses++;
        if(presses > limit) return INT_MAX;
    }
}

// ---------- A* search ----------
int solve_machine_astar(int ncounters, int nbuttons,
    int buttons[][MAX_COUNTERS], int button_sizes[], int targets[])
{
    unsigned long long space = compute_state_space_size(ncounters, targets);
    if(space == 0) return -1;
    if(space > (unsigned long long)UINT_MAX) {
        // we can still encode states into 32-bit id if caller expects; but if product exceeds 32-bit we'll still try
        // but we guard against huge expansions by EXPLORE_LIMIT below.
    }

    // precompute max coverage for heuristic
    int max_cov = 0;
    for(int i=0;i<nbuttons;i++) if(button_sizes[i] > max_cov) max_cov = button_sizes[i];
    if(max_cov <= 0) return -1;

    // initial greedy UB to help prune
    int greedyUB = greedy_upper_bound(ncounters, nbuttons, buttons, button_sizes, targets);
    if(greedyUB == INT_MAX) greedyUB = INT_MAX/4;

    // choose data structure for dist: use hash map if space big, else array
    unsigned int statesz = (space <= STATE_LIMIT_ARRAY) ? (unsigned int)space : 0;

    int *distArr = NULL;
    // sentinel INF
    const int INF = INT_MAX/4;
    if(statesz) {
        distArr = malloc(sizeof(int) * statesz);
        if(!distArr) statesz = 0;
        else for(unsigned int i=0;i<statesz;i++) distArr[i] = INF;
    }

    // visited hash map if needed (open addressing)
    // We will only create hash map if statesz==0
    unsigned int *hash_key = NULL;
    int *hash_val = NULL;
    unsigned int hash_size = 0;
    if(!statesz){
        // pick a hash size (power of two) based on min(space, EXPLORE_LIMIT)
        unsigned long long want = space;
        if(want == ULLONG_MAX || want > EXPLORE_LIMIT) want = EXPLORE_LIMIT;
        // ensure a power of two > 2*want
        hash_size = 1;
        while(hash_size < (unsigned int)(want * 2 + 10)) hash_size <<= 1;
        hash_key = malloc(sizeof(unsigned int) * hash_size);
        hash_val = malloc(sizeof(int) * hash_size);
        if(!hash_key || !hash_val){
            if(hash_key) free(hash_key);
            if(hash_val) free(hash_val);
            return -2;
        }
        for(unsigned int i=0;i<hash_size;i++){ hash_key[i] = 0xFFFFFFFFu; hash_val[i] = INF; }
    }

    PriorityQueue *pq = pq_create(4096);

    int startstate[MAX_COUNTERS] = {0};
    unsigned int start_id = encode_state(ncounters, targets, startstate);
    // heuristic for start:
    int h_start = 0;
    long long sumr = 0;
    for(int i=0;i<ncounters;i++) sumr += targets[i];
    int h1 = 0;
    for(int i=0;i<ncounters;i++) if(targets[i] > h1) h1 = targets[i];
    int h2 = (int)((sumr + max_cov - 1) / max_cov);
    h_start = (h1 > h2) ? h1 : h2;

    // set dist[start] = 0
    if(statesz) distArr[start_id] = 0;
    else {
        // insert into hash
        unsigned int key = start_id;
        unsigned int idx = key & (hash_size - 1);
        while(hash_key[idx] != 0xFFFFFFFFu && hash_key[idx] != key) idx = (idx + 1) & (hash_size - 1);
        hash_key[idx] = key; hash_val[idx] = 0;
    }

    pq_push(pq, start_id, 0, h_start);

    unsigned int visited_count = 0;
    unsigned int goal_id;
    {
        int goal_state[MAX_COUNTERS];
        for(int i=0;i<ncounters;i++) goal_state[i] = targets[i];
        goal_id = encode_state(ncounters, targets, goal_state);
    }

    while(!pq_empty(pq)){
        PQNode cur = pq_pop(pq);
        unsigned int sid = cur.state;
        int g = cur.g;
        // check dist
        int known = INF;
        if(statesz){
            known = distArr[sid];
        } else {
            unsigned int key = sid;
            unsigned int idx = key & (hash_size - 1);
            while(hash_key[idx] != 0xFFFFFFFFu && hash_key[idx] != key) idx = (idx + 1) & (hash_size - 1);
            if(hash_key[idx] == key) known = hash_val[idx];
            else known = INF;
        }
        if(g != known) continue; // outdated
        if(sid == goal_id){
            // found optimal
            int ans = g;
            if(distArr) free(distArr);
            if(hash_key) { free(hash_key); free(hash_val); }
            pq_free(pq);
            return ans;
        }

        // safety cutoff
        if(++visited_count > EXPLORE_LIMIT){
            if(distArr) free(distArr);
            if(hash_key) { free(hash_key); free(hash_val); }
            pq_free(pq);
            return -2; // exploring too many nodes
        }

        // decode state
        int curstate[MAX_COUNTERS];
        decode_state(ncounters, targets, sid, curstate);

        // compute heuristic components common to all successors:
        int max_rem = 0; long long sum_rem = 0;
        for(int i=0;i<ncounters;i++){
            int r = targets[i] - curstate[i];
            if(r > max_rem) max_rem = r;
            sum_rem += r;
        }

        for(int b=0;b<nbuttons;b++){
            // compute new state by adding button once; check validity
            int ok = 1;
            int newstate[MAX_COUNTERS];
            memcpy(newstate, curstate, sizeof(int)*ncounters);
            for(int k=0;k<button_sizes[b];k++){
                int idx = buttons[b][k];
                newstate[idx]++;
                if(newstate[idx] > targets[idx]) { ok = 0; break; }
            }
            if(!ok) continue;

            unsigned int nid = encode_state(ncounters, targets, newstate);
            int ng = g + 1;

            // lower bound heuristic for newstate:
            int new_max_rem = 0; long long new_sum_rem = 0;
            for(int i=0;i<ncounters;i++){
                int r = targets[i] - newstate[i];
                if(r > new_max_rem) new_max_rem = r;
                new_sum_rem += r;
            }
            int h1n = new_max_rem;
            int h2n = (int)((new_sum_rem + max_cov - 1) / max_cov);
            int hn = (h1n > h2n) ? h1n : h2n;
            int nf = ng + hn;

            // if we already have a better dist, skip
            int prev = INF;
            if(statesz) prev = distArr[nid];
            else {
                unsigned int key = nid;
                unsigned int idxh = key & (hash_size - 1);
                while(hash_key[idxh] != 0xFFFFFFFFu && hash_key[idxh] != key) idxh = (idxh + 1) & (hash_size - 1);
                if(hash_key[idxh] == key) prev = hash_val[idxh];
            }
            if(ng < prev){
                if(statesz) distArr[nid] = ng;
                else {
                    unsigned int key = nid;
                    unsigned int idxh = key & (hash_size - 1);
                    while(hash_key[idxh] != 0xFFFFFFFFu && hash_key[idxh] != key) idxh = (idxh + 1) & (hash_size - 1);
                    hash_key[idxh] = key; hash_val[idxh] = ng;
                }
                // pruning by greedyUB if exists:
                if(greedyUB < INT_MAX/4 && ng + hn > greedyUB) {
                    // skip pushing since cannot beat current greedy UB
                } else {
                    pq_push(pq, nid, ng, nf);
                }
            }
        }
    }

    if(distArr) free(distArr);
    if(hash_key) { free(hash_key); free(hash_val); }
    pq_free(pq);
    return -1; // unreachable
}

int main(void){
    FILE *f = fopen("input.txt", "r");
    if(!f) f = stdin;
    char line[4096];
    long long total = 0;
    int line_no = 0;
    while(fgets(line, sizeof(line), f)){
        line_no++;
        int L = strlen(line);
        while(L>0 && (line[L-1]=='\n' || line[L-1]=='\r')) line[--L] = 0;
        if(L==0) continue;

        int ncounters = 0, nbuttons = 0;
        int buttons[MAX_BUTTONS][MAX_COUNTERS];
        int button_sizes[MAX_BUTTONS];
        int targets[MAX_COUNTERS];
        if(!parse_line_buttons_and_targets(line, &ncounters, &nbuttons, buttons, button_sizes, targets)){
            fprintf(stderr, "Warning: could not parse line %d, skipping\n", line_no);
            continue;
        }

        // quick impossible check: if some counter has no button affecting it but target>0 -> impossible
        for(int i=0;i<ncounters;i++){
            int any = 0;
            for(int b=0;b<nbuttons;b++){
                for(int k=0;k<button_sizes[b];k++) if(buttons[b][k] == i){ any = 1; break; }
                if(any) break;
            }
            if(!any && targets[i] > 0){
                printf("IMPOSSIBLE\n");
                if(f != stdin) fclose(f);
                return 0;
            }
        }

        int ans = solve_machine_astar(ncounters, nbuttons, buttons, button_sizes, targets);
        if(ans == -2){
            // exploration limit / memory problem
            fprintf(stderr, "Line %d: search exploded or memory insufficient. Aborting.\n", line_no);
            printf("IMPOSSIBLE\n");
            if(f != stdin) fclose(f);
            return 2;
        }
        if(ans == -1){
            printf("IMPOSSIBLE\n");
            if(f != stdin) fclose(f);
            return 0;
        }
        total += ans;
    }
    if(f != stdin) fclose(f);
    printf("%lld\n", total);
    return 0;
}

