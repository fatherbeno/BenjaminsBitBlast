#include "Game.cpp"


//main file that runs the engine

int main()
{
    //Init srand
    std::srand(static_cast<unsigned>(time(NULL)));

    Game Game; //init game engine

    //game loop
    while(Game.getWindowIsOpen())
    {
        //game update
        Game.Update();

        //render
        Game.Render();
    }

    //end of application
    return 0;
}