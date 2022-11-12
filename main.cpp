#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
vector<vector<double>> E(int n){
    vector<vector<double>> result (n);
    for (int i = 0; i<n; i++){
        result[i].resize(n,0);
        result[i][i] = 1;
    }
    return result;
}
vector <vector<double>> orth(int n){
    vector <vector<double>> result(n);
    result[0].push_back(1);
    for(int i = 1; i < n; i++){
        result[i].push_back(0);
    }
    return result;
}
vector<double> getcolumn (vector<vector<double>> A, int k){
    vector<double> result;
    for(int i = 0; i < A.size(); i++){
        result.push_back(A[i][k]);
    }
    return result;
}
vector <vector<double>> msub (vector <vector<double>> A, vector <vector<double>> B){
    vector<vector<double>> result (A.size());
    for (int i = 0; i < result.size(); i++){
        result[i].resize(A[0].size());
        for (int j = 0; j < result[i].size(); j++){
            result[i][j] = A[i][j]-B[i][j];
        }
    }
    return result;
}
double ger_sqrt(double u, double eps){ //square root
    double res = 1, old = 0;
    int k = 0;
    while (abs(res - old) > eps){
        old = res;
        res = 0.5*(old+double(u)/old);
    }
    return res;
}
vector <vector<double>> mnumm(double a, vector <vector<double>> b){
    for (int i = 0; i < b.size(); i++){
        for(int j = 0; j < b[i].size(); j++) {
            b[i][j] = b[i][j]*a;
        }
    }
    return b;
}
vector <vector<double>> mmultiply (vector <vector<double>> A, vector <vector<double>> B){ //matrix multiplication
    if (A[0].size() == B.size()){
        vector <vector<double>> C (A.size());
        for (int i = 0; i < C.size(); i++){
            C[i].resize(B[0].size());
        }
        for (int i = 0; i < A.size(); i++){
            for (int j = 0; j < B[0].size(); j++){
                for (int k = 0; k < A[0].size(); k++){
                    C[i][j] += A[i][k]*B[k][j];
                }
            }
        }
        return C;
    }
}
double vnorm (vector<vector<double>> a){
    double result = 0;
    for (auto i:a){
        result+=i[0]*i[0];
    }
    return ger_sqrt(result, pow(10,-10));
}
void normalize (vector<vector<double>>& a){
    double p = vnorm(a);
    for (int i = 0; i < a.size(); i++){
        a[i][0] /= p;
    }
}
double mnorm(vector<vector<double>> A){
    double result = 0;
    for(int i = 0; i < A[0].size(); i++){
        vector<double> curcol = getcolumn(A,i);
        double colnorm = 0;
        for (auto j:curcol){
            colnorm += j*j;
        }
        result+= ger_sqrt(colnorm,pow(10,-10));
    }
    return result;
}
vector<vector<double>> transpose (vector<vector<double>> A){
    vector<vector<double>> result (A[0].size());
    for(int i = 0; i < result.size(); i++){
        result[i] = getcolumn(A,i);
    }
    return result;
}
vector<vector<double>> getblock(vector<vector<double>> A, int a, int b){
    vector<vector<double>> result(b-a+1);
    for(int i = 0; i < b-a+1; i++){
        result[i].resize(b-a+1);
        for (int j = 0; j < b-a+1; j++){
            result[i][j] = A[i+a][j+a];
        }
    }
    return result;
}
void insertblock(vector<vector<double>>& A, vector<vector<double>>B, int a){
    for (int i = a; i < a + B.size(); i++){
        for (int j = a; j < a + B.size(); j++){
            A[i][j] = B[i-a][j-a];
        }
    }
}
pair<vector<vector<double>>,vector<vector<double>>> LUP (vector <vector<double>> A){
    vector <vector<double>> P(A.size());
    for(int i = 0; i < P.size(); i++){
        P[i].resize(A.size(),0);
        P[i][i] = 1;
    }
    for(int i = 0; i<A.size()-1; i++){
        double lead = INT64_MIN;
        double nlead = -1;
        for (int j = i; j < A.size(); j++){
            if (abs(A[j][i]) > lead){
                lead = abs(A[j][i]);
                nlead = j;
            }
        }
        swap(A[i],A[nlead]);
        swap(P[i],P[nlead]);
        for (int j = i+1; j < A.size(); j++){
            A[j][i] = A[j][i]/A[i][i];
            for (int k = i+1; k<A.size(); k++){
                A[j][k] = A[j][k]-A[j][i]*A[i][k];
            }
        }
    }
    return make_pair(A,P);
}
vector <double> LUPsolve(vector <vector<double>> A, vector<vector<double>> b){
    pair<vector<vector<double>>,vector<vector<double>>> LpUaP = LUP (A);
    vector<vector<double>> LU = LpUaP.first;
    b = mmultiply(LpUaP.second,b);
    vector<double> y(b.size());
    for(int i = 0; i<b.size(); i++){
        y[i] = b[i][0];
    }
    for (int i = 0; i < A.size(); i++){
        for (int k = 0; k<i;k++){
            y[i]-=LU[i][k]*y[k];
        }
    }
    vector<double> x(b.size());
    for(int i = b.size()-1; i>=0; i--){
        x[i] = y[i];
        for (int k = i+1; k<b.size(); k++){
            x[i] -= LU[i][k]*x[k];
        }
        x[i] = x[i]/LU[i][i];
    }
    return x;
}
pair <vector <vector<double>>, vector <vector<double>>> QR (vector <vector<double>> A, vector <vector<double>> b){
    int n = A.size();
    vector <vector<double>> Q = E(n), R = A, w, z, y;
    for (int i = 0; i < n-1; i++){
        y.resize(n-i);
        for(int j = 0; j < y.size(); j++){
            y[j].push_back(R[i+j][i]);
        }
        double a = vnorm(y);
        z = orth(n-i);
        vector<vector<double>> Qi = E(n), Ri;
        w = msub(y, mnumm(a,z));
        normalize(w);
        insertblock(Qi, msub(E(Q.size()-i),mnumm(2, mmultiply(w, transpose(w)))),i);
        Q = mmultiply(Q,Qi);
        Ri = mmultiply(getblock(Qi,i,2), getblock(R,i,2));
        insertblock(R,Ri,i);
        y.resize(0);
    }
    return make_pair(Q,R);
}
int main() {
    int n;
    cin >> n;
    vector <vector<double>> A(n);
    vector <vector<double>> b(n);
    for (int i = 0; i < n; i++){
        A[i].resize(n);
        b[i].resize(1);
        cin >> b[i][0];
    }
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            cin >> A[i][j];
        }
    }
    vector <double> x = LUPsolve(A,b);
    for (int i = 0; i < n; i++){
        cout << "x" << i << " = " << x[i] << endl;
    }
}