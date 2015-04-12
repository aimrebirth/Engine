%module Polygon4

%include <std_string.i>

%{
#include "Game.h"
%}

namespace polygon4
{

class Game
{
public:
    void OpenLevel(std::string level);
};

}

