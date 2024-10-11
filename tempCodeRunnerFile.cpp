#include<bits/stdc++.h>

using namespace std;

const int N=1010;

const int L=5,R=10; //禁忌步长
int tabu[N];//禁忌表
int w[N];
int dis[50][50];
int ans;
int n;
int fun(){  //评价函数
    int res=0;
    for(int i=0;i<n-1;i++)
        res+=dis[w[i]][w[i+1]];
    return res;
}

//禁忌搜索框架
void solve(){
    memset(tabu,0,sizeof(tabu));
    random_shuffle(w+1,w+n-1); //打乱位置
    //领域移动
    for(int t=1;t<=50;t++){
        int tmp=INT_MAX;
        int nx,ny;
        for(int i=1;i<n-1;i++)
            for(int j=i+1;j<n-1;j++){
                if(tabu[i*20+j]>t) continue; //在禁忌步长内会失效，对象(i,j)的禁忌在多次迭代后失效
                swap(w[i],w[j]); //交换节点
                int now=fun();
                if(now<tmp){
                    tmp=now;
                    nx=i,ny=j;
                }
                swap(w[i],w[j]); //回溯
            }
        if(tmp!=INT_MAX){
            tabu[nx*15+ny]=t+rand()%(R-L+1)+L; //放入禁忌表中
            ans=min(ans,tmp); //更新当前解
            swap(w[nx],w[ny]);
        }
    }
}

int main(){
    cin>>n;
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) cin>>dis[i][j];
    for(int i=0;i<n;i++) w[i]=i; //对应节点的下标
    ans=fun();//随便去一个可行解
    //迭代多次取最优解
    for(int i=0;i<500;i++) solve();
    cout<<ans<<endl;
    return 0;
}