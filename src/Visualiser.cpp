#include "Visualiser.hpp"
#include <random>
#include <chrono>
#include <algorithm>
#include <stack>
#include <utility>

std::string secondsAsTime(float t) {
	int ms = int(t * 100) % 100;
	int s = t;
	int m = s / 60;
	s = s % 60;

	return std::to_string(m) + ":" + std::to_string(s) + ":" + std::to_string(ms);
}

void Visualiser::initWindow()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	window.create(sf::VideoMode(1450, 500), "Visualiser", sf::Style::Default, settings);
	window.setFramerateLimit(120);
}

void Visualiser::initGUI()
{
	font.loadFromFile("data/consola.ttf");

	mainFrame.setWindow(window);
	mainPage.setFillColor(sf::Color(255, 255, 255, 26));
	mainPage.setSize({ 400, 1000 });
	mainPage.setPosition(1050, 0);
	mainPage.setActiveRegion({ 0,0, 400, 600 });
	mainPage.setScroll(gui::Scroll::RIGHT);
	mainFrame.addEntity(mainPage);

	sf::Vector2f size(180, 50);
	sf::Vector2i pos(0, 0);

	shuffle.setSize({ 180, 50 });
	shuffle.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	shuffle.setFillColor(sf::Color::Transparent);
	shuffle.setOutlineColor(sf::Color::Red);
	shuffle.setOutlineThickness(3);
	shuffle.setHighlightFillColor(sf::Color(255, 0, 0, 126));
	shuffle.setHighlightOutlineColor(sf::Color(255, 165, 0));
	shuffle.setHighlightOutlineThickness(5);
	shuffle.setCharacterSize(28);
	shuffle.setTextFillColor(sf::Color::White);
	shuffle.setFont(font);
	shuffle.setString("Shuffle");
	mainPage.addEntity(shuffle);
	shuffle.setAction([this] {generateRandom(); });

	pos = { 1, 0 };
	load.copy(shuffle);
	load.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	load.setString("Load");
	mainPage.addEntity(load);
	load.setAction([this] {loadAgain(); });

	pos = { 0, 2 };
	widthSlider.setSize(size);
	widthSlider.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	widthSlider.setFillColor(sf::Color::Transparent);
	widthSlider.setOutlineColor(sf::Color::Blue);
	widthSlider.setOutlineThickness(3);
	widthSlider.setTextFillColor(sf::Color::White);
	widthSlider.setFont(font);
	widthSlider.setCharacterSize(28);
	widthSlider.setString("Width");
	widthSlider.setAction([this]() {
		widthSlider.moveSliderBar(mainPage.getMousePosition());
		barWidth = std::max(2.f, widthSlider.getMappedOffset() * 50);
		});
	mainPage.addEntity(widthSlider);

	pos = { 1, 2 };
	speedSlider.copy(widthSlider);
	speedSlider.setString("Speed");
	speedSlider.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	speedSlider.setAction([this]() {
		speedSlider.moveSliderBar(mainPage.getMousePosition());
		sleepTime = 500 - std::max(0.f, speedSlider.getMappedOffset() * 500);
		});
	speedSlider.setHigh();
	mainPage.addEntity(speedSlider);

	timeBox.setSize({ 80, 40 });
	timeBox.setPosition(10 + 0.75 * 195, 75);
	timeBox.setFont(font);
	timeBox.setFillColor(sf::Color::Transparent);
	timeBox.setOutlineColor(sf::Color::Transparent);
	timeBox.setCharacterSize(28);
	timeBox.setTextFillColor(sf::Color::White);
	timeBox.setString(secondsAsTime(0));
	mainPage.addEntity(timeBox);

	pos = { 0, 3 };
	bubble.copy(shuffle);
	bubble.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	bubble.setOutlineColor(sf::Color(160, 32, 240));
	bubble.setHighlightFillColor(sf::Color(160, 32, 240, 126));
	bubble.setHighlightOutlineColor(sf::Color(124, 163, 251));
	bubble.setString("Bubble");
	mainPage.addEntity(bubble);
	bubble.setAction([this] {setAlgorithm(BUBBLE); });

	pos = { 1,3 };
	select.copy(bubble);
	select.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	select.setString("Select");
	mainPage.addEntity(select);
	select.setAction([this] {setAlgorithm(SELECTION); });

	pos = { 0,4 };
	insert.copy(bubble);
	insert.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	insert.setString("Insert");
	mainPage.addEntity(insert);
	insert.setAction([this] {setAlgorithm(INSERTION); });

	pos = { 1,4 };
	quickLL.copy(bubble);
	quickLL.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	quickLL.setString("Quick LL");
	mainPage.addEntity(quickLL);
	quickLL.setAction([this] {setAlgorithm(QUICK_LL); });

	pos = { 0,5 };
	quickLR.copy(bubble);
	quickLR.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	quickLR.setString("Quick LR");
	mainPage.addEntity(quickLR);
	quickLR.setAction([this] {setAlgorithm(QUICK_LR); });

	pos = { 1,5 };
	merge.copy(bubble);
	merge.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	merge.setString("Merge");
	mainPage.addEntity(merge);
	merge.setAction([this] {setAlgorithm(MERGE); });

	pos = { 0,6 };
	merge2.copy(bubble);
	merge2.setPosition(10 + pos.x * 195, 10 + pos.y * 65);
	merge2.setString("Merge 2");
	mainPage.addEntity(merge2);
	merge2.setAction([this] {setAlgorithm(MERGE_2); });
}

Visualiser::Visualiser()
{
	this->initWindow();
	this->initGUI();

	running = true;
	paused = false;
	runAlgorithm = false;

	barWidth = 2.f;
	sleepTime = 0.f;

	mainColor = sf::Color::Red;
	priColor = sf::Color::White; secColor = sf::Color::Green; terColor = sf::Color::Blue;

	generateRandom();
}

Visualiser::~Visualiser()
{

}

void Visualiser::generateRandom()
{
	nums.clear();
	int bar_count = (getWinSizeInt().y * 2) / barWidth;
	nums.reserve(bar_count);
	for (int i = 0; i < bar_count; i++)
		nums.push_back(i + 1);

	std::shuffle(nums.begin(), nums.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

	backup = nums;
	
	colorMap.clear();
	colorMap.reserve(nums.size());
	for (int i = 0; i < nums.size(); i++)
		colorMap.push_back(mainColor);

	setAlgorithm(NONE);
}

void Visualiser::loadAgain()
{
	nums = backup;

	colorMap.clear();
	colorMap.reserve(nums.size());
	for (int i = 0; i < nums.size(); i++)
		colorMap.push_back(mainColor);

	setAlgorithm(NONE);
}

bool Visualiser::isRunning()
{
	return this->running;
}

void Visualiser::update()
{
	mainFrame.update();
	this->pollEvents();
	
	if (runAlgorithm)timeBox.setString(secondsAsTime(algoClock.getElapsedTime().asSeconds()));

	if (!this->paused) {
		
	}
}

void Visualiser::pollEvents()
{
	sf::Event event;
	while (this->window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
			this->running = false; this->runAlgorithm = false;
		}

		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)this->paused = !this->paused;

		mainFrame.pollEvents(event);
	}
}

void Visualiser::drawBars()
{
	sf::RectangleShape bar;
	for (int i = 0; i < nums.size(); i++) {
		bar.setFillColor(colorMap[i]);
		bar.setSize({ barWidth - 1, ((float)nums[i] * barWidth) / 2 });
		bar.setPosition(barWidth * i, getWinSizeInt().y - bar.getSize().y);
		this->window.draw(bar);
	}
}

void Visualiser::render()
{
	this->window.clear();

	this->drawBars();
	mainFrame.draw();

	this->window.display();
}

sf::Vector2f Visualiser::getMousePosition()
{
	return window.mapPixelToCoords(sf::Mouse::getPosition(this->window));
}

sf::Vector2f Visualiser::getWinSize()
{
	return sf::Vector2f(this->window.getSize());
}

sf::Vector2u Visualiser::getWinSizeInt()
{
	return this->window.getSize();
}

void Visualiser::setAlgorithm(Algo algo)
{
	if (algo != NONE) {
		runAlgorithm = true;
		widthSlider.setInactive();
	}

	switch (algo)
	{
	case BUBBLE: bubble_sort(); break;
	case SELECTION: selection_sort(); break;
	case INSERTION: insertion_sort(); break;
	case QUICK_LL: quick_sort_LL(); break;
	case QUICK_LR: quick_sort_LR(); break;
	case MERGE: merge_sort(); break;
	case MERGE_2: merge_sort2(); break;
	case NONE: runAlgorithm = false; widthSlider.setActive(); break;
	}
}

void Visualiser::bubble_sort()
{
	algoClock.restart();
	for (int i = 0; i < nums.size() - 1 && runAlgorithm; i++) {
		for (int j = 1; j < nums.size() && runAlgorithm; j++) {
			colorMap[nums.size() - i - 1] = priColor;
			colorMap[j] = secColor;

			if (nums[j - 1] > nums[j]) {
				int t = nums[j - 1];
				nums[j - 1] = nums[j];
				nums[j] = t;
			}

			this->update();
			this->render();
			if (runAlgorithm) {
				colorMap[j] = mainColor;
				colorMap[nums.size() - i - 1] = mainColor;
			}

			sf::sleep(sf::Time(sf::milliseconds(sleepTime)));
		}
	}

	setAlgorithm(NONE);
}

void Visualiser::selection_sort()
{
	algoClock.restart();
	for (int i = 0; i < nums.size() - 1 && runAlgorithm; i++) {
		int min = i;
		for (int j = i + 1; j < nums.size() && runAlgorithm; j++) {
			colorMap[i] = priColor;
			colorMap[j] = secColor;
			colorMap[min] = terColor;

			this->update();
			this->render();

			if (runAlgorithm) {
				colorMap[i] = mainColor;
				colorMap[j] = mainColor;
				colorMap[min] = mainColor;

				if (nums[j] < nums[min])min = j;
			}

			sf::sleep(sf::Time(sf::milliseconds(sleepTime)));
		}
		if (runAlgorithm) {
			if (min != i) {
				int t = nums[i];
				nums[i] = nums[min];
				nums[min] = t;
			}
		}
	}

	setAlgorithm(NONE);
}

void Visualiser::insertion_sort()
{
	algoClock.restart();
	for (int i = 1; i < nums.size() && runAlgorithm; i++) {
		int x = nums[i];
		int j = i;
		while (j > 0 &&  x < nums[j-1] && runAlgorithm) {
			colorMap[j] = secColor;
			colorMap[i] = priColor;

			nums[j] = nums[j-1];

			this->update();
			this->render();

			if (runAlgorithm) {
				colorMap[i] = mainColor;
				colorMap[j] = mainColor;

				j--;
			}

			sf::sleep(sf::Time(sf::milliseconds(sleepTime)));
		}
		if (runAlgorithm) {
			nums[j] = x;
		}
	}

	setAlgorithm(NONE);
}

void Visualiser::quick_sort_LL()
{
	algoClock.restart();

	std::stack<int> stack;
	stack.push(0);
	stack.push(nums.size() - 1);

	while (!stack.empty() && runAlgorithm) {
		int right = stack.top(); stack.pop();
		int left = stack.top(); stack.pop();

		if (left < right) {
			int index = left;
			int pivot = nums[right];
			for (int j = left; j < right && runAlgorithm; j++) {
				colorMap[index] = secColor;
				colorMap[j] = terColor;
				colorMap[left] = priColor;
				colorMap[right] = priColor;

				this->update();
				this->render();

				if (runAlgorithm) {
					colorMap[index] = mainColor;
					colorMap[j] = mainColor;
					colorMap[left] = mainColor;
					colorMap[right] = mainColor;

					if (nums[j] < pivot) {
						int t = nums[index];
						nums[index] = nums[j];
						nums[j] = t;

						index++;
					}
				}

				sf::sleep(sf::Time(sf::milliseconds(sleepTime)));
			}
			if (runAlgorithm) {
				int t = nums[index];
				nums[index] = nums[right];
				nums[right] = t;

				stack.push(left); stack.push(index - 1);
				stack.push(index + 1); stack.push(right);
			}
		}
	}

	setAlgorithm(NONE);
}

void Visualiser::quick_sort_LR()
{
	algoClock.restart();

	std::stack<int> stack;
	stack.push(0);
	stack.push(nums.size() - 1);

	while (!stack.empty() && runAlgorithm) {
		int right = stack.top(); stack.pop();
		int left = stack.top(); stack.pop();

		if (left < right) {
			int l = left + 1, r = right;
			int pivot = nums[left];

			while (l < r && runAlgorithm) {
				while (nums[l] < pivot || nums[r] > pivot && runAlgorithm) {
					colorMap[l] = secColor;
					colorMap[r] = terColor;
					colorMap[left] = priColor;
					colorMap[right] = priColor;

					this->update();
					this->render();

					if (runAlgorithm) {
						colorMap[l] = mainColor;
						colorMap[r] = mainColor;
						colorMap[left] = mainColor;
						colorMap[right] = mainColor;

						if (nums[l] < pivot)l++;
						if (nums[r] > pivot)r--;
					}

					sf::sleep(sf::Time(sf::milliseconds(sleepTime)));
				}
				if (runAlgorithm && l < r) {
					int t = nums[l];
					nums[l] = nums[r];
					nums[r] = t;
				}
			}

			if (runAlgorithm) {
				if (nums[r] < nums[left]) {
					int t = nums[r];
					nums[r] = nums[left];
					nums[left] = t;
				}

				stack.push(left); stack.push(r - 1);
				stack.push(r + 1); stack.push(right);
			}
		}
	}

	setAlgorithm(NONE);
}

void Visualiser::merge_sort()
{
	algoClock.restart();

	std::stack<std::pair<std::pair<int, int>, std::pair<int, int>>> stack;
	std::vector<std::pair<int, int>> s;
	s.push_back(std::make_pair(0, nums.size()));
	int top = 0;
	while (top < s.size()) {
		if (s[top].first != (s[top].first + s[top].second) / 2) {
			s.insert(s.begin() + top + 1, std::make_pair(s[top].first, (s[top].first + s[top].second) / 2));
			s.insert(s.begin() + top + 1, std::make_pair((s[top].first + s[top].second) / 2, s[top].second));
			stack.push(std::make_pair(
				std::make_pair(s[top].first, (s[top].first + s[top].second) / 2),
				std::make_pair((s[top].first + s[top].second) / 2, s[top].second)
			));
		}
		top++;
	}

	while (!stack.empty())
	{
		auto info = stack.top();
		stack.pop();
		std::vector<int> one, two;
		one.insert(one.begin(), nums.begin() + info.first.first, nums.begin() + info.first.second);
		two.insert(two.begin(), nums.begin() + info.second.first, nums.begin() + info.second.second);
		int index = info.first.first;
		int first = 0, second = 0;
		while (first < one.size() || second < two.size()) {
			int a = std::min(info.first.first + first, info.first.second - 1),
				b = std::min(info.second.first + second, info.second.second - 1),
				c = index;

			colorMap[a] = secColor;
			colorMap[b] = terColor;
			colorMap[c] = priColor;

			if (first < one.size() && second < two.size()) {
				if (one[first] < two[second]) {
					nums[index] = one[first];
					first++;
				}
				else {
					nums[index] = two[second];
					second++;
				}
				index++;
			}
			else if (first < one.size()) {
				nums[index] = one[first];
				first++; index++;
			}
			else if (second < two.size()) {
				nums[index] = two[second];
				second++; index++;
			}

			this->update();
			this->render();

			if (runAlgorithm) {
				colorMap[a] = mainColor;
				colorMap[b] = mainColor;
				colorMap[c] = mainColor;
			}

		}
	}

	setAlgorithm(NONE);
}

void Visualiser::merge_sort2()
{
	algoClock.restart();

	std::stack<std::pair<std::pair<int, int>, std::pair<int, int>>> stack;
	std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> v;
	int top = -1;
	v.push_back(std::make_pair(std::make_pair(0, nums.size() / 2 + 1), std::make_pair(nums.size() / 2 + 1, nums.size())));
	while (top + 1 < v.size()) {
		auto k = v[++top];
		if (k.first.first != (k.first.second + k.first.first) / 2)
			v.push_back(std::make_pair(std::make_pair(k.first.first, (k.first.second + k.first.first) / 2), std::make_pair((k.first.second + k.first.first) / 2, k.first.second)));
		if (k.second.first != (k.second.second + k.second.first) / 2)
			v.push_back(std::make_pair(std::make_pair(k.second.first, (k.second.second + k.second.first) / 2), std::make_pair((k.second.second + k.second.first) / 2, k.second.second)));
	}
	for (auto k : v)stack.push(k);

	while (!stack.empty())
	{
		auto info = stack.top();
		stack.pop();
		std::vector<int> one, two;
		one.insert(one.begin(), nums.begin() + info.first.first, nums.begin() + info.first.second);
		two.insert(two.begin(), nums.begin() + info.second.first, nums.begin() + info.second.second);
		int index = info.first.first;
		int first = 0, second = 0;
		while (first < one.size() || second < two.size()) {
			int a = std::min(info.first.first + first, info.first.second - 1),
				b = std::min(info.second.first + second, info.second.second - 1),
				c = index;

			colorMap[a] = secColor;
			colorMap[b] = terColor;
			colorMap[c] = priColor;

			if (first < one.size() && second < two.size()) {
				if (one[first] < two[second]) {
					nums[index] = one[first];
					first++;
				}
				else {
					nums[index] = two[second];
					second++;
				}
				index++;
			}
			else if (first < one.size()) {
				nums[index] = one[first];
				first++; index++;
			}
			else if (second < two.size()) {
				nums[index] = two[second];
				second++; index++;
			}

			this->update();
			this->render();

			if (runAlgorithm) {
				colorMap[a] = mainColor;
				colorMap[b] = mainColor;
				colorMap[c] = mainColor;
			}

		}
	}

	setAlgorithm(NONE);
}

