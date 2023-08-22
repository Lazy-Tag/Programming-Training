#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <queue>

#define x first
#define y second

using namespace std;

typedef pair<int, string> PIS;

int cube[6][3][3];
int stk[30], top;
string END = "", start, backup;
unordered_map<string, int> hashMap;     // 存储每一个状态与每一个操作的映射
unordered_map<string, string> toParent; // 存储每一个节点与其父节点的映射
unordered_map<string, int> dist;

int get(char c)
{
    if (c == 'g')
        return 0;
    if (c == 'r')
        return 1;
    if (c == 'y')
        return 2;
    if (c == 'b')
        return 3;
    if (c == 'w')
        return 4;
    return 5;
} // 将颜色字母映射到数字0 ~ 5

string toString(int q[][3][3]) // 将初始状态转化为字符串作为初始状态̬
{
    string str;
    for (int j = 0; j < 3; j++)
        for (int k = 0; k < 3; k++)
            str += q[0][j][k] + '0';
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            str += q[3][i][j] + '0';
        for (int j = 0; j < 3; j++)
            str += q[5][i][j] + '0';
        for (int j = 0; j < 3; j++)
            str += q[4][i][j] + '0';
    }

    for (int j = 0; j < 3; j++)
        for (int k = 0; k < 3; k++)
            str += q[2][j][k] + '0';

    for (int j = 0; j < 3; j++)
        for (int k = 0; k < 3; k++)
            str += q[1][j][k] + '0';

    return str;
}

bool check(string str) // 检查状态是否为终止状态
{
    for (int i = 1; i < 9; i++)
        if (str[i] != str[i - 1])
            return false;
    for (int i = 1; i < 9; i++)
        if (str[i + 36] != str[i + 35])
            return false;
    for (int i = 1; i < 9; i++)
        if (str[i + 45] != str[i + 44])
            return false;
    char s[] = {str[9], str[10], str[11], str[18], str[19], str[20], str[27], str[28], str[29]};
    for (int i = 1; i < 9; i++)
        if (s[i] != s[i - 1])
            return false;
    return true;
}

void rotate(string &x, int type)
{
    if (type >= 0 && type <= 5)
    {
        int t = type / 2;
        char t1 = x[t], t2 = x[t + 3], t3 = x[t + 6];
        if (!(type % 2)) // +
        {
            x[t] = x[45 + t], x[t + 3] = x[48 + t], x[t + 6] = x[51 + t];
            x[45 + t] = x[36 + t], x[48 + t] = x[39 + t], x[51 + t] = x[42 + t];
            x[36 + t] = x[12 + t], x[39 + t] = x[21 + t], x[42 + t] = x[30 + t];
            x[12 + t] = t1, x[21 + t] = t2, x[30 + t] = t3;
            if (t == 0)
            {
                string temp = x;
                x[9] = temp[27], x[10] = temp[18], x[11] = temp[9];
                x[18] = temp[28], x[20] = temp[10];
                x[27] = temp[29], x[28] = temp[20], x[29] = temp[11];
            }
            else if (t == 2)
            {
                string temp = x;
                x[15] = temp[17], x[16] = temp[26], x[17] = temp[35];
                x[24] = temp[16], x[26] = temp[34];
                x[33] = temp[15], x[34] = temp[24], x[35] = temp[33];
            }
        }
        else // -
        {
            x[t] = x[12 + t], x[t + 3] = x[21 + t], x[t + 6] = x[30 + t];
            x[12 + t] = x[36 + t], x[21 + t] = x[39 + t], x[30 + t] = x[42 + t];
            x[36 + t] = x[45 + t], x[39 + t] = x[48 + t], x[42 + t] = x[51 + t];
            x[45 + t] = t1, x[48 + t] = t2, x[51 + t] = t3;
            if (t == 0)
            {
                string temp = x;
                x[9] = temp[11], x[10] = temp[20], x[11] = temp[29];
                x[18] = temp[10], x[20] = temp[28];
                x[27] = temp[9], x[28] = temp[18], x[29] = temp[27];
            }
            else if (t == 2)
            {
                string temp = x;
                x[15] = temp[33], x[16] = temp[24], x[17] = temp[15];
                x[24] = temp[34], x[26] = temp[16];
                x[33] = temp[35], x[34] = temp[26], x[35] = temp[17];
            }
        }
    }
    else if (type >= 6 && type <= 11)
    {
        int t = type / 2 - 3;
        char t1 = x[t * 3], t2 = x[t * 3 + 1], t3 = x[t * 3 + 2];
        if (!(type % 2))
        {
            x[t * 3] = x[27 + t], x[t * 3 + 1] = x[18 + t], x[t * 3 + 2] = x[9 + t];
            x[9 + t] = x[42 - 3 * t], x[18 + t] = x[43 - 3 * t], x[27 + t] = x[44 - 3 * t];
            x[42 - 3 * t] = x[35 - t], x[43 - 3 * t] = x[26 - t], x[44 - 3 * t] = x[17 - t];
            x[17 - t] = t1, x[26 - t] = t2, x[35 - t] = t3;
            if (t == 0)
            {
                string temp = x;
                x[45] = temp[47], x[46] = temp[50], x[47] = temp[53];
                x[48] = temp[46], x[50] = temp[52];
                x[51] = temp[45], x[52] = temp[48], x[53] = temp[51];
            }
            else if (t == 2)
            {
                string temp = x;
                x[12] = temp[30], x[13] = temp[21], x[14] = temp[12];
                x[21] = temp[31], x[23] = temp[13];
                x[30] = temp[32], x[31] = temp[23], x[32] = temp[14];
            }
        }
        else
        {
            x[t * 3] = x[17 - t], x[t * 3 + 1] = x[26 - t], x[t * 3 + 2] = x[35 - t];
            x[17 - t] = x[44 - 3 * t], x[26 - t] = x[43 - 3 * t], x[35 - t] = x[42 - 3 * t];
            x[42 - 3 * t] = x[9 + t], x[43 - 3 * t] = x[18 + t], x[44 - 3 * t] = x[27 + t];
            x[27 + t] = t1, x[18 + t] = t2, x[9 + t] = t3;
            if (t == 0)
            {
                string temp = x;
                x[45] = temp[51], x[46] = temp[48], x[47] = temp[45];
                x[48] = temp[52], x[50] = temp[46];
                x[51] = temp[53], x[52] = temp[50], x[53] = temp[47];
            }
            else if (t == 2)
            {
                string temp = x;
                x[12] = temp[14], x[13] = temp[23], x[14] = temp[32];
                x[21] = temp[13], x[23] = temp[31];
                x[30] = temp[12], x[31] = temp[21], x[32] = temp[30];
            }
        }
    }
    else
    {
        int t = type / 2 - 6;
        char t1 = x[27 - t * 9], t2 = x[28 - t * 9], t3 = x[29 - t * 9];
        if (!(type % 2))
        {
            for (int i = 0; i < 3; i++)
                x[27 + i - 9 * t] = x[47 - i + 3 * t];
            for (int i = 0; i < 3; i++)
                x[45 + i + 3 * t] = x[35 - i - 9 * t];
            for (int i = 0; i < 3; i++)
                x[33 + i - 9 * t] = x[30 + i - 9 * t];
            x[30 - 9 * t] = t1, x[31 - 9 * t] = t2, x[32 - 9 * t] = t3;
            if (t == 0)
            {
                string temp = x;
                x[36] = temp[42], x[37] = temp[39], x[38] = temp[36];
                x[39] = temp[43], x[41] = temp[37];
                x[42] = temp[44], x[43] = temp[41], x[44] = temp[38];
            }
            else if (t == 2)
            {
                string temp = x;
                x[0] = temp[2], x[1] = temp[5], x[2] = temp[8];
                x[3] = temp[1], x[5] = temp[7];
                x[6] = temp[0], x[7] = temp[3], x[8] = temp[6];
            }
        }
        else
        {
            for (int i = 0; i < 3; i++)
                x[27 + i - 9 * t] = x[30 + i - 9 * t];
            for (int i = 0; i < 3; i++)
                x[30 + i - 9 * t] = x[33 + i - 9 * t];
            for (int i = 0; i < 3; i++)
                x[33 + i - 9 * t] = x[47 - i + 3 * t];
            x[47 + 3 * t] = t1, x[46 + 3 * t] = t2, x[45 + 3 * t] = t3;
            if (t == 0)
            {
                string temp = x;
                x[36] = temp[38], x[37] = temp[41], x[38] = temp[44];
                x[39] = temp[37], x[41] = temp[43];
                x[42] = temp[36], x[43] = temp[39], x[44] = temp[42];
            }
            else if (t == 2)
            {
                string temp = x;
                x[0] = temp[6], x[1] = temp[3], x[2] = temp[0];
                x[3] = temp[7], x[5] = temp[1];
                x[6] = temp[8], x[7] = temp[5], x[8] = temp[2];
            }
        }
    }
} // 魔方的旋转操作

int getMin(string x, int cen, int l, int r, int u, int d) // 计算每一面还原最少需要的步数
{
    int res = 0;
    char c = x[cen];
    if (x[l] != c)
        res++;
    if (x[r] != x[l] && x[r] != c)
        res++;
    if (x[u] != c)
        res++;
    if (x[d] != x[u] && x[d] != c)
        res++;
    return res;
}

int h(string x)
{
    int res = 0;
    res = max(res, getMin(x, 4, 3, 5, 1, 7));
    res = max(res, getMin(x, 19, 18, 20, 10, 28));
    res = max(res, getMin(x, 25, 24, 26, 16, 34));
    res = max(res, getMin(x, 22, 21, 23, 13, 31));
    res = max(res, getMin(x, 40, 39, 41, 37, 43));
    res = max(res, getMin(x, 49, 48, 50, 46, 52));
    return res;
} // 启发函数

void Astar() // A*
{
    priority_queue<PIS, vector<PIS>, greater<PIS>> heap;
    heap.push({h(start), start});
    dist[start] = 0;

    while (heap.size())
    {
        auto t = heap.top();
        heap.pop();

        string state = t.y;
        int distance = t.x;
        if (check(state))
        {
            END = state;
            return;
        }

        string parent = state;
        for (int i = 0; i < 18; i++)
        {
            rotate(state, i);
            if (!dist.count(state) || dist[state] > distance + 1)
            {
                dist[state] = distance + 1;
                toParent[state] = parent;
                hashMap[state] = i;
                heap.push({h(state) + dist[state], state});
            }
            state = parent;
        }
    }
}

int main()
{
    fstream file;
    file.open("input.txt", ios ::in);
    string str;
    unordered_map<string, int> M;
    M["back:"] = 0, M["down:"] = 1, M["front:"] = 2;
    M["left:"] = 3, M["right:"] = 4, M["up:"] = 5;
    for (int i = 0; i < 6; i++) // 数据读入
    {
        file >> str;
        for (int j = 0; j < 3; j++)
        {
            char c;
            for (int k = 0; k < 3; k++)
            {
                file >> c;
                cube[M[str]][j][k] = get(c);
            }
        }
    }

    start = toString(cube);
    backup = start;

    Astar();
    while (END != backup) // 将操作反向压入栈中
    {
        stk[++top] = hashMap[END];
        END = toParent[END];
    }

    for (int i = top; i; i--)
    {
        int t = stk[i] & 1;
        printf("%d%c ", stk[i] / 2, t ? '-' : '+');
    }

    return 0;
}
