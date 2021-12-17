#include "Game.h"

//========PRIVATE FUNCTIONS========//
void Game::InitVariables()
{
    /*
        Use this method to initialise random variables within the game class
    */

    //allocate nullptrs to pointers
    this->window = nullptr;
    this->musicPointer = nullptr;

    //========GAME LOGIC========//
    
    //player variables
    this->GameMenu = MainScene; //options are: MainScreen, GameScreen, EndScreen & PauseScreen ---> EDIT: Swap "Screen" with "Scene"
    this->score = 0;
    this->maxHealth = 5;
    this->currentHealth = this->maxHealth;
    //enemy variables
    this->enemySpawnTimerMax = 1.0f; //in seconds
    this->maxEnemies = 20;
    this->baseEnemySpeed = 70.f;
    //text input
    this->currentInput.push_back('\0');
    this->maxHighscores = 10;
    //clock variables
    this->DeltaTime = 0.f;
    this->enemySpawnTimer = 0.f;
}

void Game::InitWindow()
{
    /*
        Used for initialising variables to do with the game window
    */

    //init videoMode
    this->videoMode.height = 600;
    this->videoMode.width = 800;

    //init window settings
    this->windowSettings.antialiasingLevel = 4;

    //init RenderWindow
    this->window = new sf::RenderWindow(sf::VideoMode(this->videoMode), "Benjamin's Bit Blast", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(this->windowSettings));

    //set framerate limit
    this->window->setFramerateLimit(90);
}

void Game::InitEnemies()
{
    /*
        Here is where you need to initialise an enemy.

        - declare a new instance of an EnemyType object in the header.
        - call .InitEnemy with this->event and this->videoMode as it's parameters.
        - call .CreateEnemy and initialise 7 different aspects of the enemy:
            * Size, X and Y dimensions.
            * Colour.
            * Health - How many times the player must click before the enemy dies.
            * Speed - A percentage value based on the game's base speed, 1.f = 100% of base speed while 0.5f = 50% of base speed.
            * Attack - How many health points are depleted if the enemy reaches the bottom of the screen.
            * Points - How many points the player recieves if they kill the enemy.
            * & it's Spawn Weight - Defines how likely this enemy will spawn in comparison to other enemy types, for more information go to Game::InitWeights.
        - & add the enemy to the weight calculator.

        If all these things are done correctly the enemy will show up on screen and have comeplete functionality. No need to add anything to the draw function.
    */
   
    //init regular enemy
    this->regularEnemy.InitEnemy(this->event, this->videoMode);
    this->regularEnemy.CreateEnemy(40.f, 60.f, sf::Color::Blue, 2, 1.f, 1, 10, 5);
    this->weightCalculator.push_back(this->regularEnemy);

    //init fast enemy
    this->fastEnemy.InitEnemy(this->event, this->videoMode);
    this->fastEnemy.CreateEnemy(30.f, 40.f, sf::Color::Yellow, 1, 4.f, 1, 15, 4);
    this->weightCalculator.push_back(this->fastEnemy);

    //init tank enemy
    this->tankEnemy.InitEnemy(this->event, this->videoMode);
    this->tankEnemy.CreateEnemy(70.f, 70.f, sf::Color::Green, 5, 0.25f, 2, 30, 2);
    this->weightCalculator.push_back(this->tankEnemy);

    //demo enemies when showing people

    //init boss enemy
    //this->bossEnemy.InitEnemy(this->event, this->videoMode);
    //this->bossEnemy.CreateEnemy(100.f, 40.f, sf::Color::Red, 15, 0.15f, 10, 1000, 1);
    //this->weightCalculator.push_back(this->bossEnemy);

    //init finn enemy
    //this->finnEnemy.InitEnemy(this->event, this->videoMode);
    //this->finnEnemy.CreateEnemy(400.f, 60.f, sf::Color(48, 128, 20, 255), 1, 0.1f, 1000, 1, 20);
    //this->weightCalculator.push_back(this->finnEnemy);

    //init ken enemy
    //this->kenEnemy.InitEnemy(this->event, this->videoMode);
    //this->kenEnemy.CreateEnemy(50.f, 200.f, sf::Color(0, 255, 255, 255), 20, 0.5f / 0.4f * 1.f, 2, 100, 1);
    //this->weightCalculator.push_back(this->kenEnemy);

    //calling init enemy weights so that random spawn functionality works
    this->InitEnemyWeights();
}

void Game::InitEnemyWeights()
{
    /*
        This method allows the randomisation of enemy spawning, and calculates the percentage chance of a specific enemy type spawning.
        This method's major flaw is that if many enemy types are added, then the percentage chance of all enemies go down but overall
        but I believe this to be the best method of calculating randomness, this method will change if a better method is found.

        The chances are calculated by determining how much weight and individual enemy has compared to all other enemies.
        If there are three enemies, enemy A, B & C, and enemy A and B have a weight of 1 but enemy C has a weight of 2.
        Enemy A and B would have a 25% chance of spawning while enemy C would have a 50% chance, as enemy C has 50% of the total weight. 2 / 4 * 100 = 50.

        With that in mind, the spawn chances are calculated like so:
        - The total amount of weight across all enemies is tallied so that we know what to base the percentages on.
        - The percentage start starts at either 0 OR 1% more than the last enemy's end percentage chance. Depending if it is the first enemy or not.
        - The percentage end is calculated by calculating that enemy's spawn percent, and adding it to it's percentage start.
        - A random number is rolled between 0 and 100 and when the number lands in the enemy's percentage range than that enemy is spawned.
    */
   
    this->totalWeights = 0;

    //tally total weights of every enemy
    for(int i = 0; i < this->weightCalculator.size(); ++i)
    {
        this->totalWeights += weightCalculator[i].ReturnEnemyInfo().weight;
    }

    //dummy enemy for returning value
    Game::EnemyWeightRange dummyWeightRange;

    for (int i = 0; i < weightCalculator.size(); ++i)
    {
        //set the beginning of the range to be 0, or one more than the max range of the last enemy calculated
        if(i - 1 >= 0)
        {
            dummyWeightRange.rangeBegin = weightCalculator[i - 1].ReturnSpawnChance().rangeEnd + 1;
        } else {
            dummyWeightRange.rangeBegin = 0;
        }

        //set the max range to be a percentage of their individual weight compared to the total weight of every enemy
        dummyWeightRange.rangeEnd = ((weightCalculator[i].ReturnEnemyInfo().weight / totalWeights) * 100) + dummyWeightRange.rangeBegin;

        //set the number range of the spawn chance of the enemy
        this->weightCalculator[i].SetSpawnChance(dummyWeightRange);
    }
}

void Game::InitUI()
{
    /*
        This method is for initialising UI elements such as, sf::Text, sf::RectangleShape, etc.

        There is a custom method that outputs an sf::Text object so that is used when initialising Text objects.
        For everything else, their values must be initialised manually.

        To initialise a Text object, these steps are to be taken:
        - Declare a new sf::Text object in the header.
        - Let the newly declared object be equal to "this->CreateText()", eg: this->myText = this->CreateText();
        - Then we need to pass through parameters into the CreateText method.
            * Font - using enum Game::WhichFont we decide which font is used, Arcade or ArcadeLarge.
            * Size - usually a heading is sized at 32 while a button is usually 28 units.
            * Colour - declare sf::Color and decide which colour.
            * String - use a string to define what the text will say.
            * & it's Origin - whether or not we center the text's origin, true or false.
        - After declaring the text the position must be set, fine tuning can be used by multiplying the window's size by a fraction.
    */
       
    //load fonts
    this->arcadeFont.loadFromFile("assets/fonts/pixel/pixelsix10.ttf");
    this->arcadeFontLarge.loadFromFile("assets/fonts/ipixelu/I-pixel-u.ttf");

    //copy this for setPosition -> .setPosition(this->window->getSize().x, this->window->getSize().y);

    //========INIT TEXT========//

    //===IN GAME===//
    {
    //score text
    this->InGameUi.scoreText = this->CreateText(Arcade, 24, sf::Color(255, 192, 203, 255), "NONE", true);
    this->InGameUi.scoreText.setPosition(sf::Vector2f(this->window->getSize().x / 10, this->window->getSize().y * 48 / 52)); //set position - bottom left
    this->RefreshUI(ScoreElement); //refresh text

    //health text
    this->InGameUi.healthText = this->CreateText(Arcade, 24, sf::Color(255, 192, 203, 255), "NONE", true);
    this->InGameUi.healthText.setPosition(sf::Vector2f(this->window->getSize().x / 5 * 4, this->window->getSize().y * 48 / 52)); //set position - bottom right
    this->RefreshUI(HealthElement); //refresh text

    //kill line
    this->InGameUi.killLine.setSize(sf::Vector2f(this->window->getSize().x, 5.f));
    this->InGameUi.killLine.setFillColor(sf::Color(255, 192, 203, 255));
    this->InGameUi.killLine.setOrigin(0, this->InGameUi.killLine.getSize().y / 2);
    this->InGameUi.killLine.setPosition(sf::Vector2f(0, this->window->getSize().y * 45 / 52));

    //ui background
    this->InGameUi.uiBackground.setSize(sf::Vector2f(this->window->getSize().x, this->window->getSize().y * 8 / 52));
    this->InGameUi.uiBackground.setFillColor(sf::Color::Black);
    this->InGameUi.uiBackground.setPosition(sf::Vector2f(0, this->window->getSize().y * 45 / 52));
    }

    //===Main Menu===//
    {
    //title text main
    this->MainMenuUi.titleText = this->CreateText(ArcadeLarge, 48, sf::Color::Cyan, "BENJAMIN'S BIT BLAST", true);
    this->MainMenuUi.titleText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 4 / 10); //set position - mid centre

    //options button text
    this->MainMenuUi.optionsButtonText = this->CreateText(Arcade, 35, sf::Color::Yellow, "OPTIONS MENU", true);
    this->MainMenuUi.optionsButtonText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 25 / 40);

    //exit button text
    this->MainMenuUi.exitButtonText = this->CreateText(Arcade, 36, sf::Color::Red, "EXIT GAME", true);
    this->MainMenuUi.exitButtonText.setPosition(this->window->getSize().x / 3, this->window->getSize().y * 11 / 20); //set position - mid centre

    //play button text
    this->MainMenuUi.playButtonText = this->CreateText(Arcade, 36, sf::Color::Green, "PLAY GAME", true);
    this->MainMenuUi.playButtonText.setPosition(this->window->getSize().x * 2 / 3, this->window->getSize().y * 11 / 20); //set position - mid centre
    }

    //===End Menu===//
    {
    //title text
    this->EndMenuUi.titleText = this->CreateText(ArcadeLarge, 48, sf::Color::Cyan, "GAME OVER", true);
    this->EndMenuUi.titleText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 1 / 10);
    
    //subtitle text
    this->EndMenuUi.subTitleText = this->CreateText(ArcadeLarge, 38, sf::Color::White, "HIGHSCORES", true);
    this->EndMenuUi.subTitleText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 2 / 10);

    //exit button text
    this->EndMenuUi.exitButtonText = this->CreateText(Arcade, 36, sf::Color::Red, "EXIT GAME", true);
    this->EndMenuUi.exitButtonText.setPosition(this->window->getSize().x / 3, this->window->getSize().y * 17 / 20); //set position - mid centre

    //play button text
    this->EndMenuUi.playButtonText = this->CreateText(Arcade, 36, sf::Color::Green, "PLAY AGAIN", true);
    this->EndMenuUi.playButtonText.setPosition(this->window->getSize().x * 2 / 3, this->window->getSize().y * 17 / 20);
    }

    //===PAUSE MENU===//
    {
    //title text
    this->PauseMenuUi.titleText = this->CreateText(ArcadeLarge, 48, sf::Color::Cyan, "PAUSED", true);
    this->PauseMenuUi.titleText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 3 / 10);

    //resume text
    this->PauseMenuUi.resumeButtonText = this->CreateText(Arcade, 36, sf::Color::Green, "Resume", true);
    this->PauseMenuUi.resumeButtonText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 5 / 10);

    //quit to main menu
    this->PauseMenuUi.mainmenuButtonText = this->CreateText(Arcade, 36, sf::Color::Magenta, "Main Menu", true);
    this->PauseMenuUi.mainmenuButtonText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 6 / 10);

    //quit game
    this->PauseMenuUi.quitButtonText = this->CreateText(Arcade, 36, sf::Color::Red, "Quit Game", true);
    this->PauseMenuUi.quitButtonText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 7 / 10);
    }

    //===OPTIONS MENU===//
    {
    //load saved options data
    this->optionsSaveData = this->FileManager.ReadFile(OptionsFile);

    //title text
    this->OptionsMenuUi.titleText = this->CreateText(ArcadeLarge, 48, sf::Color::Yellow, "OPTIONS MENU", true);
    this->OptionsMenuUi.titleText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 1 / 10);

    //return to main menu button
    this->OptionsMenuUi.mainmenuButtonText = this->CreateText(Arcade, 36, sf::Color::Cyan, "Return to Main Menu", true);
    this->OptionsMenuUi.mainmenuButtonText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 9 / 10);

    //main volume title / changer
    this->OptionsMenuUi.mainSoundText = this->CreateText(Arcade, 32, sf::Color(210, 210, 210, 255), "Main", true);
    this->OptionsMenuUi.mainSoundText.setPosition(this->window->getSize().x * 2 / 11, this->window->getSize().y * 3 / 10);
    this->OptionsMenuUi.mainSoundChanger.InitRangeChanger(sf::Vector2f(this->window->getSize().x * 4 / 11, this->window->getSize().y * 3 / 10), this->window, this->optionsSaveData[0]);

    //music volume title / changer
    this->OptionsMenuUi.musicSoundText = this->CreateText(Arcade, 32, sf::Color(210, 210, 210, 255), "Music", true);
    this->OptionsMenuUi.musicSoundText.setPosition(this->window->getSize().x * 7 / 11, this->window->getSize().y * 3 / 10);
    this->OptionsMenuUi.musicSoundChanger.InitRangeChanger(sf::Vector2f(this->window->getSize().x * 9 / 11, this->window->getSize().y * 3 / 10), this->window, this->optionsSaveData[1]);

    //menu volume title / changer
    this->OptionsMenuUi.menuSoundText = this->CreateText(Arcade, 32, sf::Color(210, 210, 210, 255), "Menu", true);
    this->OptionsMenuUi.menuSoundText.setPosition(this->window->getSize().x * 2 / 11, this->window->getSize().y * 4 / 10);
    this->OptionsMenuUi.menuSoundChanger.InitRangeChanger(sf::Vector2f(this->window->getSize().x * 4 / 11, this->window->getSize().y * 4 / 10), this->window, this->optionsSaveData[2]);

    //sfx volume title / changer
    this->OptionsMenuUi.sfxSoundText = this->CreateText(Arcade, 32, sf::Color(210, 210, 210, 255), "SFX", true);
    this->OptionsMenuUi.sfxSoundText.setPosition(this->window->getSize().x * 7 / 11, this->window->getSize().y * 4 / 10);
    this->OptionsMenuUi.sfxSoundChanger.InitRangeChanger(sf::Vector2f(this->window->getSize().x * 9 / 11, this->window->getSize().y * 4 / 10), this->window, this->optionsSaveData[3]);
    }

    //===HIGHSCORE INPUT MENU===//
    {
    //title text
    this->HighscoreInputMenuUi.titleText = this->CreateText(ArcadeLarge, 48, sf::Color::Green, "NEW HIGHSCORE!", true);
    this->HighscoreInputMenuUi.titleText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 1 / 8);

    //input header text
    this->HighscoreInputMenuUi.inputHeaderText = this->CreateText(Arcade, 32, sf::Color::White, "Input Your Name:", true);
    //this->HighscoreInputMenuUi.inputHeaderText.setOrigin(this->HighscoreInputMenuUi.inputHeaderText.getGlobalBounds().width, this->HighscoreInputMenuUi.inputHeaderText.getGlobalBounds().height / 2);
    this->HighscoreInputMenuUi.inputHeaderText.setPosition(this->window->getSize().x / 2 - 10.f, this->window->getSize().y * 4 / 10);

    //input error text
    this->HighscoreInputMenuUi.inputErrorText = this->CreateText(Arcade, 28, sf::Color::Red, "Please type your name!", true);
    this->HighscoreInputMenuUi.inputErrorText.setPosition(this->window->getSize().x / 2 - 9.f, this->window->getSize().y * 33 / 50);

    //input footer text
    this->HighscoreInputMenuUi.inputFooterText = this->CreateText(Arcade, 32, sf::Color::White, "Press 'Enter' to Save!", true);
    this->HighscoreInputMenuUi.inputFooterText.setPosition(this->window->getSize().x / 2 - 10.f, this->window->getSize().y * 29 / 50);

    //input text
    this->HighscoreInputMenuUi.inputText = this->CreateText(Arcade, 28, sf::Color(219, 206, 190, 255), static_cast<std::string>("\0"), true);
    this->HighscoreInputMenuUi.inputText.setPosition(this->window->getSize().x / 2, this->window->getSize().y * 39 / 80);

    //input background
    this->HighscoreInputMenuUi.inputBackground.setSize(sf::Vector2f(this->window->getSize().x * 6 / 10, 45.f));
    this->HighscoreInputMenuUi.inputBackground.setOrigin(this->HighscoreInputMenuUi.inputBackground.getGlobalBounds().width / 2, this->HighscoreInputMenuUi.inputBackground.getGlobalBounds().height / 2);
    this->HighscoreInputMenuUi.inputBackground.setPosition(this->window->getSize().x / 2, this->window->getSize().y / 2);
    this->HighscoreInputMenuUi.inputBackground.setFillColor(sf::Color(64, 64, 64, 200));

    //quit button text
    this->HighscoreInputMenuUi.quitButtonText = this->CreateText(Arcade, 28, sf::Color::Red, "Quit Game", true);
    this->HighscoreInputMenuUi.quitButtonText.setPosition(this->window->getSize().x * 1 / 4, this->window->getSize().y * 9 / 10);

    //continue button text
    this->HighscoreInputMenuUi.continueButtonText = this->CreateText(Arcade, 28, sf::Color::Green, "Continue w/o Saving", true);
    this->HighscoreInputMenuUi.continueButtonText.setPosition(this->window->getSize().x * 7 / 10, this->window->getSize().y * 9 / 10);
    }

    //===Debug===//
    //debug info text
    this->debugInfo = this->CreateText(Arcade, 36, sf::Color::White, "NONE", true);
    this->debugInfo.setPosition(100.f, 50.f);

    //this->rangeTest.InitRangeChanger(sf::Vector2f(200.f, 100.f), this->window);

    //copy this for setPosition -> .setPosition(this->window->getSize().x, this->window->getSize().y);
}

void Game::InitAudio()
{
    /*
        This method is used for initialising class AudioObject objects, other than initialising sf::Music objects.

        
    */

    //========INIT AUDIO OBJECTS========//

    //music
    if(!this->music.openFromFile("assets/audio/music/Buskerdroid - 3D(istortion).ogg"));
        //error
        
    this->music.setVolume(50.f);
    this->music.setLoop(true);
    //this->music.play();
    this->musicPointer = &this->music;

    this->AudioManager.AddToManager(MusicSound, musicPointer);

    //explosion
    this->explosion.InitAudioObject(this->SFXSound, "explosion2.wav", 50.f);
    this->AudioManager.AddToManager(SFXSound, explosion.ReturnPointer());

    //menu forward
    this->menuForward.InitAudioObject(this->MenuSound, "menu_forward.wav", 50.f);
    this->AudioManager.AddToManager(MenuSound, menuForward.ReturnPointer());

    //menu backwards
    this->menuBackwards.InitAudioObject(this->MenuSound, "menu_backwards.wav", 50.f);
    this->AudioManager.AddToManager(MenuSound, menuBackwards.ReturnPointer());

    //enemy hit
    this->enemyHit.InitAudioObject(this->SFXSound, "enemy_hit.wav", 50.f);
    this->AudioManager.AddToManager(SFXSound, enemyHit.ReturnPointer());


    //assign sounds to UIRangeChanger objects
    this->OptionsMenuUi.menuSoundChanger.InitAudio(this->menuForward.ReturnPointer());
    this->OptionsMenuUi.sfxSoundChanger.InitAudio(this->enemyHit.ReturnPointer());

    //init audio levels according to new save data
    this->AudioManager.SetVolume(AllSound, this->OptionsMenuUi.mainSoundChanger.ReturnVal(0.1f));
    this->AudioManager.SetVolume(MusicSound, this->OptionsMenuUi.musicSoundChanger.ReturnVal(10.f));
    this->AudioManager.SetVolume(MenuSound, this->OptionsMenuUi.menuSoundChanger.ReturnVal(10.f));
    this->AudioManager.SetVolume(SFXSound, this->OptionsMenuUi.sfxSoundChanger.ReturnVal(10.f));

}

//========CONSTRUCTORS/DESTRUCTORS========//
Game::Game()
{
    /*

    */

    this->InitVariables();
    this->InitWindow();
    this->InitEnemies();
    this->InitUI();
    this->DisplayHighScores();
    this->InitAudio();

    int test = static_cast<int>(this->OptionsMenuUi.mainSoundChanger.ReturnVal(10.f));
    this->DebugText<int>(test);

    //this->FileManager.WriteOptionsData(AllSound, test);

    this->window->setKeyRepeatEnabled(false);
}

//delete any memory here
Game::~Game()
{
    /*

    */

    delete this->window;
    delete this->musicPointer;
}

//========ACCESSORS========//
const bool Game::getWindowIsOpen() const
{
    /*

    */

    return this->window->isOpen();
}

//========PUBLIC FUNCTIONS========//
//===update functions===//

//An update function that allows the game to recieve inputs and use them.
void Game::PollEvents()
{
    /*

    */

    //event polling
    while(this->window->pollEvent(this->event))
    {
        switch(this->event.type)
        {
            case sf::Event::Closed:
                this->window->close();
                break;

            case sf::Event::KeyPressed:
                if(this->event.key.code == sf::Keyboard::Escape)
                {
                    switch(this->GameMenu)
                    {
                        case InGameScene:
                            this->ChangeScreens(PauseScene);
                            break;

                        case PauseScene:
                            this->ChangeScreens(InGameScene);
                            break;
                    }
                }

                if(this->event.key.code == sf::Keyboard::Return)
                {
                    if(this->GameMenu == HighscoreInputScene) 
                    {
                        if(this->HighscoreInputMenuUi.inputText.getString() != '\0') {
                            this->FileManager.WriteSaveData(this->SubmitEnteredText(), this->score);
                            this->ChangeScreens(EndScene);
                        }
                    }
                }
                break;

            case sf::Event::MouseButtonPressed:
                if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    this->ClickEnemy();
                    this->ClickUI();
                }
                break;

            case sf::Event::LostFocus:
                if(this->GameMenu == InGameScene)
                    this->ChangeScreens(PauseScene);
                break;

            case sf::Event::TextEntered:
                if(this->GameMenu == HighscoreInputScene)
                    this->EnteredText();
                break;

            case sf::Event::MouseWheelScrolled:
                if(this->event.mouseWheelScroll.delta > 0) {
                    //this->DebugText("scrolled up");
                }
                break;
        }
    }
}

void Game::UpdateClocks()
{
    /*

    */

    this->DeltaTime = this->deltaClock.restart().asSeconds(); //updating delta time

    if(GameMenu == InGameScene)
        this->enemySpawnTimer += this->DeltaTime; //only update if in game
}

void Game::UpdateMousePositions()
{
    /*
        @ return void

        Updates the mouse positions:
        - mouse position relative to the game window (Vector2i)
    */

   this->mousePosWindow = sf::Mouse::getPosition(*this->window);
   this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::UpdateEnemies()
{
    /**
        @ return void

        Updates the enemy spawn timer and spawns an enemy each timer reset
        Moves the enemies downwards
        Deletes any enemies which exceed the window bounds
    */

    //Counting up timer in between each enemy spawn
    if(this->enemies.size() < this->maxEnemies) //check if the total amount of enemies is less than the max
    {
        if(this->enemySpawnTimer >= this->enemySpawnTimerMax) //check if the timer is greater than the max time
        {
            //spawn the enemy
            this->LoadEnemy();
            this->enemySpawnTimer = 0.f; //reset the timer
        }
    }

    
    
    //Update the enemies
    for (int i = 0; i < this->enemies.size(); i++)
    {
        //move enemy
        this->enemies[i].body.move(0.f, this->baseEnemySpeed * this->enemies[i].ReturnEnemyInfo().speed * this->DeltaTime);

        //remove enemies who exit the window bounds
        if (this->enemies[i].body.getPosition().y > this->InGameUi.killLine.getPosition().y)
            this->RemoveEnemy(i, GameKill, this->enemies[i].ReturnEnemyInfo());
    }
    
}

void Game::Update()
{
    /*

    */

    //execute any inputs before anything else
    this->PollEvents();

    //then update everything else
    this->UpdateClocks();
    this->UpdateMousePositions();

    if(this->GameMenu == InGameScene)
        this->UpdateEnemies();
}


//===render functions===//
void Game::RenderEnemies()
{
    /*

    */

    if(GameMenu == InGameScene)
    {
        for(auto &enemy : this->enemies)
        {
            this->window->draw(enemy.body);
        }
    }
}

void Game::RenderUI()
{
    /*

    */


    this->window->draw(debugInfo);

    switch (GameMenu)
    {
        case MainScene:
            this->window->draw(MainMenuUi.titleText);
            this->window->draw(MainMenuUi.exitButtonText);
            this->window->draw(MainMenuUi.playButtonText);
            this->window->draw(MainMenuUi.optionsButtonText);
            break;

        case InGameScene:
            this->window->draw(InGameUi.uiBackground);
            this->window->draw(InGameUi.scoreText);
            this->window->draw(InGameUi.healthText);
            this->window->draw(InGameUi.killLine);
            break;

        case EndScene:
            this->window->draw(EndMenuUi.titleText);
            this->window->draw(EndMenuUi.subTitleText);
            this->window->draw(EndMenuUi.exitButtonText);
            this->window->draw(EndMenuUi.playButtonText);

            for(auto &highScoreText : this->EndMenuUi.highScores)
            {
                this->window->draw(highScoreText);
            }

            break;

        case PauseScene:
            this->window->draw(PauseMenuUi.titleText);
            this->window->draw(PauseMenuUi.resumeButtonText);
            this->window->draw(PauseMenuUi.mainmenuButtonText);
            this->window->draw(PauseMenuUi.quitButtonText);
            break;

        case OptionsScene:
            //main elements
            this->window->draw(OptionsMenuUi.titleText);
            this->window->draw(OptionsMenuUi.mainmenuButtonText);

            //audio elements
            //main
            this->window->draw(OptionsMenuUi.mainSoundText);
            this->OptionsMenuUi.mainSoundChanger.Draw();

            //music
            this->window->draw(OptionsMenuUi.musicSoundText);
            this->OptionsMenuUi.musicSoundChanger.Draw();

            //menu
            this->window->draw(OptionsMenuUi.menuSoundText);
            this->OptionsMenuUi.menuSoundChanger.Draw();

            //sfx
            this->window->draw(OptionsMenuUi.sfxSoundText);
            this->OptionsMenuUi.sfxSoundChanger.Draw();
            break;
        
        case HighscoreInputScene:
            this->window->draw(this->HighscoreInputMenuUi.titleText);
            this->window->draw(this->HighscoreInputMenuUi.inputHeaderText);
            this->window->draw(this->HighscoreInputMenuUi.inputBackground);
            this->window->draw(this->HighscoreInputMenuUi.quitButtonText);
            this->window->draw(this->HighscoreInputMenuUi.continueButtonText);
            //this->window->draw(this->HighscoreInputMenuUi.inputErrorText);
            this->window->draw(this->HighscoreInputMenuUi.inputFooterText);
            if(this->HighscoreInputMenuUi.inputText.getString() != '\0')
                this->window->draw(this->HighscoreInputMenuUi.inputText);
            break;
    }
}

void Game::Render()
{
    /*
    
        - clear old frame
        - render objects
        - display frame in window

        renders the game objects

    */


    this->window->clear();

    //draw game objects
    this->RenderEnemies();
    
    //draw UI LAST
    this->RenderUI();

    this->window->display();
}



//===task functions===//

//Summons a copy of a random enemy (which is defined by their weight) which is defined in #InitEnemies.
void Game::LoadEnemy()
{
    /*

    */

    this->randomNumber = (rand() % 101);

    for(auto &allEnemies : this->weightCalculator)
    {
        if(this->randomNumber >= allEnemies.ReturnSpawnChance().rangeBegin && this->randomNumber <= allEnemies.ReturnSpawnChance().rangeEnd)
        {
            allEnemies.SpawnEnemy();
            this->enemies.push_back(allEnemies);
        }
    }
}

//Scans through every enemy present on screen and evaluates which one is clicked.
void Game::ClickEnemy()
{
    /*

    */

    if(GameMenu == InGameScene)
    {
        for (int i = 0; i < this->enemies.size(); i++)
        {
            if(this->enemies[i].body.getGlobalBounds().contains(this->mousePosView))
            {
                this->RemoveEnemy(i, PlayerKill, enemies[i].ReturnEnemyInfo());
            }
        }
    }
}

//When given an index of an enemy and the method in which it must be removed (PlayerKill, GameKill, NULLKill),
//it will update information (such as health or score) depending on kill method.
void Game::RemoveEnemy(int enemyIndex, KillType killType, Game::EnemyStats enemyStats)
{
    /*

    */

    //run fancy stuff like sounds and anims then remove the enemy
    switch (killType)
    {
    //do stuff if the player removes an enemy
    case PlayerKill:
        enemyStats.health -= 1;

        if(enemyStats.health <= 0)
        {
            this->score += enemyStats.points;
            this->explosion.Play();
            this->RefreshUI(ScoreElement);
            this->enemies.erase(this->enemies.begin() + enemyIndex);
        } else {
            //enemy takes a hit and lives another day
            this->enemyHit.Play();
            enemies[enemyIndex].TakeDamage();
        }
   
        break;

    //do stuff if the game automatically removes the enemy
    case GameKill:
        this->currentHealth -= enemyStats.attack;
        this->RefreshUI(HealthElement);
        if(this->currentHealth < 0) {
            if(this->NewHighScore())
                this->ChangeScreens(HighscoreInputScene);
            else
                this->ChangeScreens(EndScene);
        }
        this->enemies.erase(this->enemies.begin() + enemyIndex);
        break;
    
    case NULLKill:
        //not supposed to do anything
        this->enemies.erase(this->enemies.begin() + enemyIndex);
        break;
    }
}

//Must be called when a dynamic UI element (such as health or score) needs to be updated/refreshed.
//UIElement must = ScoreElement or HealthElement
void Game::RefreshUI(UIElement uiElement)
{
    /*
        @ return void

        This is for updating the dynamic UI elements: Score and Health
        - fill the string stream
        - update text value with refreshed string stream
        - clear the string stream
        - set the string in string stream to blank
        break;
    */

    switch (uiElement)
    {
        case ScoreElement:
            this->InGameUi.scoreString << "Score: " << this->score;
            this->InGameUi.scoreText.setString(this->InGameUi.scoreString.str());
            this->InGameUi.scoreString.clear();
            this->InGameUi.scoreString.str("");
            break;

        case HealthElement:
            this->InGameUi.healthString << "Health: " << this->currentHealth << "/" << this->maxHealth;
            this->InGameUi.healthText.setString(this->InGameUi.healthString.str());
            this->InGameUi.healthString.clear();
            this->InGameUi.healthString.str("");
    }
}

//Function to initiate functionality when a specific UI element is clicked.
void Game::ClickUI()
{
    /*

    */
    
    //Main Menu
    if(GameMenu == MainScene) 
    {
        //play button text
        if(this->MainMenuUi.playButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuForward.Play();
            this->ChangeScreens(InGameScene);
        }

        if(this->MainMenuUi.exitButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuBackwards.Play();
            this->window->close();
        }

        //Options menu button
        if(this->MainMenuUi.optionsButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuForward.Play();
            this->ChangeScreens(OptionsScene);
        }
    }
    
    //End Menu
    if(GameMenu == EndScene)
    {
        if(this->EndMenuUi.playButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuForward.Play();
            this->RestartGame();
            this->ChangeScreens(InGameScene);
        }

        if(this->EndMenuUi.exitButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuBackwards.Play();
            this->window->close();
        }
    }

    //Pause Menu
    if(GameMenu == PauseScene)
    {
        //Resume
        if(this->PauseMenuUi.resumeButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuForward.Play();
            this->ChangeScreens(InGameScene);
        }

        //Main Menu
        if(this->PauseMenuUi.mainmenuButtonText.getGlobalBounds().contains(this->mousePosView)){
            this->menuBackwards.Play();
            this->RestartGame();
            this->ChangeScreens(MainScene);
        }

        //Quit Game
        if(this->PauseMenuUi.quitButtonText.getGlobalBounds().contains(this->mousePosView)){
            this->menuBackwards.Play();
            this->window->close();
        }
    }

    //Options Menu
    if(GameMenu == OptionsScene) 
    {
        //Return to main menu
        if(this->OptionsMenuUi.mainmenuButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->menuBackwards.Play();
            this->ChangeScreens(MainScene);
        }

        //main audio changer
        if(this->OptionsMenuUi.mainSoundChanger.ClickButtons(this->mousePosView)) {
            this->AudioManager.SetVolume(AllSound, this->OptionsMenuUi.mainSoundChanger.ReturnVal(0.1f));
            this->FileManager.WriteOptionsData(AllSound, static_cast<int>(this->OptionsMenuUi.mainSoundChanger.ReturnVal(1.f)));
        }
            

        //music audio changer
        if(this->OptionsMenuUi.musicSoundChanger.ClickButtons(this->mousePosView)) {
            this->AudioManager.SetVolume(MusicSound, this->OptionsMenuUi.musicSoundChanger.ReturnVal(10.f));
            this->FileManager.WriteOptionsData(MusicSound, static_cast<int>(this->OptionsMenuUi.musicSoundChanger.ReturnVal(1.f)));
        }
            

        //menu audio changer
        if(this->OptionsMenuUi.menuSoundChanger.ClickButtons(this->mousePosView)) {
            this->AudioManager.SetVolume(MenuSound, this->OptionsMenuUi.menuSoundChanger.ReturnVal(10.f));
            this->FileManager.WriteOptionsData(MenuSound, static_cast<int>(this->OptionsMenuUi.menuSoundChanger.ReturnVal(1.f)));
        }
            

        //sfx audio changer
        if(this->OptionsMenuUi.sfxSoundChanger.ClickButtons(this->mousePosView)) {
            this->AudioManager.SetVolume(SFXSound, this->OptionsMenuUi.sfxSoundChanger.ReturnVal(10.f));
            this->FileManager.WriteOptionsData(SFXSound, static_cast<int>(this->OptionsMenuUi.sfxSoundChanger.ReturnVal(1.f)));
        }
    }

    //Highscore Input Menu
    if(GameMenu == HighscoreInputScene)
    {
        if(this->HighscoreInputMenuUi.quitButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->window->close();
        }

        if(this->HighscoreInputMenuUi.continueButtonText.getGlobalBounds().contains(this->mousePosView)) {
            this->ChangeScreens(EndScene);
        }
    }
}

//Resets all variables so that the game can restart
void Game::RestartGame()
{
    /*

    */

    this->score = 0;
    this->currentHealth = this->maxHealth;
    this->enemySpawnTimer = 0.f;

    this->RefreshUI(ScoreElement);
    this->RefreshUI(HealthElement);

    this->enemies.clear();
}

//Call when changing game scenes, will update gameMenu + update drawn UI.
//WhichScreen = MainScreen, GameScreen, EndScreen, PauseScreen
void Game::ChangeScreens(Game::GameScene nextScene)
{
    /*

    */

    switch (nextScene)
    {
        case MainScene:
            //code
            break;
        
        case InGameScene:
            if(GameMenu != PauseScene)
                this->enemySpawnTimer = 0.f;
            this->enemyRespawnClock.restart();
            break;

        case EndScene:
            this->DisplayHighScores();
            break;

        case PauseScene:
            //code
            break;

        case OptionsScene:
            //code
            break;
    }

    this->GameMenu = nextScene; //change game menu
}

//Creates sf::Text dummy value which returns the properties set to an existing sf::Text value.
//EG: sf::Text myText = CreateText(whichFont, textSize, textColor, textString);
sf::Text Game::CreateText(WhichFont whichFont, int textSize, sf::Color textColor, std::string textString, bool centerOrigin)
{
    /*

    */

    sf::Text dummyText;

    switch (whichFont)
    {
    case Arcade:
        dummyText.setFont(arcadeFont);
        break;

    case ArcadeLarge:
        dummyText.setFont(arcadeFontLarge);
        break;
    }

    dummyText.setCharacterSize(textSize); //text size
    dummyText.setFillColor(textColor); //colour
    dummyText.setStyle(sf::Text::Bold); //bold
    dummyText.setString(textString); //default text
    if(centerOrigin)
        dummyText.setOrigin(dummyText.getGlobalBounds().width / 2, dummyText.getGlobalBounds().height / 2); //set origin

    //dummyText.setPosition(this->window->getSize().x * 2 / 3, this->window->getSize().y * 6 / 10); //set position - mid centre

    return dummyText;
}

//Updates the content which is displayed as a debug message in the top corner.
template <typename T>
void Game::DebugText(T value)
{
    /*

    */

    std::stringstream debugSS;

    debugSS << "Input: " << value;
    this->debugInfo.setString(debugSS.str());
}

void Game::EnteredText()
{
    /*

    */

    std::stringstream dummyStream;
    char currentChar = this->event.text.unicode;

    if(currentChar >= 32 && currentChar <= 126)
    {
        if(currentChar >= 65 && currentChar <= 90)
            currentChar += 32;

        if(this->currentInput.size() < 10)
        {
            if(this->currentInput.back() != '\0')
                this->currentInput.push_back(static_cast<char>(currentChar));
            else
                this->currentInput.back() = static_cast<char>(currentChar); 
        }
        
    }

    if(this->event.text.unicode == 8 || this->event.text.unicode == 127)
    {
        if(this->currentInput.size() > 1)
            this->currentInput.pop_back();
        else if(this->currentInput.size() <= 1)
            this->currentInput[0] = '\0';
    }
        

    for(auto &character : this->currentInput)
    {
        dummyStream << character;
    }

    this->HighscoreInputMenuUi.inputText.setString(dummyStream.str());
    this->HighscoreInputMenuUi.inputText.setOrigin(this->HighscoreInputMenuUi.inputText.getGlobalBounds().width / 2, this->HighscoreInputMenuUi.inputText.getGlobalBounds().height / 2);

}

std::string Game::SubmitEnteredText()
{
    /*

    */

    std::stringstream dummyStream;

    for(auto &character : this->currentInput)
    {
        dummyStream << character;
    }

    this->currentInput.clear();

    if(this->currentInput.empty())
        this->currentInput.push_back(static_cast<char>('\0'));

    return dummyStream.str();
}

void Game::DisplayHighScores()
{
    /*

    */

    this->highscoreSaveData.clear();
    this->EndMenuUi.highScores.clear();
    this->highscoreSaveData = this->FileManager.ReadFile(SaveFile);

    std::stringstream dummyStream;
    sf::Text dummyText;

    for(int i = 0; i < this->highscoreSaveData.size(); ++i)
    {
        dummyStream << this->highscoreSaveData[i].name << " " << this->highscoreSaveData[i].value;
        dummyText = this->CreateText(Arcade, 28, sf::Color(219, 206, 190, 255), this->highscoreSaveData[i].name, false);

        this->EndMenuUi.highScores.push_back(this->CreateText(Arcade, 28, sf::Color(219, 206, 190, 255), dummyStream.str(), false));
        this->EndMenuUi.highScores[i].setOrigin(dummyText.getGlobalBounds().width + 7.f, dummyText.getGlobalBounds().height);

        if(i == 0)
            this->EndMenuUi.highScores[i].setPosition(this->window->getSize().x / 2, this->window->getSize().y * 7 / 20);
        else
            this->EndMenuUi.highScores[i].setPosition(this->EndMenuUi.highScores[i - 1].getPosition().x, this->EndMenuUi.highScores[i - 1].getPosition().y + this->EndMenuUi.highScores[i - 1].getGlobalBounds().height + 10);

        dummyStream.str("");
        dummyStream.clear();
    }
}

bool Game::NewHighScore()
{
    std::vector<Game::FileData> dummyVector = this->FileManager.ReadFile(SaveFile);

    if(dummyVector.size() >= 10) 
    {
        if(dummyVector.back().value < this->score)
            return true;
        else
            return false;        
    } 
    else 
    {
        return true;
    }

}

//========CLASS FUNCTIONS========//

//class EnemyType

//constructor
Game::EnemyType::EnemyType()
{
    this->CreateEnemy(20.f, 20.f, sf::Color::White, 1, 1, 1, 10, 1);
}

//destructor
Game::EnemyType::~EnemyType()
{

}

//Recieves in an integer for a colour channel which then changes the value.
//Returns an integer.
int Game::EnemyType::GetDifferentColour(int colour)
{
    /*
        @ return int

        This is meant to get an existing R/G/B value and change it to something else
    */

    colour += 71; //add to the colour

    if(colour < 255) //check if the new colour is within 0 - 255, if not, set it within range
        return colour;
    else
        return colour - 255;
}

//Passes in a reference to the game's event manager and video mode variables
void Game::EnemyType::InitEnemy(sf::Event eventManagerIn, sf::VideoMode windowVideoModeIn)
{
    this->eventManager = eventManagerIn;
    this->windowVideoMode = windowVideoModeIn;
}

//Initialise an enemy's stats and properties
void Game::EnemyType::CreateEnemy(float sizeX, float sizeY, sf::Color enemyColour, int enemyHealth, float enemySpeed, int enemyAttack, int enemyPoints, int spawnWeight)
{
    //set enemy size
    this->bodySizeX = sizeX;
    this->bodySizeY = sizeY;
    this->body.setSize(sf::Vector2f(bodySizeX, bodySizeY));

    //set enemy colour
    this->bodyColour = enemyColour;
    this->outlineColour = sf::Color(this->GetDifferentColour(enemyColour.r), this->GetDifferentColour(enemyColour.g), this->GetDifferentColour(enemyColour.b), 255);

    this->body.setFillColor(bodyColour);
    this->body.setOutlineColor(outlineColour);
    this->body.setOutlineThickness(2.f);

    //set enemy stats
    this->health = enemyHealth;
    this->speed = enemySpeed;
    this->attack = enemyAttack;
    this->points = enemyPoints;
    this->weight = spawnWeight;
}

//Sets weight range of enemy
void Game::EnemyType::SetSpawnChance(Game::EnemyWeightRange weightRange)
{
    //this->spawnChanceRange = weightRange;
    this->spawnChanceRangeMin = weightRange.rangeBegin;

    if(weightRange.rangeEnd > 100)
        this->spawnChanceRangeMax = 100;
    else
        this->spawnChanceRangeMax = weightRange.rangeEnd;
}

//Does stuff on each instance of an enemy appearing
void Game::EnemyType::SpawnEnemy()
{
    //set random position
    this->body.setPosition(rand() % static_cast<int>(windowVideoMode.width - this->bodySizeX), rand() % static_cast<int>(this->windowVideoMode.height / 5 * 2 - this->bodySizeY));
}

//Returns the EnemyStats struct with the enemy's unique data eg health
Game::EnemyStats Game::EnemyType::ReturnEnemyInfo()
{
    Game::EnemyStats dummyEnemy;

    dummyEnemy.attack = this->attack;
    dummyEnemy.health = this->health;
    dummyEnemy.points = this->points;
    dummyEnemy.speed = this->speed;
    dummyEnemy.weight = this->weight;

    return dummyEnemy;
}

Game::EnemyWeightRange Game::EnemyType::ReturnSpawnChance()
{
    Game::EnemyWeightRange dummyEnemy;

    dummyEnemy.rangeBegin = this->spawnChanceRangeMin;
    dummyEnemy.rangeEnd = this->spawnChanceRangeMax;

    return dummyEnemy;
}

//Lowers the enemy's health total
void Game::EnemyType::TakeDamage()
{
    this->health -= 1;
}


//class AudioObject

Game::AudioObject::AudioObject()
{
    this->soundPointer = nullptr;
    this->soundPointer = &this->sound; //<----------this aint working
}

Game::AudioObject::~AudioObject()
{
    delete this->soundPointer;
}

//Returns the complete file path to open a specific audio file
std::string Game::AudioObject::LoadFilePath(SoundType soundType, std::string fileName)
{
    //create stringstream
    std::stringstream filePath;

    //init initial path
    filePath << "assets/audio";

    //distinguish which sound type
    switch (soundType)
    {
    case MenuSound:
        filePath << "/menu/";
        break;

    case SFXSound:
        filePath << "/sfx/";
        break;

    case MusicSound:
        //code
        break;
    }

    //find file from "fileName"
    filePath << fileName;

    //return complete file path
    return filePath.str();
}

//Initialises the basic properties to allow the audio object to work
void Game::AudioObject::InitAudioObject(SoundType soundFileType, std::string fileName, float soundVolume)
{
    this->soundBuffer.loadFromFile(this->LoadFilePath(soundFileType, fileName));
    this->sound.setBuffer(soundBuffer);
    this->sound.setVolume(soundVolume);
}

//Play's the sound of the audio object
void Game::AudioObject::Play()
{
    this->sound.play();
}

void Game::AudioObject::SetVolume(float volume)
{
    this->soundPointer->setVolume(volume);
}

sf::Sound* Game::AudioObject::ReturnPointer()
{
    return soundPointer;
}


//class AudioManagerClass
Game::AudioManagerClass::AudioManagerClass()
{
    this->allVolume = 0.5f;
    this->menuVolumeBase = 50.0f;
    this->sfxVolumeBase = 50.0f;
    this->musicVolumeBase = 50.0f;
}

Game::AudioManagerClass::~AudioManagerClass()
{
    for(auto soundObject : this->allMusic) {
        delete soundObject;
    }
    this->allMusic.clear();
    
    for(auto soundObject : this->allMenu) {
        delete soundObject;
    }
    this->allMenu.clear();
    
    for(auto soundObject : this->allSFX) {
        delete soundObject;
    }
    this->allSFX.clear();
}

void Game::AudioManagerClass::AddToManager(SoundType soundType, sf::Sound* soundObject)
{
    switch (soundType)
    {
        case MenuSound:
            this->allMenu.push_back(soundObject);
            break;
            
        case SFXSound:
            this->allSFX.push_back(soundObject);
            break;

        default:
            //error code
            break;
    }
}

void Game::AudioManagerClass::AddToManager(SoundType soundType, sf::Music* musicObject)
{
    switch (soundType)
    {
        case MusicSound:
            this->allMusic.push_back(musicObject);
            break;

        default:
            //error code
            break;
    }
}

void Game::AudioManagerClass::SetVolume(SoundType soundType, float volume)
{
    switch (soundType)
    {
        case AllSound:
            //update
            this->allVolume = volume;

            this->SetVolume(MenuSound, this->menuVolumeBase);
            this->SetVolume(SFXSound, this->sfxVolumeBase);
            this->SetVolume(MusicSound, this->musicVolumeBase);
            break;

        case MenuSound:
            this->menuVolumeBase = volume;
            this->menuVolume = this->allVolume * this->menuVolumeBase;

            //update the volume of all the menu sounds
            for(auto &soundObject : this->allMenu) {
                soundObject->setVolume(menuVolume);
            }
            break;
            
        case SFXSound:
            this->sfxVolumeBase = volume;
            this->sfxVolume = this->allVolume * this->sfxVolumeBase;

            //update the volume of all the sfx sounds
            for(auto &soundObject : this->allSFX) {
                soundObject->setVolume(sfxVolume);
            }
            break;
            
        case MusicSound:
            this->musicVolumeBase = volume;
            this->musicVolume = this->allVolume * this->musicVolumeBase;

            //update the volume of all the music sounds
            for(auto &soundObject : this->allMusic) {
                soundObject->setVolume(musicVolume);
            }
            break;
    }
}


//class UIRangeChanger

//========PRIVATE FUNCTIONS========//

sf::CircleShape Game::UIRangeChanger::CreateTriangle()
{
    sf::CircleShape dummyTriangle;

    dummyTriangle.setRadius(15.f);
    dummyTriangle.setPointCount(3);
    dummyTriangle.setOrigin(dummyTriangle.getGlobalBounds().width / 2, dummyTriangle.getGlobalBounds().height / 2);
    dummyTriangle.setFillColor(sf::Color(210, 210, 210, 255));

    return dummyTriangle;
}

void Game::UIRangeChanger::ChangeDisplayNumber(int value)
{
    std::stringstream ss;
    ss << value;

    this->numberTextP.setString(ss.str());
    this->numberTextP.setOrigin(this->numberTextP.getGlobalBounds().width / 2, this->numberTextP.getGlobalBounds().height / 2);
}

//========PUBLIC FUNCTIONS========//

//constructor - this is called when the object is created
Game::UIRangeChanger::UIRangeChanger()
{
    this->soundRef = nullptr;

    //init number value for display and functionality
    this->numberVal = 5;

    //init the number text for displaying the number value
    this->font.loadFromFile("assets/fonts/pixel/pixelsix10.ttf"); //load font
    this->numberTextP.setFont(font); //set font
    this->numberTextP.setCharacterSize(32); //set char size
    this->numberTextP.setFillColor(sf::Color(210, 210, 210, 255)); //set colour
    this->numberTextP.setStyle(sf::Text::Bold); //set bold
    this->ChangeDisplayNumber(numberVal); //chnage the displayed number to be equal to the number value
    
    //init the left triangle button
    this->buttonLeftP = this->CreateTriangle(); //create the base triangle
    this->buttonLeftP.setRotation(270.f); //rotate the triangle to be facing west

    //init the right triangle button
    this->buttonRightP = this->CreateTriangle(); //create the base triangle
    this->buttonRightP.setRotation(90.f); //rotate the triangle to be facing east

    //audio

    //===base object values have now been initialised===//
}

//destructor - this is called when the object is destroyed
Game::UIRangeChanger::~UIRangeChanger()
{
    //delete allocated memory
    delete this->windowRef;
    delete this->soundRef;
}

//Call when creating the range changer, set the location and assign a reference to the game's window
void Game::UIRangeChanger::InitRangeChanger(sf::Vector2f coords, sf::RenderWindow* windowRefIn, Game::FileData initData)
{
    //assign local version of the window to be equal to the game's version
    this->windowRef = windowRefIn;

    //set the distance that is between the number text and each triangle
    float horizontalSpace = 60.f;

    //set the position of all three elements to be centered around the given coords
    this->numberTextP.setPosition(coords);
    this->buttonLeftP.setPosition(sf::Vector2f(coords.x - horizontalSpace + 2.f, coords.y + 13.f));
    this->buttonRightP.setPosition(sf::Vector2f(coords.x + horizontalSpace, coords.y + 9.f));

    //set the initial value of the volume
    this->numberVal = initData.value;
    this->ChangeDisplayNumber(this->numberVal);
}

void Game::UIRangeChanger::InitAudio(sf::Sound* soundPointer)
{
    this->soundRef = soundPointer;
}

//Call when the range changer's buttons needs to be clicked, pass the location of the mouse pointer
bool Game::UIRangeChanger::ClickButtons(sf::Vector2f mousePos)
{
    bool triggerFunction = false;

    if(this->buttonLeftP.getGlobalBounds().contains(mousePos))
    {
        if(this->numberVal - 1 >= 0) {
            this->numberVal -= 1;
            triggerFunction = true;
        }
    }

    if(this->buttonRightP.getGlobalBounds().contains(mousePos))
    {
        if(this->numberVal + 1 <= 10) {
            this->numberVal += 1;
            triggerFunction = true;
        }
    }

    if(triggerFunction) {
        if(soundRef != nullptr)
            this->soundRef->play();

        this->ChangeDisplayNumber(numberVal);
    }

    return triggerFunction;
}

float Game::UIRangeChanger::ReturnVal(float multiplier)
{
    if(multiplier <= 0)
        multiplier = 1.f;

    return this->numberVal * multiplier;
}

//Call to draw the range changer, will draw all three elements on the assigned window
void Game::UIRangeChanger::Draw()
{
    this->windowRef->draw(this->buttonLeftP);
    this->windowRef->draw(this->buttonRightP);
    this->windowRef->draw(this->numberTextP);
}


//class FileClass

//========PRIVATE FUNCTIONS========//

std::string Game::FileManagerClass::OpenFile(Game::WhichFile whichFile)
{
    switch(whichFile)
    {
        case SaveFile:
            return "files/saveData.txt";
            break;
            
        case OptionsFile:
            return "files/optionsData.txt";
            break;
    }
}

int Game::FileManagerClass::FindElementIndex(std::vector<Game::FileData> vectorInput, int elementValue)
{
    for(int i = 0; i < vectorInput.size(); ++i)
    {
        if(vectorInput[i].value == elementValue)
            return i;
    }

    return -1;
}

std::vector<std::string> Game::FileManagerClass::ConvertVector(std::vector<Game::FileData> vectorInput)
{
    std::vector<std::string> dummyVector;
    std::stringstream dummyStream;

    for(auto &vectorIn : vectorInput)
    {
        dummyStream.str("");
        dummyStream.clear();

        dummyStream << vectorIn.name << " " << vectorIn.value;

        dummyVector.push_back(dummyStream.str());
    }

    return dummyVector;
}

void Game::FileManagerClass::WriteFile(Game::WhichFile whichFile, Game::FileData inputFileData)
{
    this->fileDataLines.clear();
    this->fileLines.clear();
    std::string filePath = this->OpenFile(whichFile);

    if(whichFile == SaveFile)
    {
        this->fileDataLines = this->ReadFile(SaveFile);

        this->fileDataLines.push_back(inputFileData);

        std::sort(this->fileDataLines.begin(), this->fileDataLines.end(), [](const Game::FileData& lhs, const Game::FileData& rhs)
        {
            return lhs.value > rhs.value;
        });

        int newElementIndex = this->FindElementIndex(this->fileDataLines, inputFileData.value);
        
        if(newElementIndex > 10 || this->fileDataLines.size() > 10)
        {
            this->fileDataLines.pop_back();
        }
    }

    if(whichFile == OptionsFile)
    {
        int optionsIndex;

        if(inputFileData.name == "Main")
            optionsIndex = 0;

        if(inputFileData.name == "Music")
            optionsIndex = 1;

        if(inputFileData.name == "Menu")
            optionsIndex = 2;

        if(inputFileData.name == "SFX")
            optionsIndex = 3;

        this->fileDataLines = this->ReadFile(OptionsFile);

        if(this->fileDataLines[optionsIndex].name == inputFileData.name);
            this->fileDataLines[optionsIndex].value = inputFileData.value;
    }

    //file writing
    {
        //
    this->fileLines = this->ConvertVector(this->fileDataLines);

    this->myFileWrite.open(filePath);
    if(this->myFileWrite.is_open())
    {
        for(int i = 0; i < this->fileLines.size(); ++i)
        {
            if(i == 0)
                this->myFileWrite << fileLines[i];
            else
                this->myFileWrite << std::endl << fileLines[i];
        }

        this->myFileWrite.close();
    }
        //
    }
}

//========PUBLIC FUNCTIONS========//

Game::FileManagerClass::FileManagerClass()
{
    
}

Game::FileManagerClass::~FileManagerClass()
{
    this->myFileWrite.close();
    this->myFileRead.close();
}

void Game::FileManagerClass::WriteSaveData(std::string playerName, int playerScore)
{
    Game::FileData dummyFileData;

    dummyFileData.name = playerName;
    dummyFileData.value = playerScore;

    this->WriteFile(SaveFile, dummyFileData);
}

void Game::FileManagerClass::WriteOptionsData(Game::SoundType soundType, int soundValue)
{
    Game::FileData dummyFileData;

    switch(soundType)
    {
        case AllSound:
            dummyFileData.name = "Main";
            break;

        case MusicSound:
            dummyFileData.name = "Music";
            break;

        case MenuSound:
            dummyFileData.name = "Menu";
            break;

        case SFXSound:
            dummyFileData.name = "SFX";
            break;
    }

    dummyFileData.value = soundValue;
    
    this->WriteFile(OptionsFile, dummyFileData);
}

std::vector<Game::FileData> Game::FileManagerClass::ReadFile(Game::WhichFile whichFile)
{
    std::vector<Game::FileData> dummyVector;
    std::string filePath = this->OpenFile(whichFile);

    Game::FileData fileLine;
    this->myFileRead.open(filePath);
    if(this->myFileRead.is_open())
    {
        while(!this->myFileRead.eof())
        {
            this->myFileRead >> fileLine.name >> fileLine.value;
            
            dummyVector.push_back(fileLine);
        }
        this->myFileRead.close();
    }

    if(whichFile == SaveFile)
    {
        std::sort(dummyVector.begin(), dummyVector.end(), [](const Game::FileData& lhs, const Game::FileData& rhs)
        {
            return lhs.value > rhs.value;
        });
    }

    if(whichFile == OptionsFile)
    {
        if(dummyVector.empty())
        {
            fileLine.value = 5;
            fileLine.name = "Main";
            dummyVector.push_back(fileLine);

            fileLine.name = "Music";
            dummyVector.push_back(fileLine);

            fileLine.name = "Menu";
            dummyVector.push_back(fileLine);

            fileLine.name = "SFX";
            dummyVector.push_back(fileLine);
        }
    }
    

    return dummyVector;
}