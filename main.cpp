#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>

//Which color does each player has
std::vector<sf::Color> playerColors;

const int GRIDSIZE = 4;
const int SCREENWIDTH = 800;
const int SCREENHEIGHT = 600;
const int WIDTH = 400;
const int HEIGHT = 200;
const int SCREENX = SCREENWIDTH / GRIDSIZE;
const int SCREENY = SCREENHEIGHT / GRIDSIZE;


//Class for every object, that renders
class RenderObject {
protected:
	//It's "sprite", actually it's 4 points of a rectangle
	sf::VertexArray sprite;
public:
	//Coordinates
	int x;
	int y;

	//Id of player (for coloring)
	int player;
	
	//Constructor
	RenderObject() : sprite(sf::Quads, 4) {
		x = 0;
		y = 0;
		player = 0;
	}
	
	//Rendering
	sf::VertexArray& render() {
		//Coloring
		for(int i = 0; i < 4; i++) {
			sprite[i].color = playerColors[player];
		}
		//Positioning
		int _x = x * GRIDSIZE;
		int _y = y * GRIDSIZE;
		sprite[0].position = sf::Vector2f(_x, _y);
		sprite[1].position = sf::Vector2f(_x + GRIDSIZE, _y);
		sprite[2].position = sf::Vector2f(_x + GRIDSIZE, _y + GRIDSIZE);
		sprite[3].position = sf::Vector2f(_x, _y + GRIDSIZE);
		return sprite;
	}
};

std::vector<std::vector<RenderObject*>> map;

enum class Direction {
	Left,
	Up,
	Right,
	Down
};

class Player : public RenderObject {
	Direction dir;
public:
	Player(int id) { 
		player = id;
		dir = Direction::Right;
	}
	
	void changeDir(Direction ndir) {
		if(abs((int)ndir - (int)dir) != 2) {
			dir = ndir;
		}
	}

	void input() {
		//Player input handle
		if(player == 0) {
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				changeDir(Direction::Left);
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				changeDir(Direction::Up);
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				changeDir(Direction::Right);
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				changeDir(Direction::Down);
			}
		}
		//AI input handle
		else {
			if(rand() % 50 == 0) {
				Direction ndir = dir;
				dir = (Direction)(((int)dir + (rand() % 2 ? 1 : -1) + 4) % 4);
				sf::Vector2i npos = move();
				if(collision(npos.x, npos.y)) {
					dir = ndir;
				}
			}
			else {
				sf::Vector2i npos = move();
				if(collision(npos.x, npos.y)) {
					dir = (Direction)(((int)dir + 1) % 4);
				}
			}
		}
	}

	bool collision(int x, int y) {
		if(x >= WIDTH || x < 0 || y >= HEIGHT || y < 0) {
			return true;
		}
		return map[y][x] != nullptr;
	}

	sf::Vector2i move() {
		int x = this->x;
		int y = this->y;
		switch(dir) {
		case Direction::Left:
			x--;
			break;
		case Direction::Up:
			y--;
			break;
		case Direction::Right:
			x++;
			break;
		case Direction::Down:
			y++;
			break;
		}
		return sf::Vector2i(x, y);
	}

	bool update() {
		input();
		map[y][x] = new RenderObject();
		map[y][x]->x = x;
		map[y][x]->y = y;
		map[y][x]->player = player;
		sf::Vector2i pos = move();
		x = pos.x;
		y = pos.y;
		bool val = collision(x, y);
		if(!val) {
			map[y][x] = this;
		}
		return !val;
	}
};

int main() {
	//Set random seed
	srand(time(0));
	//Create window
	sf::RenderWindow window(sf::VideoMode(800, 600), "EVAGame");
	//Resize map
	map = std::vector<std::vector<RenderObject*>>(HEIGHT, std::vector<RenderObject*>(WIDTH, nullptr));
	//Limit framerate
	window.setFramerateLimit(30);
	//How many players there is in game, at least 1
	int playerCount = 6;
	//Resize array of colors, that each player has
	playerColors.resize(playerCount);
	//Color for player
	playerColors[0] = sf::Color::Blue;
	//Colors for AI players
	for(int i = 1; i < playerCount; i++) {
		playerColors[i] = sf::Color::Red;
	}
	//Render array
	sf::VertexArray array(sf::Quads, 4 * SCREENX * SCREENY);
	//Array of players pointers
	std::vector<Player*> players(playerCount, nullptr);
	//Player
	Player* player = new Player(0);
	map[SCREENY / 2][5] = player;
	map[SCREENY / 2][5]->x = 5;
	map[SCREENY / 2][5]->y = SCREENY / 2;
	players[0] = player;
	//Bots
	for(int i = 0; i < playerCount; i++) {
		int x = rand() % (SCREENX - 10) + 5;
		int y = rand() % (SCREENY - 10) + 5;
		if(map[y][x] == nullptr) {
			Player *p = new Player(i + 1);
			p->changeDir(Direction(rand() % 4));
			players[i + 1] = p;
			map[y][x] = p;
			map[y][x]->x = x;
			map[y][x]->y = y;
		}
		else {
			i--;
		}
	}
	
	//Matrix init
	for(int i = 0; i < SCREENY; i++) {
		for(int j = 0; j < SCREENX; j++) {
			int id = ((i * SCREENX) + j) * 4;
			array[id].position = sf::Vector2f(j * GRIDSIZE, i * GRIDSIZE);
			array[id + 1].position = sf::Vector2f(j * GRIDSIZE + GRIDSIZE, i * GRIDSIZE);
			array[id + 2].position = sf::Vector2f(j * GRIDSIZE + GRIDSIZE, i * GRIDSIZE + GRIDSIZE);
			array[id + 3].position = sf::Vector2f(j * GRIDSIZE, i * GRIDSIZE + GRIDSIZE);
		}
	}
	
	bool win = true;

	while(window.isOpen() && playerCount > 1) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		for(int i = 0; i < players.size(); i++) {
			if(players[i] && !players[i]->update()) {
				if(i == 0) {
					window.close();
					win = false;
				}
				else {
					playerCount--;
					players[i] = nullptr;
				}
			}
		}

		window.clear(sf::Color::White);
		int outputX = player->x - SCREENX / 2;
		outputX = std::min(WIDTH - SCREENX - 1, outputX);
		outputX = std::max(0, outputX);
		int outputY = player->y - SCREENY / 2;
		outputY = std::min(HEIGHT - SCREENY - 1, outputY);
		outputY = std::max(0, outputY);
		for(int i = outputY; i < outputY + SCREENY; i++) {
			for(int j = outputX; j < outputX + SCREENX; j++) {
				int id = ((i - outputY) * SCREENX) + j - outputX;
				id *= 4;
				if(map[i][j]) {
					sf::VertexArray& sprite = map[i][j]->render();
					for(int i = 0; i < 4; i++) {
						array[id + i].color = sprite[i].color;
					}
				}
				else {
					for(int i = 0; i < 4; i++) {
						array[id + i].color = sf::Color::Transparent;
					}
				}
			}
		}
		window.draw(array);
		window.display();
	}
	if(win) {
		std::cout << "You won!" << std::endl;
	}
	else {
		std::cout << "You lose :C" << std::endl;
	}
	return 0;
}
