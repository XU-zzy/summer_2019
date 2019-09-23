#include<iostream>
char s[200005],t[200005];
using namespace std;
int main()
{
    int n,i=0,j=1,k=0;
        cin>>n;
    for(i=0;i<n;i++)
        cin>>s[i];
    i=0;
    while(j<n)
    {
        if(s[i]!=s[j])
        {
            t[k]=s[i];
            k++;
            t[k]=s[j];
            k++;
            i=j+1;
            j+=2;
        }
        else
            j++;
    }
    cout<<n-k<<endl;
    for(i=0;i<k;i++)
        cout<<t[i];
    return 0;
}
