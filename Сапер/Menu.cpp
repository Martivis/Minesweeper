#include "Menu.h"

Menu::~Menu()
{
    delete[] point;
}

void Menu::set_point(size_t number, string value)
{
    point[number].text = value;
}

void Menu::set_point(size_t number, string value, string* cases, size_t* selector, size_t maxCaseNumber)
{
    point[number].text = value;
    point[number].changeCases = cases;
    point[number].curCase = selector;
    point[number].casesNumber = maxCaseNumber;
}

void Menu::control()
{
    bool isExit = false;
    while (!isExit)
    {
        if (_kbhit())
        {
            switch (_getch())
            {
            case 72: // Up
                if (coursorPos > 0)
                    coursorPos--;
                else
                    coursorPos = pointsCount - 1;
                break;
            case 80: // Down
                if (coursorPos < pointsCount - 1)
                    coursorPos++;
                else
                    coursorPos = 0;
                break;
            case 77:
                if (point[coursorPos].changeCases)
                    if (*point[coursorPos].curCase < point[coursorPos].casesNumber)
                        (*point[coursorPos].curCase)++;
                    else
                        *point[coursorPos].curCase = 0;
                break;
            case 75:
                if (point[coursorPos].changeCases)
                    if (*point[coursorPos].curCase > 0)
                        (*point[coursorPos].curCase)--;
                    else
                        *point[coursorPos].curCase = point[coursorPos].casesNumber;
                break;
            case ENTER_KEY:
                system("cls");
                isExit = true;
                break;
            }
            draw();
        }
    }
    chosenPoint = coursorPos;
}

void Menu::draw()
{
    system("cls");
    cout << menuTitle << "\n\n";
    for (size_t i = 0; i < pointsCount; i++)
    {
        if (i == coursorPos)
            cout << "> ";
        else
            cout << "  ";
        cout << point[i].text;
        if (point[i].changeCases)
            cout << '\t' << '<' << point[i].changeCases[*point[i].curCase] << '>';
        cout << '\n';
    }
}
