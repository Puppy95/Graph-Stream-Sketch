#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <set>

using namespace std;
int randint(int l, int r) {
    return rand() % (r - l + 1) + l;
}

typedef pair<int, int> pii;

int g[1001][1001];
int n = 20;

set<pii > s;

pii randEdge() {
    assert(s.empty() == false);
    int t = randint(1, s.size());
    int i = 1;
    for (set<pii> ::iterator it = s.begin(); it != s.end(); ++it) {
        if (i == t) {

            return *it;
        }
        ++i;
    }
    return make_pair(-1, -1);
}

int main() {
    freopen("tmp.in", "w", stdout);

    srand(123);

    int q = randint(500, 1000);

    printf("%d\n", q);
    while(q--) {
        int ty = randint(0, 99);
        int subty = randint(0, 2);

        if (ty < 60) {
            ty = 0;
        } else if (ty < 70) {
            ty = 1;
        } else if (ty < 100) {
            ty = 2;
        }

        if (s.size() == 0) {
            ty = 0;
        }

        //printf("size : %d ty = %d\n", s.size(), ty);

        int u, v, w;
        pii tmp_edge;

        if (ty == 0) {
            u = randint(1, n);
            v = randint(1, n);
            w = randint(1, 1);
            g[u][v] += w;
            s.insert(make_pair(u, v));
            printf("i %d %d %d\n", u, v, w);

        } else if (ty == 1) {
            tmp_edge = randEdge();
            s.erase(tmp_edge);
            g[tmp_edge.first][tmp_edge.second] = 0;
            printf("e %d %d\n", tmp_edge.first, tmp_edge.second);

        } else if (ty == 2) {
            //subty = 0;
            if (subty == 0) {
                tmp_edge = randEdge();
                printf("q %d %d\n", tmp_edge.first, tmp_edge.second);
                printf("%d\n", g[tmp_edge.first][tmp_edge.second]);
            } else if (subty == 1) {
                int u = randint(1, n);
                int cnt = 0;

                printf("s %d\n", u);

                for (int i = 1; i <= n; ++i)
                    cnt += (g[u][i] != 0);

                printf("%d ", cnt);
                for (int i = 1; i <= n; ++i) {
                    if (g[u][i]) {
                        printf("%d ", i);
                    }
                }
                puts("");
            } else {
                int u = randint(1, n);
                int cnt = 0;

                printf("p %d\n", u);

                for (int i = 1; i <= n; ++i)
                    cnt += (g[i][u] != 0);

                printf("%d ", cnt);
                for (int i = 1; i <= n; ++i) {
                    if (g[i][u]) {
                        printf("%d ", i);
                    }
                }

                puts("");
            }
        }
    }

    return 0;
}
