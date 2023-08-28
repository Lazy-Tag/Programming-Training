#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <random>

#define DEBUG 0         // 调试宏
#define MAXTIME 930     // 最大运行时间
#define MORECHESS 0     // 跳过大于五个棋子的情况
#define CHENG5 10000000 // 各种棋型分数定义
#define HUO4 10000
#define CHONG4 1000
#define HUO3 800
#define TIAOHUO3 700
#define MIAN3 50
#define HUO2 5
#define MIAN2 1
#define HUO1 1

#define x first
#define y second

using namespace std;

typedef long long LL;
typedef pair<int, LL> PIL;

const int N = 15;
const LL INF = 1 << 30;
int player, nextplayer;
int solution;
unordered_set<int> S;              // 下一步棋可能性的集合
unordered_map<LL, PIL> chessBoard; // 棋盘与深度和分数的映射
unordered_map<LL, LL> toParent;    // 每个棋盘对应父节点
unordered_map<LL, LL> Next;        // 每个棋盘下一步棋的位置
int graph[N][N];
LL zobristBlock[N][N], zobristPlayer[N][N], zobristNextPlayer[N][N], MAP, backup; // zobrist哈希棋局
int dx[] = {0, 0, 1, -1, -1, 1, -1, 1};                                           // 八个方向的位置数组
int dy[] = {1, -1, 0, 0, -1, 1, 1, -1};

struct Node // 存取下这部下一步棋的位置与得分
{
    int x, y;
    int score;
    bool operator<(const Node &t) const
    {
        return score < t.score;
    }
};

void print() // 调试函数
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int t = 2;
            if (graph[i][j] >= 0)
                printf("%3d", graph[i][j]);
            else
                printf("%3d", t);
        }
        puts("");
    }
    puts("");
}

int evaluate(int x, int y, int p) // 每颗棋子对应分数
{
    int res = 0;
    for (int i = 0; i < 8; i += 2) // 四个方向建成字符串
    {
        int j = i + 1;
        string str = "1";
        for (int k = 1; k <= 4; k++)
        {
            int a = x + k * dx[i], b = y + k * dy[i];
            if (a < 0 || a >= N || b < 0 || b >= N)
            {
                str += '-';
                break;
            }
            else if (graph[a][b] == (p ^ 1))
                str += '-';
            else if (graph[a][b] == p)
                str += '1';
            else
                str += '0';
        }
        reverse(str.begin(), str.end());
        for (int k = 1; k <= 4; k++)
        {
            int a = x + k * dx[j], b = y + k * dy[j];
            if (a < 0 || a >= N || b < 0 || b >= N)
            {
                str += '-';
                break;
            }
            else if (graph[a][b] == (p ^ 1))
                str += '-';
            else if (graph[a][b] == p)
                str += '1';
            else
                str += '0';
        }
        if (str.find("111111") != string ::npos)
            res += MORECHESS; // 判断棋型的类型
        else if (str.find("11111") != string ::npos)
            res += CHENG5;
        else if (str.find("011110") != string ::npos)
            res += HUO4;
        else if (str.find("01111-") != string ::npos ||
                 str.find("-11110") != string ::npos ||
                 str.find("10111") != string ::npos ||
                 str.find("11011") != string ::npos ||
                 str.find("11101") != string ::npos)
            res += CHONG4;
        else if (str.find("01110") != string ::npos)
            res += HUO3;
        else if (str.find("1011") != string ::npos ||
                 str.find("1101") != string ::npos)
            res += TIAOHUO3;
        else if (str.find("00111-") != string ::npos ||
                 str.find("-11100") != string ::npos ||
                 str.find("01011-") != string ::npos ||
                 str.find("-11010") != string ::npos ||
                 str.find("01101-") != string ::npos ||
                 str.find("-10110") != string ::npos ||
                 str.find("10011") != string ::npos ||
                 str.find("11001") != string ::npos ||
                 str.find("10101") != string ::npos ||
                 str.find("-01110-") != string ::npos)
            res += MIAN3;
        else if (str.find("001100") != string ::npos ||
                 str.find("01010") != string ::npos ||
                 str.find("1001") != string ::npos)
            res += HUO2;
        else if (str.find("00011-") != string ::npos ||
                 str.find("-11000") != string ::npos ||
                 str.find("10001") != string ::npos ||
                 str.find("00101-") != string ::npos ||
                 str.find("-10100") != string ::npos ||
                 str.find("01001-") != string ::npos ||
                 str.find("-10010") != string ::npos)
            res += MIAN2;
        else if (str.find("00100") != string ::npos ||
                 str.find("01000") != string ::npos ||
                 str.find("10000") != string ::npos ||
                 str.find("00010") != string ::npos ||
                 str.find("00001") != string ::npos)
            res += HUO1;
    }
    return res;
}

int h() // 评估棋局分数
{
    int res = 0;
    for (auto it : S)
    {
        int i = it / N, j = it % N;
        if (graph[i][j] == player)
            res += evaluate(i, j, player);
        else if (graph[i][j] == nextplayer)
            res -= evaluate(i, j, nextplayer);
    }
    return res;
}

bool miniMax(int u, int depth, int BEGIN) // minimax求下一步棋的位置
{
    clock_t END = clock();
    if ((double)(END - BEGIN) * 1000 / CLOCKS_PER_SEC > MAXTIME) // 搜索到指定时间返回
    {
        solution = Next[backup];
        graph[solution / N][solution % N] = player;
        return true;
    }
    if (!depth)
    {
        chessBoard[MAP] = {0, h()};
        return false;
    }
    LL self = MAP;
    LL parent = toParent[MAP];
    Node node[N * N];
    int cnt = 0;
    unordered_set<int> K;
    for (int t : S) // 将可行步加入集合中
    {
        int x = t / N, y = t % N;
        for (int i = 0; i < 8; i++)
        {
            int a = x + dx[i], b = y + dy[i];
            int option = a * N + b;
            if (K.count(option))
                continue;
            if (a < 0 || a >= N || b < 0 || b >= N)
                continue;
            if (graph[a][b] != -1)
                continue;
            K.insert(option);
            graph[a][b] = u;
            node[cnt++] = {a, b, h()};
            graph[a][b] = -1;
        }
    }
    sort(node, node + cnt); // 对权值进行排序
    if (u == player)
        reverse(node, node + cnt);
    double v = chessBoard[parent].y;
    if (!chessBoard.count(self)) // 初始化
    {
        if (u == player)
            chessBoard[self] = {0, -INF};
        else
            chessBoard[self] = {0, INF};
    }
    for (int i = 0; i < cnt; i++) // 遍历每种情况
    {
        int a = node[i].x, b = node[i].y;
        graph[a][b] = u;
        MAP ^= zobristBlock[a][b];
        if (u == player)
            MAP ^= zobristPlayer[a][b];
        else
            MAP ^= zobristNextPlayer[a][b];
        int option = a * N + b;
        S.insert(option);
        if (u == player)
        {
            if (chessBoard.count(MAP) && chessBoard[MAP].x < depth)
                goto R1; // 置换表优化
            if (miniMax(u ^ 1, depth - 1, BEGIN))
                return true;
        R1:
            graph[a][b] = -1;
            if (chessBoard[MAP] > chessBoard[self])
            {
                chessBoard[self] = chessBoard[MAP];
                Next[self] = option;
            }
            MAP ^= zobristBlock[a][b];
            if (u == player)
                MAP ^= zobristPlayer[a][b];
            else
                MAP ^= zobristNextPlayer[a][b];
            S.erase(option);
            if (chessBoard[self].y > v)
                return false; // AlphaBeta剪枝
        }
        else
        {
            if (chessBoard.count(MAP) && chessBoard[MAP].x < depth)
                goto R2; // 置换表优化
            if (miniMax(u ^ 1, depth - 1, BEGIN))
                return true;
        R2:
            graph[a][b] = -1;
            if (chessBoard[MAP] < chessBoard[self])
            {
                chessBoard[self] = chessBoard[MAP];
                Next[self] = option;
            }
            MAP ^= zobristBlock[a][b];
            if (u == player)
                MAP ^= zobristPlayer[a][b];
            else
                MAP ^= zobristNextPlayer[a][b];
            S.erase(option);
            if (chessBoard[self].y < v)
                return false; // AlphaBeta剪枝
        }
    }
    return false;
}

int main()
{
    cin >> player;
    nextplayer = player ^ 1;
    mt19937_64 rng(time(0));
    uniform_int_distribution<LL> distribution(1, 1ll << 63); // 随机数生成器
    bool flag = false;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            cin >> graph[i][j];
            if (graph[i][j] != -1)
            {
                flag |= true;
                S.insert(i * N + j);
            }
            zobristBlock[i][j] = distribution(rng);
            zobristPlayer[i][j] = distribution(rng);
            zobristNextPlayer[i][j] = distribution(rng); // 定义zobrist哈希数组
        }
    if (!flag)
        cout << 7 << ' ' << 7 << endl; // 如果棋盘上没棋子，直接下中元
    else
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                int t = graph[i][j];
                if (t == -1)
                    MAP ^= zobristBlock[i][j];
                else if (t == player)
                    MAP ^= zobristPlayer[i][j];
                else
                    MAP ^= zobristNextPlayer[i][j];
            }
        backup = MAP; // 初始棋盘哈希值
        toParent[MAP] = 0;
        if (DEBUG)
            print();
        int depth = 2;
        clock_t BEGIN = clock();
        while (!miniMax(player, depth, BEGIN))
            depth += 2;
        cout << solution / N << ' ' << solution % N << endl;
    }
    if (DEBUG)
        print();
    return 0;
}
