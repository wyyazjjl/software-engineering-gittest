#include<iostream>
using namespace std;
typedef long long ll;
ll gcd(ll a, ll b)
{
    return b == 0 ? a : gcd(b, a % b);
}
int main()
{
    ll t;
    cin >> t;
    while (t--)
    {
        ll n, a, d;
        cin >> n >> a >> d;
        if (n == 1)
        {
            cout << -1 << endl;
        }
        else if (gcd(a, d) > 1)
        {
            cout << -1 << endl;
        }
        else if (a % 2 == 0 && n % 2 == 1&&d%2==1)
        {
            cout << -1 << endl;
        }
        else
        {
            if (n % 2 == 0)
            {
                for (ll i = 0; i < n; i++)
                {
                    if (i % 2 == 1)
                        cout << a + (i - 1) * d << " ";
                    else
                        cout << a + (i + 1) * d << " ";
                }
            }
            else
            {
                for (ll i = 0; i < n - 3; i++)
                {
                    if (i % 2 == 1)
                        cout << a + (i - 1) * d << " ";
                    else
                        cout << a + (i + 1) * d << " ";
                }
                cout << a + (n - 2) * d << " ";
                cout << a + (n - 1) * d << " ";
                cout << a + (n - 3) * d << " ";
            }
            cout << endl;
        }
    }
    return 0;
}