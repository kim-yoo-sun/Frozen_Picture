#define _CRT_SECURE_NO_WARNINGS

#include <bangtal>
#include <cstdlib>
#include <ctime>

using namespace bangtal;

ScenePtr scene;
ObjectPtr game_board[16] = { 0, }, game_original[16] = { 0, };
ObjectPtr start = 0;

TimerPtr timer, timer_inst, timer_inc;
float animationTime = 0.1f;
int mixCount = 3;
	
int blank;
int time_inc = 0;

int game_index(ObjectPtr piece) {
	for (int i = 0; i < 16; i++)
		if (game_board[i] == piece) return i;
	return -1;
}

int index_to_x(int index) {
	return (300 + 150 * (index % 4));
}

int index_to_y(int index) {
	return (470 - 150 * (index / 4));
}

void game_move(int index) {
	auto piece = game_board[index];
	game_board[index] = game_board[blank];
	game_board[index]->locate(scene, index_to_x(index), index_to_y(index));
	game_board[blank] = piece;
	game_board[blank]->locate(scene, index_to_x(blank), index_to_y(blank));
	blank = index;
}

bool check_move(int index) {
	if (blank % 4 > 0 && index == blank - 1) return true;
	if (blank % 4 < 3 && index == blank + 1) return true;
	if (blank / 4 > 0 && index == blank - 4) return true;
	if (blank / 4 < 3 && index == blank + 4) return true;

	return false;
}

int random_move() {
	int index = rand() % 16;
	while (!check_move(index)) {
		index = rand() % 16;
	}
	return index;
}

void start_game() {
	mixCount = 3;
	timer->set(animationTime);
	timer->start();
	blank = 15;
	game_board[blank]->hide();
	start->hide();
}

bool check_end() {
	for (int i = 0; i < 16; i++) {
		if (game_board[i] != game_original[i]) return false;
	}
	return true;
}

void end_game() {
	start->show();
	game_board[blank]->show();
	showMessage("Completed!");
}

void init_game() {
	scene = Scene::create("겨울왕국 퍼즐", "Images/background.png");
	
	timer_inc = Timer::create(time_inc);
	timer_inc->setOnTimerCallback([&](auto)->bool {
		time_inc++;
		printf("%d", time_inc);
		timer_inst->set(1);
		timer_inst->start();
		timer_inc->set(time_inc);
		showTimer(timer_inc);
		if (check_end()) {
			timer_inc->stop();
		}
		return true;
	});
	
	timer_inst = Timer::create(1);
	timer_inst->setOnTimerCallback([&](auto)->bool {
		timer_inc->set(0);
		timer_inc->start();
		if (check_end()) {
			timer_inc->stop();
			printf("\n");
		}
		return true;
	});
	
	char path[20];
	for (int i = 0; i < 16; i++) {
		sprintf(path, "Images/%d.png", i + 1);
		game_board[i] = Object::create(path, scene, index_to_x(i), index_to_y(i));
		game_board[i]->setOnMouseCallback([&](auto piece, auto x, auto y, auto action)-> bool {
			int index = game_index(piece);
			if (check_move(index)) {
				game_move(index);
				if (check_end()) {
					end_game();
				}
			}
			return true;
		});
		game_original[i] = game_board[i];
	}
	
	start = Object::create("Images/start.png", scene, 590, 100);
	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		start_game();
		time_inc = 0;
		timer_inc->start();
		return true;
	});

	timer = Timer::create(animationTime);
	timer->setOnTimerCallback([&](auto)->bool {
		game_move(random_move());
		mixCount--;
		if (mixCount > 0) {
			timer->set(animationTime);
			timer->start();
		}
		return true;
	});

	startGame(scene);
}

int main() {
	srand((unsigned int)time(NULL));

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	init_game();
	return 0;
}