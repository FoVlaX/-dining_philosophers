/*
##################
Обедающие философы с параметром число философов
Время голодания и приема пищи для каждого философа различно и задается случайным образом при создании филососфа
Linux version
##################
*/

#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <thread>
using namespace std;


enum State { EATING, THINKING  }; //состояние философа ест или думает
enum Hungry { FULL, HUNGRY }; //  состояние "организма" философа сытый или голодный

class waiter { //"официант" with семафорom регулирующий подачу вилок
public:
	waiter(int s) {
		S = s; //задаем cколько одновременно можно держaть вилок  n - 1, где n  кол-во философов
	}
	~waiter() {

	}
	void P() { //философ собщил что взял вилку
		while (S == 0);//блокируем поток вызвавший функцию пока не появятся вилки, которые можно взять
		S--; //берем вилку
	}

	void V() {
		S++; //увеличиваем кол-во вилок которые можно взять
	}
	int getS() {
		return S;
	}
protected:
	int S;//semafore
};

class Fork : public waiter { //вилка c семафорom, чтобы два философа не смогли взять одновременно одну и ту же вилку и войти вместе в критический участок
public:

	Fork(int s) : waiter(s) {
		id = 0;
	}
	int id; //кто взял вилку, 0 - никто
};


class philosopher {
public:
	static int globalid;
	philosopher(Fork *l, Fork *r, waiter *w, int hT, int fT) { //в конструкторе передаем ссылки на правую и левую вилки если значение по этим адресам = 0 - вилка свободна,\
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
					usleep(fTime); //ест fTime microseconds
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
				usleep(hTime); //сытость сохраняется hTime microseconds
				hungry = HUNGRY; //проголодался
			}
		}
	}
	void printStates(int x, int y) { //выводит состояние философа
		move(y,x);
		attron(COLOR_PAIR(3));
		
		if (state == EATING) {
			attron(COLOR_PAIR(5));
		}
		if (state == THINKING && hungry == HUNGRY) {
			attron(COLOR_PAIR(4));
		}
		printw("%s%i%s","Philosopher #" ,id, " is ");
		if (hungry == HUNGRY) printw("HUNGRY and ");
		else printw("FULL and ");
		if (state == THINKING) printw("THINKING       ");
		else printw("EATING         ");
	}
	void setTimes(int hT,int fT) {
		fTime = fT;
		hTime = hT;
	}

protected: 
	int id;
	State state; //думает или ест
	Hungry hungry; // сытый или голодный
	Fork* left; //вилка слева
	Fork* right; //вилка справа
	waiter* W; // официант;
	int hTime; // время сытости философа в microсекундах
	int fTime; // время питания в microсекундах

};

int philosopher::globalid = 1;

void foo(philosopher* p) { //функция передаваемая в поток
	p->run();
}

int main() {
	
	initscr(); //init ncurses
	setlocale(LC_ALL, "");
	start_color();
	
        init_pair(4,  COLOR_YELLOW,  COLOR_BLACK);
        init_pair(5,  COLOR_BLUE,    COLOR_BLACK);
        init_pair(6,  COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7,  COLOR_CYAN,    COLOR_BLACK);
        init_pair(8,  COLOR_BLUE,    COLOR_BLACK);
        init_pair(9,  COLOR_WHITE,   COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK );
	init_pair(2, COLOR_BLUE, COLOR_BLACK );
	init_pair(1, COLOR_BLUE, COLOR_BLACK );
	init_pair(10, COLOR_WHITE, COLOR_BLACK );
	init_pair(11, COLOR_RED, COLOR_BLACK ); //set colours
	attron(COLOR_PAIR(10));
	move(0,0);
	printw("Write philosopher's count: ");
	refresh();
	
	int n = 5;
	//вводим параметр
	scanf("%i",&n);

	curs_set(0);
	Fork** Forks = new Fork*[n]; //создаем вилки
	for (int i = 0; i < n; i++) {
		Forks[i] = new Fork(1); // 1 - значение для S
	}
	philosopher **phil = new philosopher*[n];// создаем массив указателей на философов
	waiter Water(n - 1); //официант //разрешаем брать n-1 вилки слева исключая когда случай когда все возьмут вилки слева и заблокируются \
	философ сначала берет вилку слева, пока он не взял вилку слева он не возмет вилку справа т.е. ситуации все взяли вилку справа не будет
	
	thread** threads = new thread *[n]; //создаем массив указателей на потоки для каждого философа
	
	for (int i = 0; i < n; i++) {
		Fork* l; //левая вилка
		Fork* r; //правая вилка

		l = Forks[i];
		
		if (i == n - 1) { //последнему философу правая вилка левая от первого
			r = Forks[0];
		}
		else {
			r = Forks[i + 1];
		}

		phil[i] = new philosopher(l,r,&Water,(rand()%9000+1000)*1000,(rand()%9000+2000)*1000);
		threads[i] = new thread(foo, phil[i]);
		threads[i]->detach();
	}

	

	while (true) {
		for (int i = 0; i < n; i++) {
			
			
			move(2+2*i,12);
			attron(COLOR_PAIR(11));
			if (Forks[i]->id == 0) {
				printw("%s%i%s","Fork #" , i+1 , " on table                              ");
			}
			else {
				attron(COLOR_PAIR(5));
				printw("%s%i%s%i","Philosopher ", Forks[i]->id, " take Fork #",  i+1);
				
			}
			
			phil[i]->printStates(2, 3 + 2 * i);
		}

		int i = 0;
		move(2+2*n,12);
		attron(COLOR_PAIR(11));
		if (Forks[i]->id == 0) {
			printw("%s%i%s","Fork #" , i+1 , " on table                              ");
		}
		else {
			attron(COLOR_PAIR(5));
			printw("%s%i%s%i","Philosopher ", Forks[i]->id , " take Fork #",  i+1);
		}
		move(2+2*(n+2),12);
		attron(COLOR_PAIR(11));
		printw("%s%i%s","Free left Forks: " , Water.getS() , "            ");
		refresh();
	}
	endwin(); 
	return 0;
}
