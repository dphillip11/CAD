#include "App/Application.h"

bool Application::Start()
{
    return true;
}

bool Application::Run()
{
    static int count = 0;
    count++;
    return count < 1000;
}

bool Application::Exit()
{
    return true;
}