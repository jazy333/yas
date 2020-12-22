#include <cstring>
#include "algorithm.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

int LevenshteinDistance(const string &a, const string &b)
{

	int alen = a.size();
	int blen = b.size();
	if (alen == 0)
	{
		return blen;
	}

	if (blen == 0)
	{
		return alen;
	}

	vector<vector<int>> matrix;
	matrix.resize(alen + 1);
	for (int i = 0; i < alen + 1; ++i)
	{
		matrix[i].resize(blen + 1, 0);
		for (int j = 0; j < blen + 1; j++)
		{
			if (i == 0)
			{
				matrix[i][j] = j;
			}
			else if (j == 0)
			{
				matrix[i][j] = i;
			}
		}
	}

	for (int i = 1; i <= alen; ++i)
	{
		for (int j = 1; j <= blen; ++j)
		{
			int d1 = matrix[i - 1][j] + 1;
			int d2 = matrix[i][j - 1] + 1;
			int d3 = (a[i - 1] == b[j - 1]) ? matrix[i - 1][j - 1] : matrix[i - 1][j - 1] + 1;
			matrix[i][j] = min((d1, d2), d3);
		}
	}

	return matrix[alen][blen];
}

int main()
{
	string a = "a";
	string b = "ab";
	cout << "distance:" << LevenshteinDistance(a, b) << endl;
}
