// ConsolePongGame.cpp
//
// This is a simple two player pong game. Old school. I implemented this
// just to try doing everything from scratch. 

#include <iostream>
#include <conio.h>
#include <dos.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include <random>

#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 26
#define PI 3.14159265

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

int score1 = 0;
int score2 = 0;

static std::default_random_engine g_generator;

// Position the cursor on the screen
void gotoxy(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

// Control cursor visibility
void setCursor(bool visible, DWORD size)
{
    if (size == 0)
        size = 20;

    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpCursor);
}

class Paddle
{
public:
    Paddle(int player)
    {
        if (player == 1)
            x_ = 0;
        else
            x_ = SCREEN_WIDTH - 2;
        reset();
    }
    ~Paddle() = default;
    Paddle(const Paddle&) = delete;

    void draw()
    {
        gotoxy(x_, y_ - 2); cout << "||";
        gotoxy(x_, y_ - 1); cout << "||";
        gotoxy(x_, y_);     cout << "||";
        gotoxy(x_, y_ + 1); cout << "||";
        gotoxy(x_, y_ + 2); cout << "||";
    }

    void erase()
    {
        gotoxy(x_, y_ - 2); cout << "  ";
        gotoxy(x_, y_ - 1); cout << "  ";
        gotoxy(x_, y_);     cout << "  ";
        gotoxy(x_, y_ + 1); cout << "  ";
        gotoxy(x_, y_ + 2); cout << "  ";
    }

    void update(int dy)
    {
        int newy = y_ + dy;
        if (newy < 4 or newy > 22)
            return;
        erase();
        y_ += dy;
        draw();
    }

    void reset()
    {
        erase();
        y_ = SCREEN_HEIGHT / 2;
        draw();
    }

    int ymin() const { return y_ - 2; }
    int ymax() const { return y_ + 2; }

private:
    int x_;
    int y_;
};

class Ball
{
public:

    Ball() = default;
    ~Ball() = default;
    Ball(const Ball&) = delete;

    void launch()
    {
        std::uniform_real_distribution<double> distribution(10.0, 30.0);
        
        x_ = SCREEN_WIDTH / 2.0;
        y_ = SCREEN_HEIGHT / 2.0;
        double ang = distribution(g_generator);
        dx_ = cos(ang * PI / 180.0) / 3.0;
        if (rand() % 2)
            dx_ = -dx_;
        dy_ = sin(ang * PI / 180.0) / 3.0;
        if (rand() % 2)
            dy_ = -dy_;
        posx_ = round(x_);
        posy_ = round(y_);
    }

    int update(const Paddle &player1, const Paddle &player2)
    {
        x_ += dx_;
        y_ += dy_;
        posx_ = round(x_);
        posy_ = round(y_);

        if (posy_ <= 2 and dy_ < 0.0)
        {
            posy_ = 2;
            y_ = 2.0;
            dy_ = -dy_;
        }
        else if (posy_ >= 23 and dy_ > 0.0)
        {
            posy_ = 23;
            y_ = 23.0;
            dy_ = -dy_;
        }
        if (posx_ <= 2 and dx_ < 0.0)
        {
            if (posx_ <= 0)
            {
                return 1;
            }
            else if (posy_ >= player1.ymin()-1 and posy_ <= player1.ymax())
            {
                posx_ = 2;
                x_ = 2.0;
                dx_ = -dx_;
                // speed up 
                dx_ *= 1.05;
                dy_ *= 1.05;
            }
        }
        else if (posx_ >= SCREEN_WIDTH - 4 and dx_ > 0.0)
        {
            if (posx_ >= SCREEN_WIDTH - 1)
            {
                return 2;
            }
            else if (posy_ >= player2.ymin()-1 and posy_ <= player2.ymax())
            {
                posx_ = SCREEN_WIDTH - 4;
                x_ = SCREEN_WIDTH - 4;
                dx_ = -dx_;
                // speed up 
                dx_ *= 1.05;
                dy_ *= 1.05;
            }
        }
        return 0;
    }

    void adjustSpeed(float delta)
    {
        dx_ *= delta;
        dy_ *= delta;
    }

    void draw()
    {
        gotoxy(posx_, posy_);     cout << "**";
        gotoxy(posx_, posy_ + 1); cout << "**";
    }

    void erase()
    {
        gotoxy(posx_, posy_);     cout << "  ";
        gotoxy(posx_, posy_ + 1); cout << "  ";
    }

private:
    int posx_, posy_;
    double x_, y_;
    double dx_, dy_;
};

void updateScore()
{
    gotoxy(10, 0); 
    cout << "Player 1: " << score1;
    gotoxy(SCREEN_WIDTH - 21, 0); 
    cout << "Player 2: " << score2;
}

void drawGame()
{
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        gotoxy(x, 1); cout << "=";
        gotoxy(x, SCREEN_HEIGHT - 1); cout << "=";
    }
    updateScore();
}

void gameOver(int winner)
{
    int x = SCREEN_WIDTH / 2 - 15;
    gotoxy(x, 4);  cout << "                              ";
    gotoxy(x, 5);  cout << "  +------------------------+  ";
    gotoxy(x, 6);  cout << "  |       GAME OVER        |  ";
    gotoxy(x, 7);  cout << "  +------------------------+  ";
    gotoxy(x, 8);  cout << "                              ";
    gotoxy(x, 9);  cout << "  Final Score: " << score1 << " - " << score2 << "            ";
    gotoxy(x, 10); cout << "                              ";
    gotoxy(x, 11); cout << "  Player " << winner << " wins!             ";
    gotoxy(x, 13); cout << "                              ";
    gotoxy(x, 13); cout << "  Press any key for menu...   ";
    gotoxy(x, 14); cout << "                              ";

    _getch();
}


void instructions()
{
    int x = SCREEN_WIDTH / 2 - 15;
    gotoxy(x, 4);  cout << "                              ";
    gotoxy(x, 5);  cout << "  +------------------------+  ";
    gotoxy(x, 6);  cout << "  |      INSTRUCTIONS      |  ";
    gotoxy(x, 7);  cout << "  +------------------------+  ";
    gotoxy(x, 8);  cout << "                              ";
    gotoxy(x, 9);  cout << "  Keep the ball moving. If    ";
    gotoxy(x, 10); cout << "  you let it get by your      ";
    gotoxy(x, 11); cout << "  paddle you lose the point.  ";
    gotoxy(x, 12); cout << "  First to five points wins!  ";
    gotoxy(x, 13); cout << "                              ";
    gotoxy(x, 14); cout << "  Press any key for menu...   ";
    gotoxy(x, 15); cout << "                              ";

    _getch();
}

char menu()
{
    int x = SCREEN_WIDTH / 2 - 15;
    gotoxy(x, 4);  cout << "                              ";
    gotoxy(x, 5);  cout << "  +------------------------+  ";
    gotoxy(x, 6);  cout << "  |          PONG          |  ";
    gotoxy(x, 7);  cout << "  +------------------------+  ";
    gotoxy(x, 8);  cout << "                              ";
    gotoxy(x, 9);  cout << "  1. Start Game               ";
    gotoxy(x, 10); cout << "  2. Instructions             ";
    gotoxy(x, 11); cout << "  3. Quit                     ";
    gotoxy(x, 12); cout << "                              ";
    gotoxy(x, 13); cout << "  Select option:              ";
    gotoxy(x, 14); cout << "                              ";
    gotoxy(x, 15); cout << "                              ";

    gotoxy(x + 18, 13);
    return _getche();
}

void eraseMenu()
{
    int x = SCREEN_WIDTH / 2 - 15;
    gotoxy(x, 4);  cout << "                              ";
    gotoxy(x, 5);  cout << "                              ";
    gotoxy(x, 6);  cout << "                              ";
    gotoxy(x, 7);  cout << "                              ";
    gotoxy(x, 8);  cout << "                              ";
    gotoxy(x, 9);  cout << "                              ";
    gotoxy(x, 10); cout << "                              ";
    gotoxy(x, 11); cout << "                              ";
    gotoxy(x, 12); cout << "                              ";
    gotoxy(x, 13); cout << "                              ";
    gotoxy(x, 14); cout << "                              ";
    gotoxy(x, 15); cout << "                              ";
}

void play()
{
    eraseMenu();
    score1 = score2 = 0;
    updateScore();

    Ball ball;
    ball.launch();
    ball.draw();

    Paddle player1(1);
    player1.draw();
    Paddle player2(2);
    player2.draw();

    while (1)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q')
                player1.update(-1);
            else if (ch == 'a' || ch == 'A')
                player1.update(1);
            else if (ch == 72)
                player2.update(-1);
            else if (ch == 80)
                player2.update(1);
        }
        //draw player paddles
        ball.erase();
        int ret = ball.update(player1, player2);
        ball.draw();
        if (ret)
        {
            if (ret == 1)
            {
                score2++;
                if (score2 == 5)
                {
                    updateScore();
                    gameOver(2);
                    return;
                }
            }
            else
            {
                score1++;
                if (score1 == 5)
                {
                    updateScore();
                    gameOver(1);
                    return;
                }
            }
            ball.erase();
            ball.launch();
            updateScore();
            player1.reset();
            player2.reset();
        }
        Sleep(10);
    }
}

int main()
{
    setCursor(0, 0);
    srand((unsigned)time(NULL));
    drawGame();

    do {
        char op = menu();
        if (op == '1') play();
        else if (op == '2') instructions();
        else if (op == '3') exit(0);
    } while (1);

    return 0;

}
