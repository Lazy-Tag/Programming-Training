#include <iostream>
#include <cstring>
#include <valarray>
#include <vector>

#define x first
#define y second

using namespace std;

const int ROW = 30, COL = 40;
const int INF = 1e9;
const int MAXLEN = 1210, MAXPLAYER = 300;

typedef pair<int, int> PII;
typedef pair<double, int> PDI;

int T, K, B, N;
double score = -INF, option = 0;
int X, Y, myself;
int dist[ROW][COL];
vector<PDI> op;
double dampingfactor = 1, scoreforbean = 1.5;
double scoreforshield = 10, scorefactor = 1;
int R[4] = {2, 3, 0, 1};
int dx[4] = {0, -1, 0, 1}; // 左上右下
int dy[4] = {-1, 0, 1, 0};

struct Map{
    int type; // 0是空地，1是积分，2是增长豆，3是盾牌，4是障碍物，5是蛇头
    int score;
    int time;
    int num = -1;
    void setScore(int v)
    {
        if(v > 0)
        {
            type = 1;
            score = v;
        }
        else if(v == -1)
        {
            type = 2;
            score = 5;
        }
        else
        {
            type = 3;
            score = 30;
        }
    }
    void setHead(int name, int t)
    {
        type = 5;
        num = name;
        time = t;
    }
}map[ROW][COL]; // 定义map类表示地图

double getDist(int a, int b, int x, int y)
{
    int dx = a - x, dy = b - y;
    return sqrt(dx * dx + dy * dy);
} // 求两点之间距离

void initBarrier(int x, int y, int r, int t) // 初始化障碍物
{
    if(map[x][y].type == 5) return ;
    for(int i = -r; i <= r; i ++ )
        for(int j = -r; j <= r; j ++ )
        {
            int a = x + i, b = y + i;
            if(getDist(x, y, a, b) > r) continue;
            map[a][b].type = 4;
            map[a][b].time = max(map[a][b].time, t);
        }
}

struct Snake{
    string name;
    PII head;
    PII body[MAXLEN];
    int length;
    int score;
    int direct;
    int shield;
    int inProtected;
    void init(string _name, int _len, int _score, int _direct, int _shield, int _time, int num)
    {
        name = _name, length = _len, score = _score;
        direct = _direct, shield = _shield, inProtected = _time;
        for(int i = 0; i < length; i ++ )
        {
            int x, y;
            scanf("%d%d", &x, &y);
            if(!i)
            {
                map[x][y].setHead(num, length);
                head = {x, y};
            }
            else
            {
                body[i] = {x, y};
                if(name != "2021201780") initBarrier(x, y, 1, length - i);
            }
        }
    }
    bool operator < (const Snake& t){ return score > t.score; }
    void useShield()
    {
        shield -- ;
        inProtected += 5;
    }
    void setBarrier(int x, int y)
    {
        score -= 30;
        inProtected = max(2, inProtected);
        initBarrier(x, y, 1, 10);
    }
    bool checkShield(){ return shield > 0 && inProtected <= 2; }
    bool checkScore(Snake snake[])
    {
        bool flag = false;
        for(int i = 1; i <= 5; i ++ )
            if(snake[i].name == "2021201780") flag = true;
        return score > 30 && flag;
    }
    void move(int type)
    {
        PII last = head;
        map[head.x][head.y].type = 4;
        head.x += dx[type], head.y += dy[type];
        map[head.x][head.y].time = length;
        for(int i = 1; i < length; i ++ )
        {
            map[last.x][last.y].time -- ;
            PII temp = body[i];
            body[i] = last;
            last = temp;
        }
        map[last.x][last.y].type = 0;
        map[last.x][last.y].time = 0;
        if(name == "2021201780") return ;
        for(int i = 0; i < 4; i ++ )
        {
            int x = head.x + dx[i], y = head.y + dy[i];
            initBarrier(x, y, 1, 1);
        }
    }
}snake[MAXPLAYER]; // 定义贪吃蛇类表示一条蛇
vector<Snake> danger; // 记录有危险的蛇

double evaluateValue()
{
    double res = 0;
    for(int i = 0; i < ROW; i ++ )
        for(int j = 0; j < COL; j ++ )
        {
            if(!dist[i][j]) continue;
            if (map[i][j].type == 1) res += (double) map[i][j].score / dist[i][j];
            else if (map[i][j].type == 2) res += (double) scoreforbean / dist[i][j];
            else if (map[i][j].type == 3) res += (double) scoreforshield / dist[i][j];
        }
    return res;
} // 评估地图上的积分豆随评估分数的影响

void bfsValue(int x, int y, int direct)
{
    int hh = 0, tt = -1;
    PII q[ROW * COL];
    memset(dist, -1, sizeof dist);
    q[ ++ tt] = {x, y};
    dist[x][y] = 0;
    while(hh <= tt)
    {
        auto t = q[hh ++ ];
        int distance = dist[t.x][t.y];
        for(int i = 0; i < 4; i ++ )
        {
            int a = t.x + dx[i], b = t.y + dy[i];
            if(t == snake[myself].head && i == R[direct]) continue;
            if(a < 0 || a >= ROW || b < 0 || b >= COL) continue;
            if(snake[myself].inProtected <= distance && (map[a][b].type == 4 || map[a][b].type == 5)) continue;
            if(dist[a][b] > -1) continue;
            dist[a][b] = distance + 1;
            q[ ++ tt] = {a, b};
        }
    }
} // 计算每个积分到蛇头的距离

double getScore(Snake s, int direct)
{
    PII position = s.head;
    if(position.x < 0 || position.x >= 30 || position.y < 0 || position.y >= 40) return -INF;
    Map now = map[position.x][position.y];
    bfsValue(position.x, position.y, direct);
    double res = scorefactor * (s.score + s.shield * scoreforshield * max(10 - s.shield, 0) / 10);
    if(now.type == 1) res += now.score;
    else if(now.type == 2) res += scoreforbean;
    else if(now.type == 3) res += scoreforshield;
    else if((now.type == 4 || now.type == 5) && s.inProtected  <= 1) res -= INF;
    res += evaluateValue() / dampingfactor;
    return res;
} // 评估每个决策的分数

bool dangerous(Snake s)
{
    int x = s.head.x, y = s.head.y;
    bool flag = false;
    for(int i = 0; i < 4; i ++ )
    {
        int a = x + dx[i], b = y + dy[i];
        if(map[a][b].type == 5)
        {
            Snake d = snake[map[a][b].num];
            danger.push_back(d);
            flag = true;
        }
    }

    for(int i = -3; i <= 3; i ++ )
    {
        for(int j = -3; j <= 3; j ++ )
        {
            int a = x + i, b = y + j;
            if(a < 0 || a >= ROW || b < 0 || b >= COL) continue;
            if(abs(i) + abs(j) >= 1 && abs(i) + abs(j) <= 3)
            {
                if(map[a][b].type != 5) continue;
                Snake d = snake[map[a][b].num];
                if(d.checkScore(snake) || (d.score > 30 && snake[myself].shield >= 2))
                {
                    flag = true;
                    danger.push_back(d);
                }
            }
        }
    }
    return flag;
} // 判断是否危险

void setFactor()
{
    scoreforshield = (5 + N * 3) * (12 - snake[myself].shield) / 12;
    dampingfactor = (1 + N * 0.3);
    if(T <= 10) dampingfactor *= (1 + (double) (10 - T) / 10);
    if(T <= 100) scorefactor += (100 - T) / 40;
}// 通过棋局剩余时间设置参数的大小

int choose(Snake s)
{
    int x = s.head.x, y = s.head.y, op;
    if(s.shield && s.score < 30 && danger.size() >= 2) return 4;
    else
    {
        if(!s.shield && s.checkScore(snake))
        {
            X = danger[0].head.x;
            Y = danger[0].head.y;
            return 5;
        }
        else if(!s.checkScore(snake)) return 4;
        else
        {
            if(danger[0].score > 100 && !danger[0].shield && T > 20)
            {
                X = danger[0].head.x;
                Y = danger[0].head.y;
                return 5;
            }
            else return 4;
        }
    }
}//当有危险是选择使用盾牌或释放障碍物

void input()
{
    cin >> T;
    cin >> K;
    while(K -- )
    {
        int x, y, v;
        scanf("%d%d%d", &x, &y, &v);
        map[x][y].setScore(v);
    }
    cin >> B;
    while(B -- )
    {
        int x, y, r, t;
        scanf("%d%d%d%d", &x, &y, &r, &t);
        initBarrier(x, y, 2, t);
    }
    cin >> N;
    for(int i = 1; i <= N; i ++ )
    {
        int n, len, score, direct, shield, time;
        scanf("%d%d%d%d%d%d", &n, &len, &score, &direct, &shield, &time);
        string name = to_string(n);
        snake[i].init(name, len, score, direct, shield, time, i);
    }
    for(int i = 0; i < ROW; i ++ )
        for(int j = 0; j < COL; j ++ )
        {
            if(!snake[myself].shield && snake[myself].inProtected == 1 && map[i][j].type < 4)
            {
                int cnt = 0;
                for(int k = 0; k < 4; k ++ )
                {
                    int a = i + dx[k], b = j + dy[k];
                    if(a < 0 || a >= ROW || b < 0 || b >= COL) continue;
                    if(map[a][b].type >= 4) cnt ++ ;
                }
                if(cnt >= 3) initBarrier(i, j, 1, 1);
            }
        }
    sort(snake + 1, snake + N + 1);
    for(int i = 1; i <= N ; i ++ )
        if(snake[i].name == "2021201780") myself = i;
}// 处理输入

int main()
{
    input();
    setFactor();
    Snake srce = snake[myself];
    for (int i = 0; i < 4; i ++ )
    {
        if (R[i] == snake[myself].direct) continue;
        snake[myself].move(i);
        double prediction = getScore(snake[myself], i);
        op.push_back({prediction, i});
        if (prediction > score)
        {
            score = prediction;
            option = i;
        }
        snake[myself] = srce;
    }
    sort(op.begin(), op.end());
    reverse(op.begin(), op.end());
    for(int i = 0; i < 3; i ++ )
    {
        score = op[i].x;
        if(score < -1e7) break;
        option = op[i].y;
        snake[myself].move(option);
        if(dangerous(snake[myself]))
        {
            if(snake[myself].inProtected >= 2) break;
            if(!snake[myself].checkShield() && !snake[myself].checkScore(snake))
            {
                snake[myself] = srce;
                continue;
            }
            else if(i < 2 && op[i + 1].x > -1e7 )
            {
                snake[myself] = srce;
                snake[myself].useShield();
                double p1 = getScore(snake[myself], snake[myself].direct);
                snake[myself] = srce;
                snake[myself].move(op[i + 1].y);
                double p2 = getScore(snake[myself], op[i + 1].y);
                if(p1 > p2 || dangerous(snake[myself])) option = choose(snake[myself]);
                else option = op[i + 1].y;
                break;
            }
        }
        else break;
    }
    if (snake[myself].checkShield())
    {
        snake[myself].useShield();
        double prediction = getScore(snake[myself], snake[myself].direct);
        if (prediction > score)
        {
            score = prediction;
            option = 4;
        }
        snake[myself] = srce;
    }
    if(snake[1].name == "2021201780")
    {
        if(danger.size()) snake[myself].setBarrier(danger[0].head.x, danger[0].head.y);
        else snake[myself].setBarrier(snake[myself].body[2].x, snake[myself].body[2].y);
        double prediction = getScore(snake[myself], snake[myself].direct);
        if (prediction > score)
        {
            score = prediction;
            option = 5;
        }
        snake[myself] = srce;
    }
    if (option < 5) cout << option;
    else cout << 5 << ' ' << X << ' ' << Y;
    return 0;
}
