#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>

using namespace std;

int main()
{
    int n, m, l;
    cin >> n >> m >> l;
    int a [m][l];    
    int b [n][l];
    for(int i=0; i<n; i++) {
        for(int j=0; j<l; j++) {
            cin >> a[i][j];
        }
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<l; j++){
            cin >> b[i][j];
        }
    }
// kjk
    int q, ia, ib;
    int minx, maxx, indmin;
    cin >> q;
    while(q){
        cin >> ia >> ib;
        minx = pow(10,5)-1
        for(int j=0; j<l; j++){
            maxx = max(a[ia][j], 
                b[ib][j]);
            if (minx > maxx){
                minx = maxx;
                indmin = j;
            }

        }
        cout <<  indmin <<'\n';
        q--;
    }


    return 0;
}
