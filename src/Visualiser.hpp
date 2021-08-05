#pragma once
#include <SFML/Graphics.hpp>
#include "GUI.hpp"

#include <vector>

class Visualiser {
private:
	sf::RenderWindow window;
	bool running;
	bool paused;
	bool runAlgorithm;
	float sleepTime;

	float barWidth;

	std::vector<int> nums;
	std::vector<int> backup;
	std::vector<sf::Color> colorMap;
	sf::Color mainColor, priColor, secColor, terColor;

	sf::Font font;
	sf::Clock algoClock;

	gui::Frame mainFrame;
	gui::Page mainPage;
	gui::Button shuffle, load;
	gui::Button bubble, select, insert, quickLL, quickLR, merge, merge2;
	gui::Slider widthSlider, speedSlider;
	gui::Textbox timeBox;

	void initWindow();
	void initGUI();

	void generateRandom();
	void loadAgain();

	void drawBars();

	enum Algo { NONE, BUBBLE, SELECTION, INSERTION, QUICK_LL, QUICK_LR, MERGE, MERGE_2 };

	void setAlgorithm(Algo algo);
	void bubble_sort();
	void selection_sort();
	void insertion_sort();
	void quick_sort_LL();
	void quick_sort_LR();
	void merge_sort();
	void merge_sort2();

public:
	Visualiser();
	~Visualiser();

	bool isRunning();

	void update();
	void pollEvents();
	void render();

	sf::Vector2f getMousePosition();
	sf::Vector2f getWinSize();
	sf::Vector2u getWinSizeInt();
};
