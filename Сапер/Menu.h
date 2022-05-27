#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <conio.h>
using std::string;
using std::cout;

#define ENTER_KEY 13

class Menu
{
    struct Point
    {
        string text;
        string* changeCases = nullptr;
        size_t* curCase = nullptr;
        size_t casesNumber = 0;
    };

    Point* point = nullptr;
    string menuTitle;
    size_t pointsCount;
    size_t coursorPos;

public:
    int chosenPoint;

    Menu(size_t count, string title) :
        chosenPoint(-1),
        pointsCount(count),
        menuTitle(title),
        point(new Point[count]),
        coursorPos(0)
    {}
    ~Menu();
    

    void set_point(size_t number, string value);
    void set_point(size_t number, string value, string* cases, size_t* selector, size_t maxCaseNumber);
    void control();
    void draw();
    
};

