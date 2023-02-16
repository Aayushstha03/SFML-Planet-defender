#include "Game.h"

void Game::initializeWindow()
{
	this->window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Planet Defence" ,sf::Style::Fullscreen);
	this->window->setFramerateLimit(61);
	this->window->setVerticalSyncEnabled(true);

	this->worldBackgroundtexture.loadFromFile("Textures/background.jpg");
	this->worldBackground.setTexture(this->worldBackgroundtexture);

}

void Game::initTextures()
{
	this->textures["BULLET"] = new sf::Texture();
	this->textures["BULLET"]->loadFromFile("Textures/Bullet.png");
}

void Game::initWelcomeScreen()
{
	this->welcome = true;
	this->welcomeText.setFont(this->font);
	this->welcomeText.setFillColor(sf::Color::White);
	this->welcomeText.setCharacterSize(50);
	this->welcomeText.setString("Welcome to Planet Defender!\nThe objective is to destroy all of the asteroids to protect the planet for 5 minutes\nUse WASD to move and SPACEBAR to shoot\nPress ENTER to start!");
	this->welcomeText.setPosition(this->window->getSize().x / 2.f - this->welcomeText.getGlobalBounds().width / 2.f,
		this->window->getSize().y / 2.f - this->welcomeText.getGlobalBounds().height / 2.f);
}

void Game::initHUD()
{
	//load fonts
	if (!this->font.loadFromFile("Fonts/8-bit Arcade In.ttf"))
	{
		std::cout << "Cant load fonts!";
	}

	//initializing points text
	this->pointsText.setFont(this->font);
	this->pointsText.setCharacterSize(50);
	this->pointsText.setFillColor(sf::Color::White);

	//Game over Screen! 
	this->gameOverText.setFont(this->font);
	this->gameOverText.setCharacterSize(200);
	this->gameOverText.setFillColor(sf::Color::White);
	this->gameOverText.setString("Game Over");
	this->gameOverText.setPosition
	(this->window->getSize().x / 2.f - this->gameOverText.getGlobalBounds().width / 2.f,
		this->window->getSize().y / 2.f - this->gameOverText.getGlobalBounds().height / 2.f);


	//initializing Player HP data
	this->playerHpBar.setSize(sf::Vector2f(400.f, 20.f));
	this->playerHpBar.setFillColor(sf::Color::Green);
	this->playerHpBar.setPosition(sf::Vector2f(50.f, this->window->getSize().y - 50.f));

	//copy attributes from HpBar!
	this->playerHpBarBG = this->playerHpBar;
	this->playerHpBarBG.setFillColor(sf::Color::Red);

	//initializing the "planet surface" lol
	this->shield.setSize(sf::Vector2f(this->window->getSize().x, 40));
	this->shield.setFillColor(sf::Color::Green);
	this->shield.setPosition(sf::Vector2f(0.f, this->window->getSize().y - 20.f));

}

void Game::initPlayer()
{
	this->ship = new Ship;
	this->ship->setPosition(this->window->getSize().x / 2 - this->ship->getBounds().width / 2.f, this->window->getSize().y);
}

void Game::initEnemies()
{
	this->spawnTimerMax = 40.f;
	this->spawnTimer = this->spawnTimerMax;
	this->spawnRate = 0.6f;
	this->spawnRateCounter = 0.f;
}

void Game::initSystem()
{
	this->points = 0;
}

Game::Game()
{
	//load window
	this->initializeWindow();
	//load textures
	this->initTextures();
	//initializing the welcome screen
	this->initWelcomeScreen();
	//load players ship
	this->initPlayer();
	//load enemy abstracts
	this->initEnemies();
	//load the HUD
	this->initHUD();
	//points and such
	this->initSystem();
}

Game::~Game()
{
	//then close the window and the ship 
	delete this->window;
	delete this->ship;

	//delete texture else memory leak
	for (auto& i : this->textures)
	{
		//delete the textures from the container
		delete i.second;
	}

	//delete bullets
	for (auto *i : this->bulletNum)
	{
		delete i;
	}

	//delete Enemies
	for (auto* i : this->enemyNum)
	{
		delete i;
	}

}

//Main Game Loop!!!!
void Game::run()
{
	while (this->window->isOpen())
	{
		//basic window controls
		this->updatePollEvents();

		//update the game only if the player is alive!
		if (this->ship->getHp() > 0)
			this->update();

		//render function
		this->render();
	}
}

void Game::updatePollEvents()
{
	sf::Event ev;

	while (this->window->pollEvent(ev))
	{
		if (ev.Event::type == sf::Event::Closed)
			this->window->close();
		if (ev.Event::KeyPressed && ev.Event::key.code == sf::Keyboard::Escape)
			this->window->close();
	}
}

void Game::updateInputs()
{

	//movement using WASD keys and arroy keys as well
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		this->ship->Move(0.f, -1.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		this->ship->Move(0.f, 1.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		this->ship->Move(-1.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		this->ship->Move(1.f, 0.f);

	//shooting
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)&&this->ship->canAttack())
	{
		this->bulletNum.push_back(new Bullets(this->textures["BULLET"], this->ship->getPos().x + this->ship->getBounds().width / 2.4f, this->ship->getPos().y - 20.f, 0.f, -1.f, 20.f));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		this->ship->setMoveSpeed(5.f);
	}
	else
		this->ship->setMoveSpeed(15.f);


}

void Game::updateBullets()
{
	unsigned counter = 0;
	for (auto* bullet : this->bulletNum)
	{
		bullet->update();

		if (bullet->getBounds().top + bullet->getBounds().height < 0.f)
		{
			//removing if its at top of the screen
			delete this->bulletNum.at(counter);
			this->bulletNum.erase(this->bulletNum.begin() + counter);
			//std::cout << this->bulletNum.size()<<std::endl;
			//check number of bullets
			counter--;
		}

		++counter;
	}
}

void Game::updateEnemies()
{
	//spawning the enemies
	this->spawnTimer += this->spawnRate;
	if (this->spawnTimer >= this->spawnTimerMax)
	{
		this->enemyNum.push_back(new Enemy(rand() % (this->window->getSize().x - 200), -200.f));
		this->spawnTimer = 0.f;
	}

	unsigned counter = 0;
	for (auto* enemy : this->enemyNum)
	{
		enemy->update();

		//removing if its at bottom of the screen
		if (enemy->getBounds().top > window->getSize().y)
		{
			//damage the planet/mothership whatever
			delete this->enemyNum.at(counter);
			this->enemyNum.erase(this->enemyNum.begin() + counter);
			--counter;
		}
		//removing the enemy if it touches the player
		else if (enemy->getBounds().intersects(this->ship->getBounds()))
		{
			this->ship->loseHp(this->enemyNum.at(counter)->getDamage());
			delete this->enemyNum.at(counter);
			this->enemyNum.erase(this->enemyNum.begin() + counter);
			--counter;
		}
		++counter;
	}
	
}

void Game::updateCombat()
{
	for (int i = 0; i < this->enemyNum.size(); i++)
	{
		bool enemy_deleted = false;
		for (int k = 0; k < this->bulletNum.size() && !enemy_deleted; k++)
		{
			if (this->bulletNum[k]->getBounds().intersects(this->enemyNum[i]->getBounds()))
			{
				this->enemyNum[i]->loseHP(1);

				if (this->enemyNum[i]->getHp() == 0)
				{
					//adding points for killed enemies
					this->points += this->enemyNum[i]->getPoints();
					this->spawnRateCounter += this->enemyNum[i]->getPoints();

					if (this->spawnRateCounter >= 100)
					{
						this->spawnRateCounter = 0;
						this->spawnRate += 0.05f;
						std::cout << spawnRate;
					}

					//deleting enemy
					delete this->enemyNum[i];
					this->enemyNum.erase(this->enemyNum.begin() + i);

					enemy_deleted = true;
					//after deleting a enemy continue loop for another enemy!
				}			

				//deleting bullet
				delete this->bulletNum[k];
				this->bulletNum.erase(this->bulletNum.begin() + k);
			}
		}
	}
}

void Game::updateHUD()
{
	//creating string to print on the window
	std::stringstream ss;
	ss <<"Points " << this->points;	
	this->pointsText.setString(ss.str());

	//updating player HP bar and such
	float hpPercent = static_cast<float>(this->ship->getHp()) / this->ship->getHpMax();
	//original size * current hpPercent to generate the length!
	this->playerHpBar.setSize(sf::Vector2f(400.f * hpPercent, this->playerHpBar.getSize().y));
	 
}

void Game::updateWorld()
{
}

void Game::updateCollisions()
{
	//checking collion in left and right
	if (this->ship->getBounds().left < 0.f)
	{
		this->ship->setPosition(0.f, this->ship->getBounds().top);
	}
	else if (this->ship->getBounds().left + this->ship->getBounds().width  >= this->window->getSize().x)
	{
		this->ship->setPosition(this->window->getSize().x - this->ship->getBounds().width, this->ship->getBounds().top);
	}

	//checking collision in top and bottom
	if (this->ship->getBounds().top < 0.f)
	{
		this->ship->setPosition(this->ship->getBounds().left, 0.f);
	}

	if (this->ship->getBounds().top + this->ship->getBounds().height >= this->window->getSize().y)
	{
		this->ship->setPosition(this->ship->getBounds().left, this->window->getSize().y - this->ship->getBounds().height);
	}

}

void Game::update()
{
	this->updateInputs();

	this->ship->update();

	this->updateCollisions();

	this->updateBullets();

	this->updateEnemies();

	this->updateCombat();

	this->updateHUD();

	this->updateWorld();

}

void Game::renderHUD()
{
	this->window->draw(this->pointsText);
	this->window->draw(this->playerHpBarBG);
	this->window->draw(this->playerHpBar);
	this->window->draw(this->shield);
}

void Game::renderBackground()
{
	this->window->draw(this->worldBackground);
}

void Game::renderWelcomeScreen()
{
	this->window->draw(this->welcomeText);
}

void Game::render()
{
	//clear the old frame
	this->window->clear();

	this->renderBackground();

//	if((!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) && this->welcome == true)
//	{
//		this->renderWelcomeScreen();
//	}
	
	//add objects
	this->ship->render(*this->window);
	
	//for bullet
	for (auto* bullet : this->bulletNum)
	{
		bullet->render(this->window);
	}
	
	//for each enemy
	for (auto* enemy : this->enemyNum)
	{
		enemy->render(this->window);
	}

	//HUD render
	this->renderHUD();

	//Rendering Game Over Screen
	if (this->ship->getHp() == 0)
	{
		this->window->draw(this->gameOverText);
		this->window->draw(this->welcomeText);
	}

	//push the new frame
	this->window->display();
}
