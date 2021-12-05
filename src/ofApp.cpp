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
	ofBackground(0, 0, 0, 0);//��ο� ������ �����̴� ���� ��ó�� �̷ΰ� ���̰� �� ���̹Ƿ� ����� ���������� ����
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();

	finish_flag = false;//flag �ʱ�ȭ
	play_flag = false;
	hint_flag = false;
	isOpen = 0;

	hint_count = 3;//hint_count �ʱ�ȭ
	init_point();//�����̴� ���� ��ġ�� ó�� ��ġ�� �ʱ�ȭ

	game_clear.load("images/game_clear.png");//�������� ���� ���� �� ������ �̹��� �ε�

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
void ofApp::appMenuFunction(string title, bool bChecked) { //������ �޴��� ���� ������ �����Ѵ�.

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		if (isOpen) {
			freeMemory();//�޸� �Ҵ� ����
			finish_flag = false;//flag �ʱ�ȭ
			play_flag = false;

			hint_count = 3;//hint_count 3���� �ʱ�ȭ
		}
		readFile();//������ �о�帮�� ������ input�� ����
	}
	if (title == "Exit") {

		freeMemory();//�޸� �Ҵ� ����
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//

	if (title == "Play") {
		if (isOpen) {
			play_flag = true;//������ �����Ͽ����Ƿ� true�� ����
			finish_flag = false;//finish_flag �ʱ�ȭ
			init_point();//���� ��ġ �ʱ�ȭ
		}
		else {
			cout << "you must open file first" << endl;
		}
	}
	if (title == "Hint") {
		if (isOpen) {
			DFS();//�� ������Ʈ���� �ٷ�� �̷��� Ư���� bfs���� dfs�� �� ȿ���� �����Ƿ� ��Ʈ�� �˷��ֱ� ���� �̷��� �ش��� ���
			hint_flag = true;//hint ������� ǥ��
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
void ofApp::update() {//windowâ�� ũ�⿡ �´� �̷θ� �׸��� ���� size �������ִ� �Լ�
	
	size = (double)ofGetHeight() / (double)(5 * (HEIGHT / 2) + (HEIGHT - HEIGHT / 2));
	if (size > (double)ofGetWidth() / (double)(5 * (WIDTH / 2) + (WIDTH - WIDTH / 2))) {
		size = (double)ofGetWidth() / (double)(5 * (WIDTH / 2) + (WIDTH - WIDTH / 2));
	}
}


//--------------------------------------------------------------
void ofApp::draw() {//��Ʈ�� ����ߴٸ� ��� ���� �����ְ�, ������ ����ǰ� �ִ� ���̶�� �ٲ�� ���� ��ġ�� �׸��� �Լ��� ȣ���Ѵ�.

	ofBackground(0, 0, 0, 0);//��ο� ������ �����̴� ���� ��ó�� �̷ΰ� ���̰� �� ���̹Ƿ� ����� ���������� ����

	if (hint_flag)//��Ʈ�� ����ߴٸ� ��� ���� �̷��� �ش��� �����ش�.
	{
		ofSetLineWidth(5);
		if (isOpen) {
			if (hint_count > 0) {
				hint_count--;//���� ��Ʈ�� ������ 1 ����
				printf("Number of hints available : %d\n", hint_count);
				hintdraw();//�̷��� �ش��� �׷��ִ� �Լ�
			}
		}
		else
			cout << "You must open file first" << endl;
		hint_flag = false;
	}
	
	draw_maze();//���� ������ �ִ� ���� ��ġ�κ��� ������ �̷��� ���¸� �����ش�.

	if (play_flag) draw_point();//���� ��ġ�� �׷��ִ� �Լ�

} // end Draw


void ofApp::doFullScreen(bool bFull) //��üȭ��� ���õ� �Լ��̴�.
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
	if (key == VK_ESCAPE) {//ESC Ű�� ��üȭ�� ����, Ȥ�� ��üȭ�� ���� �ƴ϶�� ����
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			freeMemory();//�޸� �Ҵ� ����
			ofExit();//����
		}
	}
	if (key == OF_KEY_UP) {//���� ����Ű�� ������ ��
		if (play_flag) {//������ ���� ���� ��
			if (finish_flag)return;//���� ���� ��ǥ�� �������� �������ִٸ� ����
			int p_x = point.second;//p_x�� ���� ���� x��ǥ ����
			int p_y = point.first;//p_y�� ���� ���� y��ǥ ����
			if (input[p_y - 1][p_x] == ' ') {//���� ���� �շ��ִ��� Ȯ��
				point.first = point.first - 2;//���� �շ� �ִٸ� y���� 2��ŭ �������� �����̴� ���� ���� ������ �̵�
			}
		}
	}
	if (key == OF_KEY_DOWN) {//�Ʒ��� ����Ű�� ������ ��
		if (play_flag) {//������ ���� ���� ��
			if (finish_flag)return;// ���� ���� ��ǥ�� �������� �ش��Ѵٸ� ����
			int p_x = point.second;//p_x�� ���� ���� x��ǥ ����
			int p_y = point.first; //p_y�� ���� ���� y��ǥ ����
			if (input[p_y + 1][p_x] == ' ') {//�Ʒ��� ���� �շ��ִ��� Ȯ��
				point.first = point.first + 2;//���� �շ� �ִٸ� y���� 2��ŭ �������� �����̴� ���� �Ʒ��� ������ �̵�
			}
		}
	}
	if (key == OF_KEY_LEFT) {//���� ����Ű�� ������ ��
		if (play_flag) {//������ ���� ���� ��
			if (finish_flag)return;//���� ���� ��ǥ�� �������� �ش��Ѵٸ� ����
			int p_x = point.second;//p_x�� ���� ���� x��ǥ ����
			int p_y = point.first;//p_y�� ���� ���� y��ǥ ����
			if (input[p_y][p_x - 1] == ' ') {//���� ���� �շ��ִ��� Ȯ��
				point.second = point.second - 2;//���� �շ� �ִٸ� x���� 2��ŭ �������� �����̴� ���� ���� ������ �̵�
			}
		}
	}
	if (key == OF_KEY_RIGHT) {//������ ����Ű�� ������ ��
		if (play_flag) {//������ ���� ���� ��
			if (finish_flag)return;//���� ���� ��ǥ�� �������� �ش��Ѵٸ� ����
			int p_x = point.second;//p_x�� ���� ���� x��ǥ ����
			int p_y = point.first;//p_y�� ���� ���� y��ǥ ����
			if (input[p_y][p_x + 1] == ' ') {//������ ���� �շ��ִ��� Ȯ��
				point.second = point.second + 2;//���� �շ� �ִٸ� x���� 2��ŭ �������� �����̴� ���� ������ ������ �̵�
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
bool ofApp::readFile()//������ �а� input�� ������ �����ϴ� �Լ�
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
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {//.maz���� �϶��� �б�

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
				//������ ���� �� �о line�� ����
				for (ofBuffer::Line temp = buffer.getLines().begin(), end = buffer.getLines().end(); temp != end; temp++) {
					string line = *temp;
					vector<char> words(line.begin(), line.end());//line ��ü�� words�� charŸ���� ����
					input.emplace_back(words);//input�� words�� ���� ����, ���� input�� ���� ����
				}
			}

			HEIGHT = input.size();//���̸� ����
			WIDTH = input[0].size();//�ʺ� ����

			//��Ʈ�� ���� �̷��� �ش��� ã�� dfs�� �ʿ��� �迭���� ���� �̷��� ũ�⿡ �°� �Ҵ�
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
void ofApp::freeMemory() {//�޸𸮸� �Ҵ� ����, �ʱ�ȭ ���ִ� �Լ�
	HEIGHT = 0;//���� �ʱ�ȭ
	WIDTH = 0;//�ʺ� �ʱ�ȭ
	//���� dfs()�� hintdraw()���� ����ߴ� �迭���� �Ҵ�����
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

bool ofApp::DFS()//DFSŽ���� �ϴ� �Լ�
{
	//�� �迭���� �ʱ�ȭ
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
	A.emplace(make_pair(1, 1));//dfs�� ���� stack�� ����

	visited[1][1] = 1;//ó�� ������� �湮�� ������ ó��

	vector<vector<pair<int, int>>> cell;
	cell.resize(HEIGHT);//������ �湮�� ���� ����ϱ� ���� �迭
	for (int i = 0; i < HEIGHT; i++) {
		cell[i].resize(WIDTH);
	}//�ʱ�ȭ

	bool flag = false;//�������� �����ߴ��� �������ִ� flag �ʱ�ȭ

	Sy = HEIGHT - 2;
	Sx = WIDTH - 2;//�� Sy�� Sx�� ������ ��ǥ�� �ش�

	while (A.size() && !flag) {//���� A�� �ƹ��� ������ ���ų� flag�� true�� ����
		int y = A.top().first;
		int x = A.top().second;//���� A.top�� �ִ� ��ǥ�� ����
		path[y][x] = 1;//�湮�� ���̹Ƿ� �� ������ ���� �ȴ�. ���� path�� ����
		A.pop();//���� A�� pop����

		for (int i = 0; i < 4; i++) {//��,��,��,���� ������ �ֺ� ����� �˻��ϴ� �ݺ��� 
			int temp_y = y + move_y[i];
			int temp_x = x + move_x[i];//�˻��� ���� ��ǥ�� temp_x, temp_y�� ����

			if (visited[temp_y][temp_x] || input[temp_y][temp_x] != ' ')//�湮�ߴ� ���̰ų� ���������� �ѱ��
				continue;

			visited[temp_y][temp_x] = 1;//�� ��쿡 �ش����� �����Ƿ� �湮�� ���� ��. �湮������ ����
			A.emplace(make_pair(temp_y, temp_x));//���� A�� �� ��ǥ�� ����
			cell[temp_y][temp_x] = make_pair(y, x);//���� ��ǥ�� ���� ��ǥ�� ����

			if (temp_y == Sy && temp_x == Sx) {//�������� �����ߴٸ� flag�� �������ְ� break���ֱ�
				flag = true;
				break;
			}
		}
	}

	if (flag) {//���������� �������� �����ߴٸ�
		int y = Sy;
		int x = Sx;//x,y�� ������ ��ǥ�� ����
		result_path[y][x] = 1;//�������̹Ƿ� �̷��� �ش信 �ش���. ���� result_path�� ��ǥ ����

		while (y != 1 || x != 1) {//�������������� �Ųٷ� ��������� �ö󰡸鼭 �ش信 �ش��ϴ� ��ǥ�� ����
			int cy = cell[y][x].first;
			int cx = cell[y][x].second;
			result_path[cy][cx] = 1;
			y = cy, x = cx;
		}
	}
	return false;
}

void ofApp::hintdraw()//�̷��� �ش��� ª�� �ð� ���������� ȭ�鿡�� �����ִ� �Լ�
{
	for (int i = 0; i < HEIGHT; i++) {
		double y = (i / 2) * 5 + (i - i / 2) + (double)5 / 2 - (double)1 / 2;
		y = y * size; // y���� ȭ�鿡 �°� ����
		for (int j = 0; j < WIDTH; j++) {
			double x = (j / 2) * 5 + (j - j / 2) + (double)5 / 2 - (double)1 / 2;
			x = x * size; //x���� ȭ�鿡 �°� ����
			if (result_path[i][j]) {//�ش� ��ǥ�� �ش信 �ش��Ѵٸ� 1�� �����ϰ� �����Ƿ� �ش� ��ǥ�� �ش����� �Ǻ�
				ofSetColor(ofColor::white);//������� �̷θ� �׷���
				if ((i % 2) && (j % 2)) { //�� ��ǥ ��� Ȧ���̸� �������̹Ƿ� ���簢������ ǥ��
					ofDrawRectangle(x, y, size, size);
				}
				else if (i % 2) {//i�� Ȧ���̸� ���� ���� �׷���
					ofDrawRectangle(x + size - 5 * size, y, 5 * size, size);
				}
				else {//�� ���� ����� ���� ���� �׷���
					ofDrawRectangle(x, y + size - 5 * size, size, 5 * size);
				}
			}
		}
	}

}
void ofApp::init_point() {//�����̴� ���� ��ġ�� ó�� ��ġ�� �ʱ�ȭ���ְ�, ������� �������� ��ǥ�� �˸°� �������ִ� �Լ�
	point = make_pair(1, 1);//�����̴� �� ����
	start = make_pair(1, 1);//����� ����
	end = make_pair(HEIGHT - 2, WIDTH - 2);//������ ����
}
void ofApp::draw_point() {//���� ������ ������ ���ŵǴ� ���� ��ǥ�� ȭ�鿡 ���� �׷��ִ� �Լ�, ���������� ������ ǥ�����ش�.

	int i, j;

	i = end.first;//�������� y��ǥ
	j = end.second;//�������� x��ǥ
	ofSetColor(ofColor::red);//�������� ���� ������ ǥ��
	//�� e_x,e_y�� ȭ�鿡 �˸°� ����
	double e_x = (j / 2) * 5 + (j - j / 2) + (double)5 / 2 - (double)1 / 2;
	double e_y = (i / 2) * 5 + (i - i / 2) + (double)5 / 2 - (double)1 / 2;
	e_x = e_x * size;
	e_y = e_y * size;

	ofDrawCircle(e_x, e_y, 5 * size / 3);//�������� �׷��ֱ�

	i = point.first;//�����̴� ���� y��ǥ
	j = point.second;//�����̴� ���� x��ǥ
	ofSetColor(ofColor::yellow);//�����̴� ���� ��������� ǥ��
	//�� p_x, p_y�� ȭ�鿡 �˸°� ����
	double p_x = (j / 2) * 5 + (j - j / 2) + (double)5 / 2 - (double)1 / 2;
	double p_y = (i / 2) * 5 + (i - i / 2) + (double)5 / 2 - (double)1 / 2;
	p_x = p_x * size;
	p_y = p_y * size;

	ofDrawCircle(p_x, p_y, 5 * size / 3);//�����̴� �� �׷��ֱ�

	
	if (point.first == end.first && point.second == end.second) { //�����̴� ���� ��ǥ�� �������� ��ǥ�� ���� ���, �� �̷��� �ش��� ã�� ���
		finish_flag == true; //flag ����
		finish();//�̷θ� Ŭ�����ߴٴ� �̹����� �����ִ� �Լ�
	}

}

void ofApp::draw_maze() { //�����̴� ���� ���� ��ġ�� ���� ���� ������ �ִ� 3x3�� �ش��ϴ� �̷��� �������� �׷��ִ� �Լ�
	int i, j;

	if (point.first - 3 < 0) i = 0;//���� �����̴� ���� y��ǥ�� �ʹ� ���� �־ ���� 3ĭ�� �������� ���ϴ� ���
	else i = point.first - 3;//�� ��찡 �ƴ϶�� y-3��ǥ���� �̷θ� �׷���

	for ( ; i <= point.first + 3 && i < HEIGHT; i++) { //���� ��ġ���� �ִ� y+3��ǥ, y+3�� ���ٴ��� �Ѿ�ٸ� ���ٴڱ����� �̷θ� �׷��ִ� �ݺ���
		int maze_y = (i / 2) * 5 + (i - i / 2);
		maze_y = maze_y * size;//maze_y�� ȭ�鿡 �˸°� ����

		if (point.second - 3 < 0) j = 0;//���� �����̴� ���� x��ǥ�� �ʹ� ���� �־ ���� 3ĭ�� �������� ���ϴ� ���
		else j = point.second - 3;//�� ��찡 �ƴ϶�� x-3��ǥ���� �̷θ� �׷���

		for ( ; j <= point.second + 3 && j < WIDTH; j++) {//���� ��ġ���� �ִ� x+3��ǥ, x+3�� ���ٴ��� �Ѿ�ٸ� ���ٴڱ����� �̷θ� �׷��ִ� �ݺ���
			int maze_x = (j / 2) * 5 + (j - j / 2);
			maze_x = maze_x * size;//maze_x�� ȭ�鿡 �˸°� ����
			ofSetColor(ofColor::white);//������� �̷θ� �׷���

			switch (input[i][j]) {
			case '+': //input�� '+'�� ����Ǿ� �־��ٸ� �������̹Ƿ� ���簢���� �׷���
				ofDrawRectangle(maze_x, maze_y, size, size);
				break;
			case'-'://input�� '-'�� ����Ǿ� �־��ٸ� ���κ��� �׷���
				ofDrawRectangle(maze_x, maze_y, 5 * size, size);
				break;
			case'|'://input�� '|'�� ����Ǿ� �־��ٸ� ���κ��� �׷���
				ofDrawRectangle(maze_x, maze_y, size, 5 * size);
				break;
			}
		}
	}
}

void ofApp::finish() //�����̴� ���� ��ǥ�� �������� �������� ��� ����Ǵ� �Լ�
{
	hint_count = 3;//���� ���� ��Ʈ�� �� �� �� ����� �ٽ� �� �� �̷� ã�� ������ ������ �� �ֱ� ������ hint_count�� 3���� ����
	game_clear.draw(ofGetHeight() / 3, ofGetHeight() / 3,ofGetWidth()/2, ofGetWidth()/2); //�̷θ� Ŭ������ ���� �����ϴ� �̹����� �ٽ� �ϴ� ���� �˷��ִ� �̹����� ���
	
}
