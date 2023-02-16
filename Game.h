#pragma once
#include <map>
#include "Ship.h"
#include "Bullets.h"
#include "Enemy.h"
#include<string>
#include<sstream>

class Game 
{
private:
	//window
	sf::RenderWindow* window;

	//resources
	//using map: which is structure thingy that maps two things
	std::map<std::string, sf::Texture*> textures;
	std::vector<Bullets*> bulletNum;

	//background stuff
	sf::Sprite worldBackground;
	sf::Texture worldBackgroundtexture;

	//HUD
	sf::Font font;
	sf::Text pointsText;

	sf::Text welcomeText;
	sf::Text gameOverText;

	sf::RectangleShape playerHpBar;
	sf::RectangleShape playerHpBarBG;

	//Tutorial/ Welcome
	bool welcome;

	//points system
	float points;

	//difficulty scaling
	float spawnRate;
	float spawnRateCounter;

	//main player
	Ship* ship;

	//Enemies
	float spawnTimer;
	float spawnTimerMax;
	std::vector<Enemy*> enemyNum;

	//private methods
	void initializeWindow();
	void initTextures();
	void initWelcomeScreen();
	void initPlayer();
	void initEnemies();
	void initHUD();
	void initSystem();

public:
	Game();
	virtual ~Game();

	//game functions
	void run();

	void updatePollEvents();
	void updateInputs();
	void updateHUD();
	void updateWorld();
	void updateCollisions();
	void updateBullets();
	void updateEnemies();
	void updateCombat();
	void update();

	void renderHUD();
	void renderBackground();
	void render();

};