/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool readFile();//파일을 읽어 input배열에 저장하는 함수
	void freeMemory();//메모리를 할당 해제를 담당하는 함수
	bool DFS();//힌트를 알려주기 위해 미로의 해답을 dfs알고리즘으로 찾아주는 함수
	void hintdraw();// DFS()에서 구한 미로의 해답을 화면 상에 아주 짧은 시간 동안 그려주는 함수
	void draw_point();//현재 사용자가 움직이는 점의 위치를 그려주는 함수
	void draw_maze();//현재 점의 위치를 반영하여 그 주변 3x3의 해당하는 미로의 영역만 보여주는 함수
	void finish();//사용자가 움직이는 점이 미로의 목적지에 도달했을 때 게임이 끝났음을 알리는 함수


	int WIDTH;//미로의 높이
	int HEIGHT;//미로의 너비
	vector<vector<char>> input;//텍스트 파일의 모든 정보를 담는 이차원 배열
	int** visited;//방문여부를 저장할 포인터
	int** path;//지금까지 방문한 모든 방을 저장하는 배열
	int** result_path;//출발지에서 목적지까지의 길을 저장하는 배열
	int maze_col;//미로칸의 열의 인덱스를 가리킨다.
	int maze_row;//미로칸의 행의 인덱스를 가리킨다.
	int hint_count;//총 3번의 힌트만을 보여주기 위해서 힌트의 남은 횟수를 저장하는 변수
	int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 열리지 않았고 1이면 열렸다.
	int isDFS;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행하지 않았고 1이면 실행했다.
	bool hint_flag;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행하지 않았고 1이면 실행했다.
	// Menu
	ofxWinMenu* menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	// Used by example app
	ofTrueTypeFont myFont;
	ofImage myImage;
	float windowWidth, windowHeight;//화면의 너비와 높이를 저장하는 변수
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;//전체화면을 실행할 것인지 알려주는 flag
	bool bTopmost;
	//bool isdfs;

	bool finish_flag;//사용자가 움직이는 점의 위치가 미로의 목적지에 도달했는지 알려주는 flag, true면 도착
	bool play_flag;//사용자가 게임을 실행하였는지 알려주는 flag, true면 실행

	// Example functions
	void doFullScreen(bool bFull);//전체화면을 실행해주는 함수
	void init_point();//사용자가 움직일 점의 위치를 처음 위치로 초기화해주는 함수

	double size;//미로의 size를 조절하는 변수
	int move_x[4] = { 0, -1, 0, 1 }; //move_x와 move_y는 모두 미로의 주변 방들을 우, 하, 좌, 상의 순서로 탐색하기 위한 배열
	int move_y[4] = { -1, 0, 1, 0 };
	int Sx;//목적지의 x좌표를 나타내는 변수
	int Sy;//목적지의 y좌표를 나타내는 변수

	pair<int, int> start, end, point;//각각 start는 사용자가 움직일 점과 미로의 출발지, end는 미로의 목적지, point는 사용자가 움직이는 점의 위치를 저장하는 변수

	ofImage game_clear;//마지막에 사용자가 움직이는 점이 미로의 목적지에 도달했을 때 화면에 보여줄 이미지

};
