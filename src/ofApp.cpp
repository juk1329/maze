/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
//#include "maze.h"
#include <iostream>
using namespace std;

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(0, 0, 0, 0);//어두운 곳에서 움직이는 점의 근처만 미로가 보이게 할 것이므로 배경을 검은색으로 설정
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();

	finish_flag = false;//flag 초기화
	play_flag = false;
	hint_flag = false;
	isOpen = 0;

	hint_count = 3;//hint_count 초기화
	init_point();//움직이는 점의 위치를 처음 위치로 초기화

	game_clear.load("images/game_clear.png");//목적지에 도달 했을 떄 나오는 이미지 로드

	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Option");

	menu->AddPopupItem(hPopup, "Play", false, false);
	bShowInfo = true;  // screen info display on
	//menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Hint"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup

//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) { //선택한 메뉴에 따른 동작을 수행한다.

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		if (isOpen) {
			freeMemory();//메모리 할당 해제
			finish_flag = false;//flag 초기화
			play_flag = false;

			hint_count = 3;//hint_count 3으로 초기화
		}
		readFile();//파일을 읽어드리고 정보는 input에 저장
	}
	if (title == "Exit") {

		freeMemory();//메모리 할당 해제
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//

	if (title == "Play") {
		if (isOpen) {
			play_flag = true;//게임을 선택하였으므로 true로 설정
			finish_flag = false;//finish_flag 초기화
			init_point();//점의 위치 초기화
		}
		else {
			cout << "you must open file first" << endl;
		}
	}
	if (title == "Hint") {
		if (isOpen) {
			DFS();//이 프로젝트에서 다루는 미로의 특성상 bfs보다 dfs가 더 효율이 좋으므로 힌트를 알려주기 위해 미로의 해답을 계산
			hint_flag = true;//hint 골랐음을 표시
		}
		else {
			cout << "you must open file first" << endl;
		}
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {//window창의 크기에 맞는 미로를 그리기 위해 size 조정해주는 함수
	
	size = (double)ofGetHeight() / (double)(5 * (HEIGHT / 2) + (HEIGHT - HEIGHT / 2));
	if (size > (double)ofGetWidth() / (double)(5 * (WIDTH / 2) + (WIDTH - WIDTH / 2))) {
		size = (double)ofGetWidth() / (double)(5 * (WIDTH / 2) + (WIDTH - WIDTH / 2));
	}
}


//--------------------------------------------------------------
void ofApp::draw() {//힌트를 사용했다면 잠깐 동안 보여주고, 게임이 진행되고 있는 중이라면 바뀌는 점의 위치를 그리는 함수를 호출한다.

	ofBackground(0, 0, 0, 0);//어두운 곳에서 움직이는 점의 근처만 미로가 보이게 할 것이므로 배경을 검은색으로 설정

	if (hint_flag)//힌트를 사용했다면 잠깐 동안 미로의 해답을 보여준다.
	{
		ofSetLineWidth(5);
		if (isOpen) {
			if (hint_count > 0) {
				hint_count--;//남은 힌트의 개수는 1 차감
				printf("Number of hints available : %d\n", hint_count);
				hintdraw();//미로의 해답을 그려주는 함수
			}
		}
		else
			cout << "You must open file first" << endl;
		hint_flag = false;
	}
	
	draw_maze();//시작 지점에 있는 점의 위치로부터 주위만 미로의 형태를 보여준다.

	if (play_flag) draw_point();//점의 위치를 그려주는 함수

} // end Draw


void ofApp::doFullScreen(bool bFull) //전체화면과 관련된 함수이다.
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
	}

} // end doFullScreen

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {//ESC 키로 전체화면 해제, 혹은 전체화면 중이 아니라면 종료
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			freeMemory();//메모리 할당 해제
			ofExit();//종료
		}
	}
	if (key == OF_KEY_UP) {//위쪽 방향키를 눌렀을 때
		if (play_flag) {//게임이 실행 중일 때
			if (finish_flag)return;//지금 점의 좌표가 목적지에 도달해있다면 무시
			int p_x = point.second;//p_x에 현재 점의 x좌표 저장
			int p_y = point.first;//p_y에 현재 점의 y좌표 저장
			if (input[p_y - 1][p_x] == ' ') {//위쪽 벽이 뚫려있는지 확인
				point.first = point.first - 2;//벽이 뚫려 있다면 y값을 2만큼 차감시켜 움직이는 점을 위쪽 방으로 이동
			}
		}
	}
	if (key == OF_KEY_DOWN) {//아래쪽 방향키를 눌렀을 때
		if (play_flag) {//게임이 실행 중일 때
			if (finish_flag)return;// 지금 점의 좌표가 목적지에 해당한다면 무시
			int p_x = point.second;//p_x에 현재 점의 x좌표 저장
			int p_y = point.first; //p_y에 현재 점의 y좌표 저장
			if (input[p_y + 1][p_x] == ' ') {//아래쪽 벽이 뚫려있는지 확인
				point.first = point.first + 2;//벽이 뚫려 있다면 y값을 2만큼 증가시켜 움직이는 점을 아래쪽 방으로 이동
			}
		}
	}
	if (key == OF_KEY_LEFT) {//왼쪽 방향키를 눌렀을 때
		if (play_flag) {//게임이 실행 중일 때
			if (finish_flag)return;//지금 점의 좌표가 목적지에 해당한다면 무시
			int p_x = point.second;//p_x에 현재 점의 x좌표 저장
			int p_y = point.first;//p_y에 현재 점의 y좌표 저장
			if (input[p_y][p_x - 1] == ' ') {//왼쪽 벽이 뚫려있는지 확인
				point.second = point.second - 2;//벽이 뚫려 있다면 x값을 2만큼 차감시켜 움직이는 점을 왼쪽 방으로 이동
			}
		}
	}
	if (key == OF_KEY_RIGHT) {//오른쪽 방향키를 눌렀을 때
		if (play_flag) {//게임이 실행 중일 때
			if (finish_flag)return;//지금 점의 좌표가 목적지에 해당한다면 무시
			int p_x = point.second;//p_x에 현재 점의 x좌표 저장
			int p_y = point.first;//p_y에 현재 점의 y좌표 저장
			if (input[p_y][p_x + 1] == ' ') {//오른쪽 벽이 뚫려있는지 확인
				point.second = point.second + 2;//벽이 뚫려 있다면 x값을 2만큼 증가시켜 움직이는 점을 오른쪽 방으로 이동
			}
		}

	}
}
// end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()//파일을 읽고 input에 정보를 저장하는 함수
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {//.maz파일 일때만 읽기

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			if (buffer.size()) {
				//파일을 한줄 씩 읽어서 line에 저장
				for (ofBuffer::Line temp = buffer.getLines().begin(), end = buffer.getLines().end(); temp != end; temp++) {
					string line = *temp;
					vector<char> words(line.begin(), line.end());//line 전체를 words에 char타입을 저장
					input.emplace_back(words);//input에 words의 내용 저장, 따라서 input에 정보 저장
				}
			}

			HEIGHT = input.size();//높이를 저장
			WIDTH = input[0].size();//너비를 저장

			//힌트를 위해 미로의 해답을 찾는 dfs에 필요한 배열들을 각각 미로의 크기에 맞게 할당
			visited = (int**)malloc(sizeof(int*) * HEIGHT);
			for (int i = 0; i < HEIGHT; i++) {
				visited[i] = (int*)malloc(sizeof(int) * WIDTH);
			}
			path = (int**)malloc(sizeof(int*) * HEIGHT);
			for (int i = 0; i < HEIGHT; i++) {
				path[i] = (int*)malloc(sizeof(int) * WIDTH);
			}
			result_path = (int**)malloc(sizeof(int*) * HEIGHT);
			for (int i = 0; i < HEIGHT; i++) {
				result_path[i] = (int*)malloc(sizeof(int) * WIDTH);
			}
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {//메모리를 할당 해제, 초기화 해주는 함수
	HEIGHT = 0;//높이 초기화
	WIDTH = 0;//너비 초기화
	//각각 dfs()와 hintdraw()에서 사용했던 배열들을 할당해제
	input.clear();
	for (int i = 0; i < HEIGHT; i++) {
		free(visited[i]);
	}
	free(visited);
	for (int i = 0; i < HEIGHT; i++) {
		free(path[i]);
	}
	free(path);
	for (int i = 0; i < HEIGHT; i++) {
		free(result_path[i]);
	}
	free(result_path);

}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//각 배열들을 초기화
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			visited[i][j] = 0;
		}
	}
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			path[i][j] = 0;
		}
	}
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			result_path[i][j] = 0;
		}
	}

	stack < pair<int, int>> A;
	A.emplace(make_pair(1, 1));//dfs를 위한 stack을 선언

	visited[1][1] = 1;//처음 출발지는 방문한 것으로 처리

	vector<vector<pair<int, int>>> cell;
	cell.resize(HEIGHT);//이전에 방문한 방을 기억하기 위한 배열
	for (int i = 0; i < HEIGHT; i++) {
		cell[i].resize(WIDTH);
	}//초기화

	bool flag = false;//목적지에 도달했는지 구별해주는 flag 초기화

	Sy = HEIGHT - 2;
	Sx = WIDTH - 2;//각 Sy와 Sx는 목적지 좌표에 해당

	while (A.size() && !flag) {//스택 A에 아무런 정보가 없거나 flag가 true면 종료
		int y = A.top().first;
		int x = A.top().second;//각각 A.top에 있던 좌표를 저장
		path[y][x] = 1;//방문한 방이므로 곧 지나온 방이 된다. 따라서 path에 저장
		A.pop();//스택 A를 pop연산

		for (int i = 0; i < 4; i++) {//우,하,좌,상의 순서로 주변 방들을 검사하는 반복문 
			int temp_y = y + move_y[i];
			int temp_x = x + move_x[i];//검사할 방의 좌표를 temp_x, temp_y에 저장

			if (visited[temp_y][temp_x] || input[temp_y][temp_x] != ' ')//방문했던 방이거나 막혀있으면 넘기기
				continue;

			visited[temp_y][temp_x] = 1;//위 경우에 해당하지 않으므로 방문한 것이 됨. 방문함으로 저장
			A.emplace(make_pair(temp_y, temp_x));//스택 A에 그 좌표를 저장
			cell[temp_y][temp_x] = make_pair(y, x);//현재 좌표를 이전 좌표로 저장

			if (temp_y == Sy && temp_x == Sx) {//목적지에 도달했다면 flag를 설정해주고 break해주기
				flag = true;
				break;
			}
		}
	}

	if (flag) {//성공적으로 목적지에 도달했다면
		int y = Sy;
		int x = Sx;//x,y는 목적지 좌표를 저장
		result_path[y][x] = 1;//목적지이므로 미로의 해답에 해당함. 따라서 result_path에 좌표 저장

		while (y != 1 || x != 1) {//목적지에서부터 거꾸로 출발지까지 올라가면서 해답에 해당하는 좌표를 저장
			int cy = cell[y][x].first;
			int cx = cell[y][x].second;
			result_path[cy][cx] = 1;
			y = cy, x = cx;
		}
	}
	return false;
}

void ofApp::hintdraw()//미로의 해답을 짧은 시간 동안이지만 화면에서 보여주는 함수
{
	for (int i = 0; i < HEIGHT; i++) {
		double y = (i / 2) * 5 + (i - i / 2) + (double)5 / 2 - (double)1 / 2;
		y = y * size; // y값을 화면에 맞게 설정
		for (int j = 0; j < WIDTH; j++) {
			double x = (j / 2) * 5 + (j - j / 2) + (double)5 / 2 - (double)1 / 2;
			x = x * size; //x값도 화면에 맞게 설정
			if (result_path[i][j]) {//해당 좌표가 해답에 해당한다면 1을 저장하고 있으므로 해당 좌표가 해답인지 판별
				ofSetColor(ofColor::white);//흰색으로 미로를 그려줌
				if ((i % 2) && (j % 2)) { //두 좌표 모두 홀수이면 모퉁이이므로 정사각형으로 표시
					ofDrawRectangle(x, y, size, size);
				}
				else if (i % 2) {//i만 홀수이면 가로 벽을 그려줌
					ofDrawRectangle(x + size - 5 * size, y, 5 * size, size);
				}
				else {//그 외의 경우라면 세로 벽을 그려줌
					ofDrawRectangle(x, y + size - 5 * size, size, 5 * size);
				}
			}
		}
	}

}
void ofApp::init_point() {//움직이는 점의 위치는 처음 위치로 초기화해주고, 출발지와 목적지의 좌표를 알맞게 설정해주는 함수
	point = make_pair(1, 1);//움직이는 점 설정
	start = make_pair(1, 1);//출발지 설정
	end = make_pair(HEIGHT - 2, WIDTH - 2);//목적지 설정
}
void ofApp::draw_point() {//점을 움직일 때마다 갱신되는 점의 좌표로 화면에 점을 그려주는 함수, 목적지점도 점으로 표시해준다.

	int i, j;

	i = end.first;//목적지의 y좌표
	j = end.second;//목적지의 x좌표
	ofSetColor(ofColor::red);//목적지는 빨간 점으로 표시
	//각 e_x,e_y를 화면에 알맞게 설정
	double e_x = (j / 2) * 5 + (j - j / 2) + (double)5 / 2 - (double)1 / 2;
	double e_y = (i / 2) * 5 + (i - i / 2) + (double)5 / 2 - (double)1 / 2;
	e_x = e_x * size;
	e_y = e_y * size;

	ofDrawCircle(e_x, e_y, 5 * size / 3);//목적지점 그려주기

	i = point.first;//움직이는 점의 y좌표
	j = point.second;//움직이는 점의 x좌표
	ofSetColor(ofColor::yellow);//움직이는 점은 노란색으로 표시
	//각 p_x, p_y를 화면에 알맞게 설정
	double p_x = (j / 2) * 5 + (j - j / 2) + (double)5 / 2 - (double)1 / 2;
	double p_y = (i / 2) * 5 + (i - i / 2) + (double)5 / 2 - (double)1 / 2;
	p_x = p_x * size;
	p_y = p_y * size;

	ofDrawCircle(p_x, p_y, 5 * size / 3);//움직이는 점 그려주기

	
	if (point.first == end.first && point.second == end.second) { //움직이는 점의 좌표와 목적지의 좌표가 같은 경우, 즉 미로의 해답을 찾은 경우
		finish_flag == true; //flag 설정
		finish();//미로를 클리어했다는 이미지를 보여주는 함수
	}

}

void ofApp::draw_maze() { //움직이는 점의 현재 위치에 따라서 점을 포함한 최대 3x3의 해당하는 미로의 영역만을 그려주는 함수
	int i, j;

	if (point.first - 3 < 0) i = 0;//현재 움직이는 점의 y좌표가 너무 위에 있어서 위로 3칸은 보여주지 못하는 경우
	else i = point.first - 3;//위 경우가 아니라면 y-3좌표부터 미로를 그려줌

	for ( ; i <= point.first + 3 && i < HEIGHT; i++) { //현재 위치에서 최대 y+3좌표, y+3이 끝바닥을 넘어간다면 끝바닥까지만 미로를 그려주는 반복문
		int maze_y = (i / 2) * 5 + (i - i / 2);
		maze_y = maze_y * size;//maze_y를 화면에 알맞게 설정

		if (point.second - 3 < 0) j = 0;//현재 움직이는 점의 x좌표가 너무 위에 있어서 위로 3칸은 보여주지 못하는 경우
		else j = point.second - 3;//위 경우가 아니라면 x-3좌표부터 미로를 그려줌

		for ( ; j <= point.second + 3 && j < WIDTH; j++) {//현재 위치에서 최대 x+3좌표, x+3이 끝바닥을 넘어간다면 끝바닥까지만 미로를 그려주는 반복문
			int maze_x = (j / 2) * 5 + (j - j / 2);
			maze_x = maze_x * size;//maze_x를 화면에 알맞게 설정
			ofSetColor(ofColor::white);//흰색으로 미로를 그려줌

			switch (input[i][j]) {
			case '+': //input에 '+'이 저장되어 있었다면 모퉁이이므로 정사각형을 그려줌
				ofDrawRectangle(maze_x, maze_y, size, size);
				break;
			case'-'://input에 '-'이 저장되어 있었다면 가로벽을 그려줌
				ofDrawRectangle(maze_x, maze_y, 5 * size, size);
				break;
			case'|'://input에 '|'이 저장되어 있었다면 세로벽을 그려줌
				ofDrawRectangle(maze_x, maze_y, size, 5 * size);
				break;
			}
		}
	}
}

void ofApp::finish() //움직이는 점의 좌표가 목적지에 도달했을 경우 실행되는 함수
{
	hint_count = 3;//만약 현재 힌트를 몇 번 쓴 경우라면 다시 한 번 미로 찾기 게임을 시작할 수 있기 때문에 hint_count를 3으로 설정
	game_clear.draw(ofGetHeight() / 3, ofGetHeight() / 3,ofGetWidth()/2, ofGetWidth()/2); //미로를 클리어한 것을 축하하는 이미지와 다시 하는 법을 알려주는 이미지를 출력
	
}
