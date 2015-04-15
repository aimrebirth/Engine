#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s file.project file_without_ext addition_part", argv[0]);
        return 1;
    }
    
    ifstream ifile(argv[1]);
    if (!ifile)
    {
        printf("Cannot open the input file: %s", argv[1]);
        return 1;
    }

    string target = "TargetName";
    string lib = "ImportLibrary";
    string pdb = "ProgramDataBaseFile";
    string out;
    while (ifile)
    {
        string s;
        getline(ifile, s);

        if (s.find("<" + target + ">") != string::npos)
        {
            out += "<" + target + ">";
            out += argv[2];
            out += "</" + target + ">";
        }
        else if (s.find("<" + lib + ">") != string::npos)
        {
            out += "<" + lib + ">";
            out += argv[3] + string(".lib");
            out += "</" + lib + ">";
        }
        else if (s.find("<" + pdb + ">") != string::npos)
        {
            out += "<" + pdb + ">";
            out += argv[3] + string(".pdb");
            out += "</" + pdb + ">";
        }
        else
        {
            out += s;
        }
        out += "\n";
    }
    ifile.close();

    ofstream ofile(argv[1]);
    if (!ofile)
    {
        printf("Cannot open the output file: %s", argv[1]);
        return 1;
    }
    ofile << out;

    return 0;
}

