#include<iostream>
#include<fstream>
#include<algorithm>
#include<vector>
using namespace std;
typedef long long ll;
vector<ll> v[100005]{};
ll user[1000005];
ll gcd(ll a, ll b)
{
	if (b < 0)
	{
		return 2;
	}
	else
		return b == 0 ? a : gcd(b, a % b);
}
ll judge(ll n, ll a, ll d)
{
	if (n == 1)
		return -1;
	else if (gcd(a, d) > 1)
		return -1;
	else if (a % 2 == 0 && n % 2 == 1&&d % 2 == 1)
		return -1;
	else
		return 1;
}
int main(int cmdline_num, char* cmdline[])//spj�Ƚ�ģʽ
{
	fstream infile(cmdline[1]);
	ll insNum, n, a, d;
	infile >> insNum;
	for (int i = 0; i < insNum; i++)//0��ʼ���Ȼ��ȴ���ȫ��WA
	{
		infile >> n >> a >> d;
		if (judge(n, a, d) == 1)
		{
			for (int j = 0; j < n; j++)
			{
				v[i].push_back(a + j * d);
			}
		}
		else
		{
			v[i].push_back(-1);
		}
	}
	infile.close();//�����룬Ԥ���������漰���ĵȲ����У��޽��ֱ��-1
	fstream infile2(cmdline[3]);
	for (int i = 0; i < insNum; i++)
	{
		ll cur;
		if (v[i][0] == -1)
		{
			infile2 >> cur;
			if (cur != -1)
				return 2;
		}
		else
		{
			for (int j = 0; j < v[i].size(); j++)
			{
				if (infile2.eof()) {
					return 2;
				}
				infile2 >> user[j];
				if (gcd(v[i][j], user[j]) != 1)
				{
					return 2;
				}
			}//gcd�ж�
			sort(user + 0, user + ll(v[i].size()));
			for (int j = 0; j < v[i].size(); j++)
			{
				if (v[i][j] != user[j])
				{
					return 2;
				}
			}//B3-4
		}
	}
	return 0;
}

//C4-4标记