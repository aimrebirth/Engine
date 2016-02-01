%module Polygon4

//%include <shared_ptr.i>
%include <stdint.i>
%include <std_string.i>

//%ignore DLL_EXPORT;

%{
//#include "../include/Polygon4/UnrealTypes.h"

//#include "k:/AIM/DataManager/include/Polygon4/DataManager/Types.h"
%}

//%include "../include/Polygon4/UnrealTypes.h"

//%include "k:/AIM/DataManager/include/Polygon4/DataManager/Dll.h"
//%include "k:/AIM/DataManager/include/Polygon4/DataManager/String.h"
//%include "k:/AIM/DataManager/include/Polygon4/DataManager/Types.h"

/*namespace polygon4
{

class Game
{
public:
    //void OpenLevel(std::string level);
    //void SpawnPlayer(Vector v, Rotation r);
};

}*/


%{
#include "ScriptAPI.h"
%}

%include "ScriptAPI.h"
