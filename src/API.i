%module Polygon4

%include <std_string.i>

%{
#include "Game.h"
%}

%include "../include/Polygon4/UnrealTypes.h"

namespace polygon4
{

class Game
{
public:
    void OpenLevel(std::string level);
    void SpawnPlayer(Vector v, Rotation r);
};

}

