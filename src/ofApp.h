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
	bool readFile();//������ �о� input�迭�� �����ϴ� �Լ�
	void freeMemory();//�޸𸮸� �Ҵ� ������ ����ϴ� �Լ�
	bool DFS();//��Ʈ�� �˷��ֱ� ���� �̷��� �ش��� dfs�˰������� ã���ִ� �Լ�
	void hintdraw();// DFS()���� ���� �̷��� �ش��� ȭ�� �� ���� ª�� �ð� ���� �׷��ִ� �Լ�
	void draw_point();//���� ����ڰ� �����̴� ���� ��ġ�� �׷��ִ� �Լ�
	void draw_maze();//���� ���� ��ġ�� �ݿ��Ͽ� �� �ֺ� 3x3�� �ش��ϴ� �̷��� ������ �����ִ� �Լ�
	void finish();//����ڰ� �����̴� ���� �̷��� �������� �������� �� ������ �������� �˸��� �Լ�


	int WIDTH;//�̷��� ����
	int HEIGHT;//�̷��� �ʺ�
	vector<vector<char>> input;//�ؽ�Ʈ ������ ��� ������ ��� ������ �迭
	int** visited;//�湮���θ� ������ ������
	int** path;//���ݱ��� �湮�� ��� ���� �����ϴ� �迭
	int** result_path;//��������� ������������ ���� �����ϴ� �迭
	int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
	int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
	int hint_count;//�� 3���� ��Ʈ���� �����ֱ� ���ؼ� ��Ʈ�� ���� Ƚ���� �����ϴ� ����
	int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� ������ �ʾҰ� 1�̸� ���ȴ�.
	int isDFS;//DFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� �������� �ʾҰ� 1�̸� �����ߴ�.
	bool hint_flag;//BFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� �������� �ʾҰ� 1�̸� �����ߴ�.
	// Menu
	ofxWinMenu* menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	// Used by example app
	ofTrueTypeFont myFont;
	ofImage myImage;
	float windowWidth, windowHeight;//ȭ���� �ʺ�� ���̸� �����ϴ� ����
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;//��üȭ���� ������ ������ �˷��ִ� flag
	bool bTopmost;
	//bool isdfs;

	bool finish_flag;//����ڰ� �����̴� ���� ��ġ�� �̷��� �������� �����ߴ��� �˷��ִ� flag, true�� ����
	bool play_flag;//����ڰ� ������ �����Ͽ����� �˷��ִ� flag, true�� ����

	// Example functions
	void doFullScreen(bool bFull);//��üȭ���� �������ִ� �Լ�
	void init_point();//����ڰ� ������ ���� ��ġ�� ó�� ��ġ�� �ʱ�ȭ���ִ� �Լ�

	double size;//�̷��� size�� �����ϴ� ����
	int move_x[4] = { 0, -1, 0, 1 }; //move_x�� move_y�� ��� �̷��� �ֺ� ����� ��, ��, ��, ���� ������ Ž���ϱ� ���� �迭
	int move_y[4] = { -1, 0, 1, 0 };
	int Sx;//�������� x��ǥ�� ��Ÿ���� ����
	int Sy;//�������� y��ǥ�� ��Ÿ���� ����

	pair<int, int> start, end, point;//���� start�� ����ڰ� ������ ���� �̷��� �����, end�� �̷��� ������, point�� ����ڰ� �����̴� ���� ��ġ�� �����ϴ� ����

	ofImage game_clear;//�������� ����ڰ� �����̴� ���� �̷��� �������� �������� �� ȭ�鿡 ������ �̹���

};
