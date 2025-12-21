#define _MBCS
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <math.h>

#define ROW 10
#define COL 9     
#define GRID_SIZE 93
#define INTERVAL 95

//枚举所有的棋子
enum Pieces {
    NONE = -1,
    車, 馬, 象, 士, 将, 砲, 卒,
    俥, 马, 相, 仕, 帥, 炮, 兵,
    BEGIN, END,
};

enum Pieces redChess[] = { 車,馬,象,士,将,砲,卒 };
enum Pieces blackChess[] = { 俥,马,相,仕,帥,炮,兵 };

//绘制时转化成字符串
const char* ChessName[] = { "車","馬","象","士","将","砲","卒","俥","马","相","仕","帥","炮","兵"};

//每一个棋子的属性
struct Chess {
    enum Pieces id;//棋子名称
    DWORD type;//棋子颜色
    short x;
    short y;
    bool isRiver;//是否过河
};



//游戏地图
struct Chess map[ROW][COL];

//游戏结束标志
bool ifrun;


//计时器属性
struct timeclock {
    time_t current_time;
    time_t start_time;
    int total;
    int stepTotal;
    bool ifrunning;
};
struct timeclock black, red;


char redTotal[10];
char redStepTotal[10];
char blackTotal[10];
char blackStepTotal[10];

//计时转化为输出的字符串
char* totaltime(int time,char* output) {
    int min = time / 60;
    int sec = time % 60;
    sprintf_s(output,10, "%02d:%02d", min, sec);
    return output;
}

struct State {//每一步的状态，用来判断是否合法和计算时间
    int begr;
    int begc;
    int endr;
    int endc;
    int state;
    int color;
}state={-1,-1,-1,-1,BEGIN,RED};

void chessMove();
void chessTime();

//初始化数据
void init() {
    for (size_t i = 0; i < ROW; i++)
    {
        for (size_t j = 0; j < COL; j++)
        {
            map[i][j].id = NONE;//初始化棋盘
            int temp = 0;//表示棋子在数组中的位置
            if (i <= 4) {//放黑色棋子
                map[i][j].type = WHITE;

                if (i == 0) {//放置第一行
                    if (j <= 4) {
                        temp = j;
                    }
                    else {
                        temp = 8 - j;
                    }
                    map[i][j].id = blackChess[temp];
                    map[i][j].type = BLACK;
                }
                if (i == 2) {//放置第三行的炮
                    if (j == 1 || j == 7) {
                        map[i][j].id = blackChess[5];
                        map[i][j].type = BLACK;
                    }
                }
                if (i == 3) {//放置第四行的兵
                    if (j % 2 == 0) {
                        map[i][j].id = blackChess[6];
                        map[i][j].type = BLACK;
                    }
                }

            }
            else {//放红色棋子
                map[i][j].type = WHITE;
                if (i == 6) {//放置第七行的卒
                    if (j % 2 == 0) {
                        map[i][j].id = redChess[6];
                        map[i][j].type = RED;
                    }
                }
                if (i == 7) {//放置第八行的炮
                    if (j == 1 || j == 7) {
                        map[i][j].id = redChess[5];
                        map[i][j].type = RED;
                    }
                }
                if (i == 9) {//放置第十行
                    if (j <= 4) {
                        temp = j;
                    }
                    else {
                        temp = 8 - j;
                    }
                    map[i][j].id = redChess[temp];
                    map[i][j].type = RED;
                }
            }
            map[i][j].isRiver = false;
            map[i][j].x = INTERVAL + j * GRID_SIZE;
            map[i][j].y = INTERVAL + i * GRID_SIZE;
        }
    }
}
void initTime() {
    black.stepTotal = 60;
    black.total = 15 * 60;
    red.stepTotal = 60;
    red.total = 15 * 60;
    black.ifrunning = false;
    red.ifrunning = true;
    totaltime(black.stepTotal, blackStepTotal);
    totaltime(black.total, blackTotal);
    totaltime(red.stepTotal, redStepTotal);
    totaltime(black.total, redTotal);
}

//打印棋盘
void show() {
    for (size_t i = 0; i < ROW; i++)
    {
        for (size_t j = 0; j < COL; j++)
        {
            printf("%2d  ", map[i][j].id);
        }
        printf("\n");

    }

}

//绘制
void draw() {
    chessTime();
    setfillcolor(RGB(252, 215, 162));
    setlinestyle(PS_SOLID, 2);
    settextstyle(30, 0, "楷体");
    setlinecolor(BLACK);
    settextcolor(BLACK);
    rectangle(72, 2, 351, 49);//绘制黑方计时器
    line(211, 2, 211, 49);
    outtextxy(100, 13, blackTotal);
    outtextxy(230, 13, blackStepTotal);
    setlinecolor(RED);
    settextcolor(RED);
    rectangle(570, 2, 849, 49);//绘制红方计时器
    line(709, 2, 709, 49);
    outtextxy(740, 13, redTotal);
    outtextxy(600, 13, redStepTotal);

    for (size_t i = 0; i < ROW; i++)
    {
        for (size_t j = 0; j < COL; j++)
        {
            if (map[i][j].id != NONE) {
                setlinecolor(map[i][j].type);
                fillcircle(map[i][j].x, map[i][j].y, 30);
                fillcircle(map[i][j].x, map[i][j].y, 25);
                settextcolor(map[i][j].type);
                outtextxy(map[i][j].x-15, map[i][j].y-15, ChessName[map[i][j].id]);
                
                
            }
        }
    }
}

//鼠标操作
void mouseEvent() {
    ExMessage msg;//定义消息结构体变量
    if (peekmessage(&msg, EM_MOUSE)) {
        if (msg.message == WM_LBUTTONDOWN) {//鼠标左键按下
            //通过鼠标坐标得出点击数组的下标
            int col = (msg.x - INTERVAL) / GRID_SIZE;
            int row = (msg.y - INTERVAL) / GRID_SIZE;
            //下标校准
            if (msg.x <map[row][col].x + GRID_SIZE / 3 && msg.y > map[row][col].y + GRID_SIZE / 3) {
                row++;
            }
            if (msg.x >map[row][col].x + GRID_SIZE / 3 && msg.y > map[row][col].y + GRID_SIZE / 3) {
                row++;
                col++;
            }
            if (msg.x > map[row][col].x + GRID_SIZE / 3 && msg.y < map[row][col].y + GRID_SIZE / 3) {
                col++;
            }
            
            if (state.state == BEGIN) {
                state.begc = col;
                state.begr = row;
                state.state = END;
            }else if (state.state == END) {
                state.endc = col;
                state.endr = row;
                state.state = BEGIN;
            }
            
            printf("begin(%d %d)end(%d %d)", state.begr, state.begc, state.endr, state.endc);
            chessMove();
        }
    }
}

//移动棋子
void chessMove() {
    if ((state.begc != state.endc || state.begr != state.endr) &&
        state.begc != -1 && state.begr != -1 && state.endr != -1 && state.endc != -1&&
        map[state.begr][state.begc].type!=map[state.endr][state.endc].type&&
        map[state.begr][state.begc].id!=NONE&&map[state.begr][state.begc].type==state.color) {//基础规则判断
        bool ifmove = true;
        switch (map[state.begr][state.begc].id) {
        case 車:
        case 俥:
            if (state.begc == state.endc && state.begr > state.endr) {
                for (int i = state.endr + 1; i < state.begr; i++)
                {
                    if (map[i][state.begc].id != NONE) {
                        ifmove = false;
                        break;
                    }
                }
            }
            else if (state.begc == state.endc && state.begr < state.endr) {
                for (int i = state.begr + 1; i < state.endr; i++)
                {
                    if (map[i][state.begc].id != NONE) {
                        ifmove = false;
                        break;
                    }
                }
            }
            else if (state.begr == state.endr && state.begc < state.endc) {
                for (int i = state.begc + 1; i < state.endc; i++)
                {
                    if (map[state.begr][i].id != NONE) {
                        ifmove = false;
                        break;
                    }
                }
            }
            else if (state.begr == state.endr && state.begc > state.endc) {
                for (int i = state.endc + 1; i < state.begc; i++)
                {
                    if (map[state.begr][i].id != NONE) {
                        ifmove = false;
                        break;
                    }
                }
            }
            else {
                ifmove = false;
            }
            break;
        case 馬:
        case 马:
            if ((abs(state.begc - state.endc) == 2 && abs(state.begr - state.endr) == 1) || (abs(state.begc - state.endc) == 1 && abs(state.begr - state.endr) == 2)) {
                if (state.begc - state.endc == 2) {//向左跳跃
                    if (map[state.begr][state.endc + 1].id != NONE) {
                        ifmove=false;
                    }

                }
                else if (state.endc - state.begc == 2) {//向右跳跃
                    if (map[state.begr][state.begc + 1].id != NONE) {
                        ifmove=false;
                    }
                }
                else if (state.begr - state.endr == 2) {//向后跳跃
                    if (map[state.endr + 1][state.begc].id != NONE) {
                        ifmove = false;
                    }
                }
                else if (state.endr - state.begr == 2) {//向前跳跃
                    if (map[state.begr + 1][state.begc].id != NONE) {
                        ifmove = false;
                    }
                }
            }else{
                ifmove = false;
            }
            break;
        case 象:
        case 相:
            if ((map[state.begr][state.begc].type == RED && state.endr >= 5) || (map[state.begr][state.begc].type == BLACK && state.endr <= 4)
                &&abs(state.begc-state.endc)==2&&abs(state.begr-state.endr)==2) {
                if (map[(state.begr + state.endr) / 2][(state.begc + state.endc) / 2].id != NONE) {
                    ifmove = false;
                }
            }
            else {
                ifmove = false;
            }
            break;
        }
        
        
        
        
        if (ifmove) {
            map[state.endr][state.endc].id = map[state.begr][state.begc].id;//把棋子移动到下棋位置
            map[state.begr][state.begc].id = NONE;//最开始的位置为空
            map[state.endr][state.endc].type = map[state.begr][state.begc].type;//把棋子移动到下棋位置
            map[state.begr][state.begc].type = WHITE;
            printf("移动\n");
            state.begc = -1;
            state.begr = -1;
            state.endc = -1;
            state.endr = -1;
            if (state.color == BLACK) {
                state.color = RED;
                red.ifrunning = true;
                black.ifrunning = false;
                time(&red.start_time);
                red.stepTotal = 60;//重置红棋步时
            }
            else {
                state.color = BLACK;
                red.ifrunning = false;
                black.ifrunning = true;
                time(&black.start_time);
                black.stepTotal = 60;//重置黑棋步时
            }
        }
        else {
            printf("无法移动\n");
            state.begc = -1;
            state.begr = -1;
            state.endc = -1;
            state.endr = -1;
        }
    }
    else if(state.begc != -1 && state.begr != -1 && state.endr != -1 && state.endc != -1){
        printf("无法移动\n");
        state.begc = -1;
        state.begr = -1;
        state.endc = -1;
        state.endr = -1;
    }
}

void chessTime() {
    time(&red.current_time);
    time(&black.current_time);
    if (red.ifrunning) {
        if (red.total != 0 && red.stepTotal != 0) {
            int diff = difftime(red.current_time, red.start_time);
            red.total -= diff;
            red.stepTotal -= diff;
            totaltime(red.total, redTotal);//转化字符串
            totaltime(red.stepTotal, redStepTotal);//转化字符串
            red.start_time = red.current_time;//重置开始时间
        }
        else {
            printf("黑方胜利\n");
            ifrun = false;
        }
    }
    if (black.ifrunning) {
        if (black.total != 0 && black.stepTotal != 0) {
            int diff = difftime(black.current_time, black.start_time);
            black.total -= diff;
            black.stepTotal -= diff;
            totaltime(black.total, blackTotal);//转化字符串
            totaltime(black.stepTotal, blackStepTotal);//转化字符串
            black.start_time = black.current_time;//重置开始时间
        }
        else {
            printf("红方胜利\n");
            ifrun = false;
        }
    }



}



int main() {
    //创建图形窗口
    initgraph(931, 1050);
    //设置背景模式
    setbkmode(TRANSPARENT);
    setbkcolor(WHITE);
    //贴棋盘
    IMAGE img_board;
    loadimage(&img_board, "chessboard.jpg");
    ifrun = true;

    init();
    initTime();
    show();
    time(&red.start_time);
    //双缓冲绘图
    BeginBatchDraw();
    while (ifrun) {
        cleardevice();
        putimage(0, 0, &img_board);
        draw();
        mouseEvent();
        
        FlushBatchDraw();
    }
    
    EndBatchDraw();
    
  
    getchar();
    return 0;
}