#include <conio.h>
#include <deque>
#include <vector>
#include <ctime>
#include <cwchar>
#include <windows.h>
using namespace std;
HANDLE hOut;
COORD pos;

//1.實現gotoxy函式
void gotoxy(short x, short y) {
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); //獲取控制代碼
	pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);  //移動游標到指定位置
}
void HideCursor() { //隱藏游標
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//獲取控制檯游標資訊
	CursorInfo.bVisible = false;    //隱藏控制檯游標
	SetConsoleCursorInfo(handle, &CursorInfo);//設定控制檯游標狀態
}
//2.玩家的結構體

struct Player {
	int position_x, position_y;    //玩家的資訊
};
struct Barriers {
	int item, position_x = 1400, position_y = 155;    //障礙物的資訊
};
//3.遊戲執行類
class Game {
private:
	char image;
	enum mapSize { width = 1490, height = 160 }; //遊戲地圖;寬度為偶數
	Player player;      //玩家的位置
	Player before;
	deque<Barriers> barrier;
	char hit = 'x';       //按鍵輸入
	bool end = false;     //結束遊戲
	int jump = 0;         //跳躍狀態
	int init_t = int(time(NULL)); //起始時間
	int speed_level = 0;  //速度等級
	int speed = 12;       //速度(越小越慢)
	int score = 0;        //分數
	int barrier_gap = 40; //障礙物最小間距
	int displacement = 10;//移動距離
public:
	Game();
	void draw_Frame() {  //畫邊框
		for (int i = 0; i <= width; i += 2) { //□ 這個符號佔兩個字元位置，所以是+2
			gotoxy(i, height);
			printf("□");
		}
		for (int i = 0; i <= width; i += 2) {
			gotoxy(i, height - 8);
			printf("—");
		}
	}
	void init_Set() {  //初始化
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.dwFontSize.X = 0;                  // Weight
		cfi.dwFontSize.Y = 1;                  // Height
		SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
		system("mode con cols=1510 lines=300"); //設定開啟視窗大小
		system("color f1"); //設定背景色和前景色
		system("title 小恐龍2.0"); //設定視窗標題
		srand(int(time(NULL)));

		player = { 80, 130 };//起始位置
		before = player;
		{//speed level
			gotoxy(140, 200);
			printf("■■■  ■■■  ■■■  ■■■  ■■        ■      ■■■  ■  ■  ■■■  ■");
			gotoxy(140, 201);
			printf("■      ■  ■  ■      ■      ■  ■      ■      ■      ■  ■  ■      ■      ■");
			gotoxy(140, 202);
			printf("■■■  ■■■  ■■■  ■■■  ■  ■      ■      ■■■  ■  ■  ■■■  ■");
			gotoxy(140, 203);
			printf("    ■  ■      ■      ■      ■  ■      ■      ■      ■  ■  ■      ■      ■");
			gotoxy(140, 204);
			printf("■■■  ■      ■■■  ■■■  ■■        ■■■  ■■■    ■    ■■■  ■■■");
		}
		{//score
			gotoxy(140, 210);
			printf("■■■  ■■■  ■■■  ■■■  ■■■");
			gotoxy(140, 211);
			printf("■      ■      ■  ■  ■  ■  ■      ■");
			gotoxy(140, 212);
			printf("■■■  ■      ■  ■  ■■    ■■■");
			gotoxy(140, 213);
			printf("    ■  ■      ■  ■  ■  ■  ■      ■");
			gotoxy(140, 214);
			printf("■■■  ■■■  ■■■  ■  ■  ■■■");
		}
		{//Time
			gotoxy(140, 220);
			printf("■■■  ■■■  ■      ■  ■■■");
			gotoxy(140, 221);
			printf("  ■      ■    ■■  ■■  ■      ■");
			gotoxy(140, 222);
			printf("  ■      ■    ■  ■  ■  ■■■");
			gotoxy(140, 223);
			printf("  ■      ■    ■  ■  ■  ■      ■");
			gotoxy(140, 224);
			printf("  ■    ■■■  ■  ■  ■  ■■■");
		}
	}
	int key_Down() { //按鍵響應
		char key = hit;
		if (_kbhit()) //接受按鍵
			hit = _getch();
		if (hit == ' ') {//按空白鍵起跳
			if (hit == ' ') {
				player.position_y -= 2;
				return 1;
			}
		}
		return 0;
	}
	void Clear() {//清除畫面
		int i, j, bs = barrier.size();
		for (i = 0; i < bs; i++) {
			for (j = 0; j < 27; j++) {
				gotoxy(barrier[i].position_x, barrier[i].position_y - j);
				if (j == 3) {
					printf("—————————————————————————————————————");
				}
				else {
					printf("                                                                          ");
				}
			}
		}
		gotoxy(92, height - 8);
		printf("————————");
		{
			gotoxy(before.position_x + 24, before.position_y);
			printf("                      ");
			gotoxy(before.position_x + 22, before.position_y + 1);
			printf("                          ");
			gotoxy(before.position_x + 22, before.position_y + 2);
			printf("                          ");
			gotoxy(before.position_x + 22, before.position_y + 3);
			printf("                          ");
			gotoxy(before.position_x + 22, before.position_y + 4);
			printf("                          ");
			gotoxy(before.position_x + 22, before.position_y + 5);
			printf("                          ");
			gotoxy(before.position_x + 22, before.position_y + 6);
			printf("                          ");
			gotoxy(before.position_x + 22, before.position_y + 7);
			printf("              ");
			gotoxy(before.position_x + 22, before.position_y + 8);
			printf("                    ");
			gotoxy(before.position_x + 22, before.position_y + 9);
			printf("                    ");
			gotoxy(before.position_x + 22, before.position_y + 10);
			printf("          ");
			gotoxy(before.position_x, before.position_y + 11);
			printf("                                ");
			gotoxy(before.position_x, before.position_y + 12);
			printf("                                    ");
			gotoxy(before.position_x, before.position_y + 13);
			printf("                                    ");
			gotoxy(before.position_x, before.position_y + 14);
			printf("                                ");
			gotoxy(before.position_x, before.position_y + 15);
			printf("                                ");
			gotoxy(before.position_x, before.position_y + 16);
			printf("                                ");
			gotoxy(before.position_x + 2, before.position_y + 17);
			printf("                            ");
			gotoxy(before.position_x + 4, before.position_y + 18);
			printf("                          ");
			gotoxy(before.position_x + 6, before.position_y + 19);
			printf("                      ");
			gotoxy(before.position_x + 8, before.position_y + 20);
			printf("                  ");
			gotoxy(before.position_x + 10, before.position_y + 21);
			printf("                ");
			gotoxy(before.position_x + 12, before.position_y + 22);
			printf("                ");
			gotoxy(before.position_x + 12, before.position_y + 23);
			printf("              ");
			gotoxy(before.position_x + 12, before.position_y + 24);
			printf("              ");
			gotoxy(before.position_x + 12, before.position_y + 25);
			printf("                ");
		}
	}
	void show_player() {//顯示玩家
		//擦掉前面軌跡
		before = player;
		//畫新的位置
		{
			gotoxy(player.position_x + 24, player.position_y);
			printf("■■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 1);
			printf("■■■■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 2);
			printf("■■■    ■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 3);
			printf("■■■    ■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 4);
			printf("■■■■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 5);
			printf("■■■■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 6);
			printf("■■■■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 7);
			printf("■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 8);
			printf("■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 9);
			printf("■■■■■■■■■■");
			gotoxy(player.position_x + 22, player.position_y + 10);
			printf("■■■■■");
			gotoxy(player.position_x, player.position_y + 11);
			printf("■                  ■■■■■■");
			gotoxy(player.position_x, player.position_y + 12);
			printf("■              ■■■■■■■■■■");
			gotoxy(player.position_x, player.position_y + 13);
			printf("■■        ■■■■■■■■■■  ■");
			gotoxy(player.position_x, player.position_y + 14);
			printf("■■■    ■■■■■■■■■■■");
			gotoxy(player.position_x, player.position_y + 15);
			printf("■■■■■■■■■■■■■■■■");
			gotoxy(player.position_x, player.position_y + 16);
			printf("■■■■■■■■■■■■■■■■");
			gotoxy(player.position_x + 2, player.position_y + 17);
			printf("■■■■■■■■■■■■■■");
			gotoxy(player.position_x + 4, player.position_y + 18);
			printf("■■■■■■■■■■■■■");
			gotoxy(player.position_x + 6, player.position_y + 19);
			printf("■■■■■■■■■■■");
			gotoxy(player.position_x + 8, player.position_y + 20);
			printf("■■■■■■■■■");
			gotoxy(player.position_x + 10, player.position_y + 21);
			printf("■■■■■■■■");
			if (jump) {
				gotoxy(player.position_x + 12, player.position_y + 22);
				printf("■■■  ■■");
				gotoxy(player.position_x + 12, player.position_y + 23);
				printf("■■      ■");
				gotoxy(player.position_x + 12, player.position_y + 24);
				printf("■        ■");
				gotoxy(player.position_x + 12, player.position_y + 25);
				printf("■■      ■■");
			}
			else if ((score / 2) % 2) {
				gotoxy(player.position_x + 12, player.position_y + 22);
				printf("■■      ■■");
				gotoxy(player.position_x + 14, player.position_y + 23);
				printf("■■      ■");
				gotoxy(player.position_x + 24, player.position_y + 24);
				printf("■");
				gotoxy(player.position_x + 24, player.position_y + 25);
				printf("■■");
			}
			else {
				gotoxy(player.position_x + 12, player.position_y + 22);
				printf("■■■      ■■");
				gotoxy(player.position_x + 12, player.position_y + 23);
				printf("■■");
				gotoxy(player.position_x + 12, player.position_y + 24);
				printf("■");
				gotoxy(player.position_x + 12, player.position_y + 25);
				printf("■■");
			}
		}
	}
	void Jump(int state) {//跳躍
		switch (state) {
		case 1:
			player.position_y -= 10;
			break;
		case 2:
			player.position_y -= 9;
			break;
		case 3:
			player.position_y -= 8;
			break;
		case 4:
			player.position_y -= 7;
			break;
		case 5:
			player.position_y -= 6;
			break;
		case 6:
			player.position_y -= 5;
			break;
		case 7:
			player.position_y -= 4;
			break;
		case 8:
			player.position_y -= 2;
			break;
		case 9:
			player.position_y--;
			break;
		case 10:
			player.position_y += 2;
			break;
		case 11:
			player.position_y += 3;
			break;
		case 12:
			player.position_y += 4;
			break;
		case 13:
			player.position_y += 5;
			break;
		case 14:
			player.position_y += 6;
			break;
		case 15:
			player.position_y += 7;
			break;
		case 16:
			player.position_y += 8;
			break;
		case 17:
			player.position_y += 9;
			break;
		case 18:
			player.position_y += 10;
			break;
		}
	}
	void Speed_Score() {//速度與分數
		if (!(score % 200) && speed_level < 15) {
			speed_level++;
			displacement++;
			if (!(speed_level % 3)) {
				speed--;
			}
			show_Digital(230, 200, speed_level);
		}
		show_Digital(190, 210, (++score) / 2);
	}
	void show_Time() {//時間與分數
		int tn = int(time(NULL)) - init_t;
		show_Digital(184, 220, tn);
	}
	void make_Barrier() {//是否生成障礙物
		if ((score == barrier_gap)) {
			Barriers b = { rand() % 2 };
			barrier.push_back(b);
			barrier_gap += rand() % 40 + (40 - displacement * 1.2);
		}
	}
	void show_Barrier() {//顯示障礙物
		int i, j, bs = barrier.size();
		if (bs > 0 && barrier[0].position_x < 20) {
			bs--;
			barrier.pop_front();
		}
		for (i = 0; i < bs; i++) {
			barrier[i].position_x -= displacement;
			switch (barrier[i].item) {
			case 0://大火山
				{
					gotoxy(barrier[i].position_x, barrier[i].position_y - 26);
					printf("              ★★      ★      ★      ★★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 25);
					printf("            ★    ★  ★  ★  ★  ★  ★    ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 24);
					printf("          ★        ★      ★      ★        ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 23);
					printf("                      ★    ★    ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 22);
					printf("                ★★    ★  ★  ★    ★★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 21);
					printf("              ★    ★    ★★★    ★    ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 20);
					printf("            ★        ★    ★    ★        ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 19);
					printf("                  ◎●◎●◎●◎●◎●◎");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 18);
					printf("                  ●◎●◎●◎●◎●◎●");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 17);
					printf("                  ◎●◎●◎●◎●◎●◎");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 16);
					printf("                ◎●◎●◎●◎●◎●◎●◎");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 15);
					printf("                ●◎●◎●◎●◎●◎●◎●");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 14);
					printf("                ■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 13);
					printf("             ■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 12);
					printf("             ■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 11);
					printf("             ■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 10);
					printf("            ■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 9);
					printf("            ■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 8);
					printf("          ■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 7);
					printf("          ■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 6);
					printf("        ■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 5);
					printf("        ■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 4);
					printf("      ■■■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 3);
					printf("      ■■■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 2);
					printf("    ■■■■■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 1);
					printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y);
					printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");

				}
				break;
			case 1://小火山
				{
					gotoxy(barrier[i].position_x, barrier[i].position_y - 18);
					printf("            ★★    ★  ★    ★★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 17);
					printf("          ★    ★    ★    ★    ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 16);
					printf("                  ★  ★  ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 15);
					printf("              ★★  ★  ★  ★★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 14);
					printf("            ★    ★  ★  ★    ★");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 13);
					printf("                ◎●◎●◎●◎");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 12);
					printf("                ●◎●◎●◎●");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 11);
					printf("              ●◎●◎●◎●◎●");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 10);
					printf("              ■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 9);
					printf("              ■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 8);
					printf("            ■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 7);
					printf("            ■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 6);
					printf("          ■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 5);
					printf("          ■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 4);
					printf("        ■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 3);
					printf("      ■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 2);
					printf("    ■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y - 1);
					printf("  ■■■■■■■■■■■■■■■■■■■■■");
					gotoxy(barrier[i].position_x, barrier[i].position_y);
					printf("■■■■■■■■■■■■■■■■■■■■■■■");
				}
				break;
			}
		}
	}
	void Lose() {
		if (barrier.size() != 0 && barrier[0].position_x <= 114) {//碰到障礙物
			if (!(barrier[0].item)) {
				if (!jump) {
					if (barrier[0].position_x >= 52) {
						end = true;
					}
				}
				else if (jump == 2 || jump == 18) {
					if (barrier[0].position_x <= 102 && barrier[0].position_x >= 64) {
						end = true;
					}
				}
				else if (jump == 3 || jump == 17) {
					if (barrier[0].position_x <= 94 && barrier[0].position_x >= 70) {
						end = true;
					}
				}
				else if (jump == 4 || jump == 16) {
					if (barrier[0].position_x <= 90 && barrier[0].position_x >= 68) {
						end = true;
					}
				}
			}
			if (barrier[0].item) {
				if (!jump) {
					if (barrier[0].position_x >= 56) {
						end = true;
					}
				}
				else if (jump == 2 || jump == 18) {
					if (barrier[0].position_x <= 102 && barrier[0].position_x >= 68) {
						end = true;
					}
				}
				else if (jump == 3 || jump == 17) {
					if (barrier[0].position_x <= 98 && barrier[0].position_x >= 74) {
						end = true;
					}
				}
			}
		}
	}
	void show_Digital(int x, int y, int num) {
		vector<int> number;
		for (; num > 0; num /= 10) {
			number.push_back(num % 10);
		}
		int i = number.size();
		for (i; i > 0; i--, x += 8) {
			switch (number[i - 1]) {
			case 0:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("■  ■");
				gotoxy(x, y + 2);
				printf("■  ■");
				gotoxy(x, y + 3);
				printf("■  ■");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			case 1:
				gotoxy(x, y);
				printf("  ■  ");
				gotoxy(x, y + 1);
				printf("■■  ");
				gotoxy(x, y + 2);
				printf("  ■  ");
				gotoxy(x, y + 3);
				printf("  ■  ");
				gotoxy(x, y + 4);
				printf("■■■  ");
				break;
			case 2:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("    ■");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("■    ");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			case 3:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("    ■");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("    ■");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			case 4:
				gotoxy(x, y);
				printf("■  ■");
				gotoxy(x, y + 1);
				printf("■  ■");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("    ■");
				gotoxy(x, y + 4);
				printf("    ■");
				break;
			case 5:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("■    ");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("    ■");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			case 6:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("■    ");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("■  ■");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			case 7:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("■  ■");
				gotoxy(x, y + 2);
				printf("■  ■");
				gotoxy(x, y + 3);
				printf("    ■");
				gotoxy(x, y + 4);
				printf("    ■");
				break;
			case 8:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("■  ■");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("■  ■");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			case 9:
				gotoxy(x, y);
				printf("■■■");
				gotoxy(x, y + 1);
				printf("■  ■");
				gotoxy(x, y + 2);
				printf("■■■");
				gotoxy(x, y + 3);
				printf("    ■");
				gotoxy(x, y + 4);
				printf("■■■");
				break;
			}
		}
	}
};

Game::Game() {
	HideCursor();//隱藏游標
	init_Set();//初始設定
	draw_Frame();//畫邊框
	while ((!end)) {//遊戲進行
		hit = 'x';
		if ((!jump)) {//是否起跳
 			jump = key_Down();
		}
		else {//已經起跳
			Jump(jump);
			if (jump < 17 && _kbhit()) //接受按鍵
				hit = _getch();
			if (++jump == 19) {
				jump = 0;
			}
		}
		make_Barrier();
		Clear();
		show_Barrier();
		show_player();
		Sleep(speed);
		Speed_Score();
		show_Time();
		Lose();
	}
}

int main() {
	Game game;
	Sleep(600);
	gotoxy(0, 299);
	system("pause");//按任意鍵結束
}
