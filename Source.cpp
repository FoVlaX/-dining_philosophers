/*
##################
Обедающие философы с параметром число философов
Время голодания и приема пищи для каждого философа различно и задается случайным образом при создании филососфа
##################
*/


#include <windows.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
using namespace std;


enum State { EATING, THINKING  }; //состояние философа ест или думает
enum Hungry { FULL, HUNGRY }; //  состояние "организма" философа сытый или голодный

class waiter { //"официант" семафор регулирующий подачу вилок
public:
	waiter(int s) {
		S = s; //задаем cколько одновременно можно деражть вилок  n - 1, где n  кол-во философов
	}
	~waiter() {

	}
	void P() { //философ собщил что взял вилку
		while (S == 0);//болокируем поток вызвавший функцию пока не появятся вилки, которые можно взять
		S--; //берем вилку
	}

	void V() {
		S++; //увеличиваем кол-во вилок которые можно взять
	}
	int getS() {
		return S;
	}
protected:
	int S;
};

class fork : public waiter { //вилка, семафор для вилок, чтобы два философа не смогли взять одновременно одну и ту же вилку и войти вместе в критический участок
public:

	fork(int s) : waiter(s) {
		id = 0;
	}
	int id; //кто взял вилку, 0 - никто
};


class philosopher {
public:
	static int globalid;
	philosopher(fork *l, fork *r, waiter *w, int hT, int fT) { //в конструкторе передаем ссылки на правую и левую вилки если значение по этим адресам = 0 - вилка свободна,\
								   иначе взята философом с id равным значением по этому адресу
		state = THINKING; //когда философ садится за стол он изначально думает 		
		hungry = HUNGRY; // и голодный
		left = l; 
		right = r;
		id = globalid++; //задаем id философу
		W = w;
		setTimes(hT, fT); //задаем время сытости и время приема пищи
	}
	~philosopher() {

	}
	void run(){ //основная функция описывающая поведение философа
		while (true) {
			if (hungry == HUNGRY) { //если голоден
				if (state == THINKING) { //если думает 					
					
					W->P(); //ждем разрешения взять вилку слева
					//сообщаем официанту, что возмем вилку когда она освободится
					left->P(); //ждем когда вилка освобоится(левая)
					left->id = id; //берем вилку, когда она свободна(левая)
									
					right->P();//ждем когда вилка освобоится(правая)
					right->id = id;//берем вилку, когда она свободна(правая)
					
					state = EATING; //начинает есть
				}
				else { //если ест
					this_thread::sleep_for(chrono::milliseconds(fTime)); //ест fTime милисекунд
					left->V(); //положил левую вилку
					left->id = 0;
					W->V(); //сообщаем официанту что положили левую вилку
					right->V(); //положил правую вилку
					right->id = 0;
				
					state = THINKING; //стал думать
					hungry = FULL; //стал сытым
				}
			}
			else { //если сыт
				this_thread::sleep_for(chrono::milliseconds(hTime)); //сытость сохраняется hTime милисекунд
				hungry = HUNGRY; //проголодался
			}
		}
	}
	void printStates(int x, int y) { //выводит состояние философа
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
		cout << " Philosopher №" << id << " is ";
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
	State state; //думает или ест
	Hungry hungry; // сытый или голодный
	fork* left; //вилка слева
	fork* right; //вилка справа
	waiter* W; // официант;
	int hTime; // время сытости философа в милисекундах
	int fTime; // время питания в милисекундах

};

int philosopher::globalid = 1;

void foo(philosopher* p) { //функция передаваемая в поток
	p->run();
}

int main() {
	
	setlocale(LC_ALL, "Russian");
	cout << "Введите кол-во философов: ";
	int n;
	cin >> n; //вводим параметр
	fork** forks = new fork*[n]; //создаем вилки
	for (int i = 0; i < n; i++) {
		forks[i] = new fork(1); // 1 - значение для S
	}
	philosopher **phil = new philosopher*[n];// создаем массив указателей на философов
	waiter Water(n - 1); //официант //разрешаем брать n-1 вилки слева исключая когда случай когда все возьмут вилки слева и заблокируются \
	философ сначала берет вилку слева, пока он не взял вилку слева он не возмет вилку справа т.е. ситуации все взяли вилку справа не будет
	thread** threads = new thread *[n]; //создаем массив указателей на потоки для каждого философа
	for (int i = 0; i < n; i++) {
		fork* l; //левая вилка
		fork* r; //правая вилка

		l = forks[i];
		
		if (i == n - 1) { //последнему философу правая вилка левая от первого
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
