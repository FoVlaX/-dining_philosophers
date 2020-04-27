/*
##################
��������� �������� � ���������� ����� ���������
����� ��������� � ������ ���� ��� ������� �������� �������� � �������� ��������� ������� ��� �������� ���������
##################
*/


#include <windows.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
using namespace std;


enum State { EATING, THINKING  }; //��������� �������� ��� ��� ������
enum Hungry { FULL, HUNGRY }; //  ��������� "���������" �������� ����� ��� ��������

class waiter { //"��������" ������� ������������ ������ �����
public:
	waiter(int s) {
		S = s; //������ c������ ������������ ����� ������� �����  n - 1, ��� n  ���-�� ���������
	}
	~waiter() {

	}
	void P() { //������� ������ ��� ���� �����
		while (S == 0);//���������� ����� ��������� ������� ���� �� �������� �����, ������� ����� �����
		S--; //����� �����
	}

	void V() {
		S++; //����������� ���-�� ����� ������� ����� �����
	}
	int getS() {
		return S;
	}
protected:
	int S;
};

class fork : public waiter { //�����, ������� ��� �����, ����� ��� �������� �� ������ ����� ������������ ���� � �� �� ����� � ����� ������ � ����������� �������
public:

	fork(int s) : waiter(s) {
		id = 0;
	}
	int id; //��� ���� �����, 0 - �����
};


class philosopher {
public:
	static int globalid;
	philosopher(fork *l, fork *r, waiter *w, int hT, int fT) { //� ������������ �������� ������ �� ������ � ����� ����� ���� �������� �� ���� ������� = 0 - ����� ��������,\
								   ����� ����� ��������� � id ������ ��������� �� ����� ������
		state = THINKING; //����� ������� ������� �� ���� �� ���������� ������ 		
		hungry = HUNGRY; // � ��������
		left = l; 
		right = r;
		id = globalid++; //������ id ��������
		W = w;
		setTimes(hT, fT); //������ ����� ������� � ����� ������ ����
	}
	~philosopher() {

	}
	void run(){ //�������� ������� ����������� ��������� ��������
		while (true) {
			if (hungry == HUNGRY) { //���� �������
				if (state == THINKING) { //���� ������ 					
					
					W->P(); //���� ���������� ����� ����� �����
					//�������� ���������, ��� ������ ����� ����� ��� �����������
					left->P(); //���� ����� ����� ����������(�����)
					left->id = id; //����� �����, ����� ��� ��������(�����)
									
					right->P();//���� ����� ����� ����������(������)
					right->id = id;//����� �����, ����� ��� ��������(������)
					
					state = EATING; //�������� ����
				}
				else { //���� ���
					this_thread::sleep_for(chrono::milliseconds(fTime)); //��� fTime ����������
					left->V(); //������� ����� �����
					left->id = 0;
					W->V(); //�������� ��������� ��� �������� ����� �����
					right->V(); //������� ������ �����
					right->id = 0;
				
					state = THINKING; //���� ������
					hungry = FULL; //���� �����
				}
			}
			else { //���� ���
				this_thread::sleep_for(chrono::milliseconds(hTime)); //������� ����������� hTime ����������
				hungry = HUNGRY; //������������
			}
		}
	}
	void printStates(int x, int y) { //������� ��������� ��������
		COORD c;
		c.X = x;
		c.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		if (state == EATING) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
		}
		if (state == THINKING && hungry == HUNGRY) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
		}
		cout << " Philosopher �" << id << " is ";
		if (hungry == HUNGRY) cout << "HUNGRY and ";
		else cout << "FULL and ";
		if (state == THINKING) cout << "THINKING       \n";
		else cout << "EATING         \n";
	}
	void setTimes(int hT,int fT) {
		fTime = fT;
		hTime = hT;
	}
	
protected:
	int id;
	State state; //������ ��� ���
	Hungry hungry; // ����� ��� ��������
	fork* left; //����� �����
	fork* right; //����� ������
	waiter* W; // ��������;
	int hTime; // ����� ������� �������� � ������������
	int fTime; // ����� ������� � ������������

};

int philosopher::globalid = 1;

void foo(philosopher* p) { //������� ������������ � �����
	p->run();
}

int main() {
	
	setlocale(LC_ALL, "Russian");
	cout << "������� ���-�� ���������: ";
	int n;
	cin >> n; //������ ��������
	fork** forks = new fork*[n]; //������� �����
	for (int i = 0; i < n; i++) {
		forks[i] = new fork(1); // 1 - �������� ��� S
	}
	philosopher **phil = new philosopher*[n];// ������� ������ ���������� �� ���������
	waiter Water(n - 1); //�������� //��������� ����� n-1 ����� ����� �������� ����� ������ ����� ��� ������� ����� ����� � ������������� \
	������� ������� ����� ����� �����, ���� �� �� ���� ����� ����� �� �� ������ ����� ������ �.�. �������� ��� ����� ����� ������ �� �����
	thread** threads = new thread *[n]; //������� ������ ���������� �� ������ ��� ������� ��������
	for (int i = 0; i < n; i++) {
		fork* l; //����� �����
		fork* r; //������ �����

		l = forks[i];
		
		if (i == n - 1) { //���������� �������� ������ ����� ����� �� �������
			r = forks[0];
		}
		else {
			r = forks[i + 1];
		}

		phil[i] = new philosopher(l,r,&Water,rand()%900+100,rand()%900+200);
		threads[i] = new thread(foo, phil[i]);
		threads[i]->detach();
	}

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle, &structCursorInfo);
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &structCursorInfo);

	while (true) {
		for (int i = 0; i < n; i++) {
			
			COORD c;
			c.X = 12;
			c.Y = 2 + i*2;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			if (forks[i]->id == 0) {
				cout << "Fork #" << i+1 << " on table                              ";
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
				cout << "Philosopher " << forks[i]->id << " take fork #" << i+1;
				
			}
			
			phil[i]->printStates(2, 3 + 2 * i);
		}

		int i = 0;
		COORD c;
		c.X = 12;
		c.Y = 2 + n * 2;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		if (forks[i]->id == 0) {
			cout << "Fork #" << i + 1 << " on table                               ";
		}
		else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			cout << "Philosopher " << forks[i]->id << " take fork #" << i + 1;
		}
		c.X = 12;
		c.Y = 2 + (n+2) * 2;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		cout<<"Free left forks: " << Water.getS() << "            ";
	}
	
	return 0;
}