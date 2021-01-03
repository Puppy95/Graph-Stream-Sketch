#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "GSS_mod.h"

using namespace std;

GSS *gss;

int main() {
    freopen("tmp.in", "r", stdin);
    freopen("tmp.out", "w", stdout);

    int w = 5;
    int r = 2;
    int p = 9;
    int s = 1;
    int f = 2;

    gss = new GSS(w, r, p, s, f);

    int q;
    cin >> q;
    while (q--) {
        char ty;
        string u, v;
        int w;

        cin >> ty;


        //cerr << ty << endl;
        printf("%c ", ty);
        if (ty == 'i') {
            cin >> u >> v >> w;
            cout << u << ' ' << v << ' ' << w<< endl;
            gss->insert(u, v, w);
        } else if (ty == 'q') {
            cin >> u >> v;
            cout << u << ' ' << v << endl;
            int ret = gss->edgeQuery(u, v);
            int ans;
            scanf("%d", &ans);
            if (ans != ret) {
                printf("FAIL!!!\n");
                printf("ans = %d ret = %d\n", ans, ret);
                exit(0);
            } else {
                printf(" ret = %d\n", ret);
            }
        } else if (ty == 'e') {
            cin >> u >> v;
            cout << u << ' ' << v << endl;
            gss->erase(u, v);
        } else if (ty == 's' || ty == 'p') {
            cin >> u;
            cout << u << endl;
            vector<string> ret, ans;
            int n;
            ret.clear();
            ans.clear();

            if (ty == 's')
                gss->nodeSuccessorQuery(u, ret);
            else
                gss->nodePrecursorQuery(u, ret);

            cin >> n;

            for (int i = 0; i < n; ++i) {
                string a;
                cin >> a;
                ans.push_back(a);
            }
            sort(ans.begin(), ans.end());
            sort(ret.begin(), ret.end());
            if (ret != ans) {
                printf("fail in %s query:\n", ty == 's' ? "successors" : "precursors");

                printf("correct:\n");
                for (int i = 0; i < ans.size(); ++i) {
                    cout << ans[i] << ' ';
                }
                puts("");
                printf("yours:\n");
                for (int i = 0; i < ret.size(); ++i) {
                    cout << ret[i] << ' ';
                }
                puts("");
                exit(0);
            } else {
                for (int i = 0; i < ans.size(); ++i) {
                    cout << ans[i] << ' ';
                }
                puts("");
            }
        }

    }

    return 0;
}
