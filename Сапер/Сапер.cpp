
#include <iostream>
#include <ctime>
#include <conio.h>
#include <string>

#include "Menu.h"

using std::cout;
using std::cin;
using std::string;

const clock_t DRAW_DELAY = 1000;             // Задержка для отрисовки карты
const clock_t COURSOR_BLINK_DELAY = 1000;    // Задержка моргания курсора
const clock_t DRAW_SLEEP_DELAY = 5100;       // Задержка засыпания моргания курсора

const char OPEN_FIELD_KEY = 48;              // Num 0
const char ALT_OPEN_FIELD_KEY = 13;
const char SET_FLAG_KEY = '.';               // Num Del

class Matrix
{
public:
    short int gameOver = 0;                  // 0 - игра в процессе; 1 - победа; -1 - поражение

    void draw();
    void draw_explosion();
    void control();
    bool generate();
    
    Matrix(size_t height, size_t width, unsigned short int bmCount);
    ~Matrix();
    
private:
    char** bombs = nullptr;                  // Карта с бомбами
    unsigned int openedCount = 0;            // Счетчик открытых элементов
    size_t rows;
    size_t cols;
    size_t coursorXPos = 0, coursorYPos = 0;
    unsigned int winScore = 0;               // кол-во открытых клеток для победы
    clock_t coursorTimer = 0;
    clock_t drawTimer = 0;
    clock_t sleepTimer = 0;
    time_t gameStartTime;
    bool showCoursor = true;
    bool forcedDraw = false;

    unsigned short int bombsCount = 0;
    unsigned short int leftBombs = 0;

    void open_element(size_t row, size_t col);
    void open_group(size_t row, size_t col);
    void generate_bombs();
    
};

int main()
{
    setlocale(0, "ru");
    srand(time(0));

    string* difficultly = new string[4];
    size_t currentDiff = 0;
    difficultly[0] = "Демо";
    difficultly[1] = "Легкий";
    difficultly[2] = "Нормальный";
    difficultly[3] = "Сложный";

    unsigned short int diffBombsCount[3][4] = { 
        {10, 15, 22, 35},  // 10*10
        {20, 32, 48, 75},  // 15*15
        {60, 90, 130, 210} // 20*30
    };

    string* mapSize = new string[3];
    size_t currentMapSize = 0;
    mapSize[0] = "Маленький";
    mapSize[1] = "Средний";
    mapSize[2] = "Большой";

    size_t mapSizes[3][2] = {
        { 10, 10 },
        { 15, 15 },
        { 20, 30 }
    };

    Menu MainMenu(3, "Сапер: Console Edition");
    MainMenu.set_point(0, "Новая игра");
    MainMenu.set_point(1, "Настройки");
    MainMenu.set_point(2, "Выход");
    MainMenu.draw();
    bool isExit = false;
    do
	{
        MainMenu.control();
		switch (MainMenu.chosenPoint)
		{
		case 0:
        {
            Matrix Map(mapSizes[currentMapSize][0], mapSizes[currentMapSize][1], diffBombsCount[currentMapSize][currentDiff]);

            while (!Map.generate())
                Map.draw();

            while (!Map.gameOver)
            {
                Map.control();
                Map.draw();
            }

            if (Map.gameOver == 1)
            {
                cout << "Великолепно!\nНажмите Enter для возврата в главное меню\n";
            }
            else
            {
                Map.draw_explosion();
                cout << "Game over!\nНажмите Enter для возврата в главное меню\n";
            }
            cin.get();
            MainMenu.draw();
        }
			break;
		case 1:
        {
            Menu Settings(3, "Настройки");
            Settings.set_point(0, "Размер поля", mapSize, &currentMapSize, 2);
            Settings.set_point(1, "Сложность", difficultly, &currentDiff, 3);
            Settings.set_point(2, "Назад");
            Settings.draw();
            bool isBack = false;
            do
            {
                Settings.control();
                switch (Settings.chosenPoint)
                {
                case 2:
                    isBack = true;
                    break;
                }
            } while (!isBack);
            MainMenu.draw();
        }
			break;
		case 2:
			isExit = true;
			break;
		}
    } while (!isExit);
    
    return 0;
}

void Matrix::draw()
{
    if (clock() - drawTimer > DRAW_DELAY || forcedDraw) // Для засыпания && clock() - sleepTimer < DRAW_SLEEP_DELAY
    {
        system("cls");

        if (clock() - coursorTimer > COURSOR_BLINK_DELAY) // Цикл моргания курсора
        {
            showCoursor = !showCoursor;
            coursorTimer = clock();
        }
        cout << "@: " << leftBombs << '\t' << (time(0) - gameStartTime) / 60 << ':' << (time(0) - gameStartTime) % 60 << '\n';
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
                if (showCoursor && i == coursorYPos && j == coursorXPos)
                    cout << '+' << ' ';
                else
                    if (bombs[i][j] % 10 == 1) // Проверка флага открытия ячейки
                        cout << bombs[i][j] / 10 << ' ';
                    else
                        if (bombs[i][j] % 10 == 2)
                            cout << '!' << ' ';
                        else
                            if (bombs[i][j] % 10 == 3)
                                cout << '?' << ' ';
                            else
                                cout << '#' << ' ';
            cout << '\n';
        }
        drawTimer = clock();
        forcedDraw = false;
    }
}

void Matrix::draw_explosion()
{
    system("cls");
    cout << "\a@: " << leftBombs << '\t' << (time(0) - gameStartTime) / 60 << ':' << (time(0) - gameStartTime) % 60 << '\n';
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
            if (bombs[i][j] % 10 == 1) // Проверка флага открытия ячейки
                cout << bombs[i][j] / 10 << ' ';
            else
                if (bombs[i][j] / 10 == 9)
                    cout << '@' << ' ';
                else
                    if (bombs[i][j] % 10 == 2)
                        cout << 'X' << ' ';
                    else
                        cout << '#' << ' ';
        cout << '\n';
    }
}

void Matrix::control()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 75: // Left
            coursorXPos > 0 ? coursorXPos-- : coursorXPos = cols - 1;
            showCoursor = true;
            coursorTimer = clock();
            break;
        case 72: // Up
            coursorYPos > 0 ? coursorYPos-- : coursorYPos = rows - 1;
            showCoursor = true;
            coursorTimer = clock();
            break;
        case 77: // Right
            coursorXPos < cols - 1 ? coursorXPos++ : coursorXPos = 0;
            showCoursor = true;
            coursorTimer = clock();
            break;
        case 80: // Down
            coursorYPos < cols - 1 ? coursorYPos++ : coursorYPos = 0;
            showCoursor = true;
            coursorTimer = clock();
            break; // 48 - 0 46 - .
        case ALT_OPEN_FIELD_KEY:
        case OPEN_FIELD_KEY:
            switch (bombs[coursorYPos][coursorXPos] / 10)
            {
            case 9:
                gameOver = -1;
                break;
            case 0:
                open_group(coursorYPos, coursorXPos);
            default:
                open_element(coursorYPos, coursorXPos);
                break;
            }
            showCoursor = false;
            coursorTimer = clock();
            break;
        case SET_FLAG_KEY:
            switch (bombs[coursorYPos][coursorXPos] % 10)
            {
            case 0:
                if (leftBombs > 0)
                {
                    bombs[coursorYPos][coursorXPos] += 2;
                    leftBombs--;
                }
                else
                    bombs[coursorYPos][coursorXPos] += 3;
                break;
            case 2:
                bombs[coursorYPos][coursorXPos] += 1;
                leftBombs++;
                break;
            case 3:
                bombs[coursorYPos][coursorXPos] -= 3;
                break;
            }
            showCoursor = false;
            coursorTimer = clock();
            break;
        }
        forcedDraw = true;
        //sleepTimer = clock(); //Для засыпания
    }
}

bool Matrix::generate()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 75: // Left
            coursorXPos > 0 ? coursorXPos-- : coursorXPos = cols - 1;
            showCoursor = true;
            coursorTimer = clock();
            break;
        case 72: // Up
            coursorYPos > 0 ? coursorYPos-- : coursorYPos = rows - 1;
            showCoursor = true;
            coursorTimer = clock();
            break;
        case 77: // Right
            coursorXPos < cols - 1 ? coursorXPos++ : coursorXPos = 0;
            showCoursor = true;
            coursorTimer = clock();
            break;
        case 80: // Down
            coursorYPos < cols - 1 ? coursorYPos++ : coursorYPos = 0;
            showCoursor = true;
            coursorTimer = clock();
            break; // 48 - 0 46 - .
        case ALT_OPEN_FIELD_KEY:
        case OPEN_FIELD_KEY:
            generate_bombs();
            open_group(coursorYPos, coursorXPos);
            showCoursor = false;
            coursorTimer = clock();
            return true;
            break;
        case SET_FLAG_KEY:
            switch (bombs[coursorYPos][coursorXPos] % 10)
            {
            case 0:
                if (leftBombs > 0)
                {
                    bombs[coursorYPos][coursorXPos] += 2;
                    leftBombs--;
                }
                else
                    bombs[coursorYPos][coursorXPos] += 3;
                break;
            case 2:
                bombs[coursorYPos][coursorXPos] += 1;
                leftBombs++;
                break;
            case 3:
                bombs[coursorYPos][coursorXPos] -= 3;
                break;
            }
            showCoursor = false;
            coursorTimer = clock();
            break;
        }
        forcedDraw = true;
        //sleepTimer = clock(); //Для засыпания
    }
    return 0;
}

Matrix::Matrix(size_t height, size_t width, unsigned short int bmCount)
{
    rows = height;
    cols = width;
    bombsCount = bmCount;
    leftBombs = bmCount;
    winScore = cols * rows - bombsCount;
    sleepTimer = clock();
    gameStartTime = time(0);
    bombs = new char* [rows];
    for (size_t i = 0; i < rows; i++)
    {
        bombs[i] = new char[cols];
        for (size_t j = 0; j < cols; j++)
            bombs[i][j] = 0;
    }
}

Matrix::~Matrix()
{
    for (size_t i = 0; i < rows; i++)
    {
        delete[] bombs[i];
    }
}

void Matrix::open_element(size_t row, size_t col)
{
    if (!(bombs[row][col] % 10))
    {
        bombs[row][col] += 1;
        openedCount++;
        if (openedCount == winScore)
            gameOver = 1;
    }
}

void Matrix::open_group(size_t row, size_t col)
{
    open_element(row, col);
    for (int t = (row > 0 ? -1 : 0); t <= (row < rows - 1 ? 1 : 0); t++)
        for (int k = (col > 0 ? -1 : 0); k <= (col < cols - 1 ? 1 : 0); k++)
        {
            if (t != 0 || k != 0)
                if (bombs[row + t][col + k] == 0)
                    open_group(row + t, col + k);
                else
                    open_element(row + t, col + k);
        }
}

void Matrix::generate_bombs()
{
    for (int i = 0; i < bombsCount; i++) // Генерируем бомбы
    {
        bool isGenerated = false;
        do
        {
            size_t tempX = rand() % (cols - 1), tempY = rand() % (rows - 1);
            if (bombs[tempY][tempX] != 90 &&
                (tempX != coursorXPos || tempY != coursorYPos) && (tempX != coursorXPos || tempY != coursorYPos + 1) &&
                (tempX != coursorXPos + 1 || tempY != coursorYPos) && (tempX != coursorXPos + 1 || tempY != coursorYPos + 1) &&
                (tempX != coursorXPos + 1 || tempY != coursorYPos - 1) && (tempX != coursorXPos - 1 || tempY != coursorYPos + 1) &&
                (tempX != coursorXPos - 1 || tempY != coursorYPos - 1) && (tempX != coursorXPos || tempY != coursorYPos - 1) &&
                (tempX != coursorXPos - 1 || tempY != coursorYPos))
            {
                bombs[tempY][tempX] = 90;
                isGenerated = true;
            }
        } while (!isGenerated);
    }

    for (int i = 0; i < rows; i++) // Считаем бомбы вокруг элементов
        for (int j = 0; j < cols; j++)
        {
            if (bombs[i][j] != 90)
            {
                short int curBombsCount = 0;
                for (int t = (i > 0 ? -1 : 0); t <= (i < rows - 1 ? 1 : 0); t++)
                    for (int k = (j > 0 ? -1 : 0); k <= (j < cols - 1 ? 1 : 0); k++)
                        if ((t != 0 || k != 0) && bombs[i + t][j + k] == 90)
                            curBombsCount++;
                bombs[i][j] += curBombsCount * 10;
            }
        }
}
