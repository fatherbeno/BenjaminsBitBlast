#pragma once

//cpp includes
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <algorithm>
 
//sfml includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>

//custom includes


/*
    Class that acts as a game engine
    Wrapper class.
*/

class Game
{
private:
    //========VARIABLES========//
    //window
    sf::RenderWindow* window;
    sf::ContextSettings windowSettings;
    sf::VideoMode videoMode;
    sf::Event event;

    //window size dimensions
    float windowSizeX;
    float windowSizeY;

    //mouse positions
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;

    //enums
    //enum WhichScreen {MainScreen, GameScreen, EndScreen, PauseScreen, OptionsScreen};
    enum GameScene {MainScene, InGameScene, EndScene, PauseScene, OptionsScene, HighscoreInputScene};
    enum KillType {PlayerKill, GameKill, NULLKill};
    enum UIElement {ScoreElement, HealthElement};
    enum WhichFont {Arcade, ArcadeLarge};
    enum SoundType {AllSound, MusicSound, MenuSound, SFXSound}; 
    enum WhichFile {SaveFile, OptionsFile};

    //structs for classes
    struct EnemyStats {
        int health;
        float speed;
        int attack;
        int points;
        int weight;
    };

    struct EnemyWeightRange {
        int rangeBegin = 0;
        int rangeEnd = 0;
    };

    struct FileData {
        std::string name;
        int value;
    };
    

    //========OBJECT CLASSES========//

    //enemy object class for creating enemies

    class EnemyType
    {
    public:
        //body
        sf::RectangleShape body;

    private:
        //important game stuff
        sf::Event eventManager;
        sf::VideoMode windowVideoMode;

        //body info
        sf::Color bodyColour;
        sf::Color outlineColour;
        float bodySizeX;
        float bodySizeY;

        //enemy stats
        int health;
        float speed;
        int attack;
        int points;

        //spawning info
        int weight;
        int spawnChanceRangeMax;
        int spawnChanceRangeMin;

        //private functions
        int GetDifferentColour(int colour);
        
    public:
        EnemyType();
        virtual ~EnemyType();

        //functions

        void InitEnemy(sf::Event eventManagerIn, sf::VideoMode windowVideoModeIn);
        void CreateEnemy(float sizeX, float sizeY, sf::Color enemyColour, int enemyHealth, float enemySpeed, int enemyAttack, int enemyPoints, int spawnWeight);
        void SetSpawnChance(Game::EnemyWeightRange weightRange);
        void SpawnEnemy();
        EnemyStats ReturnEnemyInfo();
        EnemyWeightRange ReturnSpawnChance();
        void TakeDamage();
    };

    //audio object class for creating sfml sounds

    class AudioObject
    {
    private:
        std::string LoadFilePath(SoundType soundType, std::string fileName);

    public:
        AudioObject();
        virtual ~AudioObject();

        sf::SoundBuffer soundBuffer;
        sf::Sound sound;
        sf::Sound* soundPointer;
        void InitAudioObject(SoundType soundFileType, std::string fileName, float soundVolume);
        void Play();
        void SetVolume(float volume);
        sf::Sound* ReturnPointer();
    };
    
    //audio manager class for changing audio levels

    class AudioManagerClass
    {
    private:

        float allVolume;

        float musicVolume;
        float sfxVolume;
        float menuVolume;

        float musicVolumeBase;
        float sfxVolumeBase;
        float menuVolumeBase;

        std::vector<sf::Music*> allMusic;
        std::vector<sf::Sound*> allSFX;
        std::vector<sf::Sound*> allMenu;

    public:
        AudioManagerClass();
        virtual ~AudioManagerClass();

        void AddToManager(SoundType soundType, sf::Sound* soundObject);
        void AddToManager(SoundType soundType, sf::Music* musicObject);
        void SetVolume(SoundType soundType, float volume);
    };

    //ui class for creating left/right buttons for changing between a range <--- probably 0 to 10

    class UIRangeChanger
    {
    private:

        //window reference from the game class
        sf::RenderWindow* windowRef;

        //triangle shapes for left and right arrows
        sf::CircleShape buttonLeftP;
        sf::CircleShape buttonRightP;

        //text for displaying selected number value
        sf::Font font;
        sf::Text numberTextP;

        //number value for functionality
        float numberVal;

        //sound reference for playing audio
        sf::Sound* soundRef;

        //private functions
        sf::CircleShape CreateTriangle();
        void ChangeDisplayNumber(int value);


    public:

        //constuctor/destructor
        UIRangeChanger();
        virtual ~UIRangeChanger();

        //public functions
        void InitRangeChanger(sf::Vector2f coords, sf::RenderWindow* windowRef, Game::FileData initData);
        void InitAudio(sf::Sound* soundPointer);
        bool ClickButtons(sf::Vector2f mousePos);
        float ReturnVal(float multiplier);
        void Draw();
    };

    //file reader/writer class

    class FileManagerClass
    {
    private:
        std::ofstream myFileWrite;
        std::ifstream myFileRead;
        std::vector<std::string> fileLines;
        std::vector<Game::FileData> fileDataLines;

        int elementValue;
        
        std::string OpenFile(Game::WhichFile whichFile);
        int FindElementIndex(std::vector<Game::FileData> vectorInput, int elementValue);
        std::vector<std::string> ConvertVector(std::vector<Game::FileData> vectorInput);

        void WriteFile(Game::WhichFile whichFile, Game::FileData inputFileData);
        void EncryptFile();
        void DecryptFile();

    public:
        FileManagerClass();
        virtual ~FileManagerClass();

        void WriteSaveData(std::string playerName, int playerScore);
        void WriteOptionsData(Game::SoundType soundType, int soundValue);

        std::vector<Game::FileData> ReadFile(Game::WhichFile whichFile);

        //https://codescracker.com/cpp/program/cpp-program-encrypt-file.htm <--- good resource for encryption and decryption
    };

    //========USER INTERFACE========//

    //note: each menu has it's own struct in an attempt to organise everything

    //fonts
    sf::Font arcadeFont;
    sf::Font arcadeFontLarge;

    //in game ui
    struct InGameUiStruct
    {
        sf::Text scoreText;
        sf::Text healthText;
        std::stringstream scoreString;
        std::stringstream healthString;
        sf::RectangleShape killLine;
        sf::RectangleShape uiBackground;
    };

    InGameUiStruct InGameUi;
    
    //menus
    struct MainMenuStruct
    {
        sf::Text titleText;
        sf::Text exitButtonText;
        sf::Text playButtonText;
        sf::Text optionsButtonText;
    };

    MainMenuStruct MainMenuUi;

    //pause menu
    struct PauseMenuStruct
    {
        sf::Text titleText;
        sf::Text resumeButtonText;
        sf::Text mainmenuButtonText;
        sf::Text quitButtonText;
    };

    PauseMenuStruct PauseMenuUi;


    //options menu
    struct OptionsMenuStruct
    {
        sf::Text titleText;
        sf::Text mainmenuButtonText;

        sf::Text mainSoundText;
        UIRangeChanger mainSoundChanger;

        sf::Text musicSoundText;
        UIRangeChanger musicSoundChanger;

        sf::Text menuSoundText;
        UIRangeChanger menuSoundChanger;

        sf::Text sfxSoundText;
        UIRangeChanger sfxSoundChanger;
    };

    OptionsMenuStruct OptionsMenuUi;


    //end menu
    struct EndMenuStruct
    {
        sf::Text titleText;
        sf::Text subTitleText;

        std::vector<sf::Text> highScores;
        sf::RectangleShape scoreListBounds;

        sf::Text exitButtonText;
        sf::Text playButtonText;
    };

    EndMenuStruct EndMenuUi;


    //highscore input menu
    struct HighscoreInputMenuStruct
    {
        sf::Text titleText;
        sf::Text inputHeaderText;
        sf::Text inputErrorText;
        sf::Text inputFooterText;
        sf::Text inputText;
        sf::RectangleShape inputBackground;
        sf::Text quitButtonText;
        sf::Text continueButtonText;
    };

    HighscoreInputMenuStruct HighscoreInputMenuUi;
    
    sf::Text debugInfo;


    //========GAME LOGIC========//
    //timer variables
    sf::Clock deltaClock;
    float DeltaTime;

    sf::Clock enemyRespawnClock;
    float enemySpawnTimer;
    //player variables
    //WhichScreen gameMenu;
    GameScene GameMenu;
    int score;
    int maxHealth;
    int currentHealth;
    int randomNumber;
    float baseEnemySpeed;
    int maxHighscores;

    //enemy variables
    float enemySpawnTimerMax;
    int maxEnemies;

    //========ENEMIES========//
    std::vector<EnemyType> enemies; //vector that holds all enemy types
    std::vector<EnemyType> weightCalculator;
    float totalWeights;

    EnemyType regularEnemy;
    EnemyType fastEnemy;
    EnemyType tankEnemy;
    //EnemyType bossEnemy;
    //EnemyType finnEnemy;
    //EnemyType kenEnemy;
    

    //========AUDIO========//
    //audio manager for common sounds - NOT MUSIC
    AudioManagerClass AudioManager;

    //music
    std::vector<sf::Music> MusicManager;
    sf::Music music;
    sf::Music* musicPointer;

    //sfx
    AudioObject explosion;
    AudioObject enemyHit;

    //menu
    AudioObject menuForward;
    AudioObject menuBackwards;

    //ambient

    //========TEXT INPUT========//

    std::vector<char> currentInput;

    //========FILE DATA========//

    FileManagerClass FileManager;
    std::vector<Game::FileData> highscoreSaveData;
    std::vector<Game::FileData> optionsSaveData;


    //========PRIVATE FUNCTIONS========//
    void InitVariables();
    void InitWindow();
    void InitEnemies();
    void InitEnemyWeights();
    void InitUI();
    void InitAudio();

public:
    //========CONSTRUCTORS/DESTRUCTORS========//
    Game(); //constructor
    virtual ~Game(); //destuctor

    //========ACCESSORS========//
    const bool getWindowIsOpen() const; //check if the game window is open and return a bool

    //========FUNCTIONS========//

    //update functions
    void UpdateClocks();
    void UpdateMousePositions();
    void UpdateEnemies();
    void Update();

    //render functions
    void RenderEnemies();
    void RenderUI();
    void Render();

    //task functions
    void PollEvents();
    void LoadEnemy();
    void ClickEnemy();
    void RemoveEnemy(int enemyIndex, KillType killType, EnemyStats enemyStats);
    void RefreshUI(UIElement uiElement);
    void ClickUI();
    void ChangeScreens(Game::GameScene nextScene);
    void RestartGame();
    sf::Text CreateText(WhichFont whichFont, int textSize, sf::Color textColor, std::string textString, bool centerOrigin);
    template <typename T>
    void DebugText(T value);
    void EnteredText();
    std::string SubmitEnteredText();
    void DisplayHighScores();
    bool NewHighScore();
};