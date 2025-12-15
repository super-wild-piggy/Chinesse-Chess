#define _MBCS
#include <stdio.h>
#include <graphics.h>
#include <conio.h>

#define ROW 10
#define COL 9     
#define GRID_SIZE 93
#define INTERVAL 95

enum Pieces {//枚举所有的棋子
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

struct State {
    int begr;
    int begc;
    int endr;
    int endc;
    int state;
}state={-1,-1,-1,-1,BEGIN};

void chessMove();
//初始化数据
void init() {
    for (size_t i = 0; i < ROW; i++)
    {
        for (size_t j = 0; j < COL; j++)
        {
            map[i][j].id = NONE;//初始化棋盘
            int temp = 0;//表示棋子在数组中的位置
            if (i <= 4) {//放黑色棋子
                map[i][j].type = BLACK;

                if (i == 0) {//放置第一行
                    if (j <= 4) {
                        temp = j;
                    }
                    else {
                        temp = 8 - j;
                    }
                    map[i][j].id = blackChess[temp];
                }
                if (i == 2) {//放置第三行的炮
                    if (j == 1 || j == 7) {
                        map[i][j].id = blackChess[5];
                    }
                }
                if (i == 3) {//放置第四行的兵
                    if (j % 2 == 0) {
                        map[i][j].id = blackChess[6];
                    }
                }

            }
            else {//放红色棋子
                map[i][j].type = RED;
                if (i == 6) {//放置第七行的卒
                    if (j % 2 == 0) {
                        map[i][j].id = redChess[6];
                    }
                }
                if (i == 7) {//放置第八行的炮
                    if (j == 1 || j == 7) {
                        map[i][j].id = redChess[5];
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
                }
            }
            map[i][j].isRiver = false;
            map[i][j].x = INTERVAL + j * GRID_SIZE;
            map[i][j].y = INTERVAL + i * GRID_SIZE;
        }
    }
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

//绘制棋子
void draw() {
    setfillcolor(RGB(252, 215, 162));
    setlinestyle(PS_SOLID, 2);
    settextstyle(30, 0, "楷体");
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
            //printf("(%d,%d)", row, col);
            if (state.state == BEGIN) {
                state.begc = col;
                state.begr = row;
                state.state = END;
            }
            if (state.state == END) {
                state.endc = col;
                state.endr = row;
                state.state = BEGIN;
            }
        }
    }
}

void chessMove() {

}



int main() {
    //创建图形窗口
    initgraph(931, 993);
    //设置背景模式
    setbkmode(TRANSPARENT);
    //贴棋盘
    IMAGE img_board;
    loadimage(&img_board, "D:\\mew\\chess\\chessboard.jpg");
    

    init();
    show();
    //双缓冲绘图
    BeginBatchDraw();
    while (true) {
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