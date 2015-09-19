#include <fstream>
#include <iostream>

using namespace std;

string user = "\
<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\
<Project ToolsVersion=\"14.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n\
    <PropertyGroup>\n\
        <TargetName>%s</TargetName>\n\
    </PropertyGroup>\n\
    <ItemDefinitionGroup>\n\
        <Link>\n\
            <ImportLibrary>%s</ImportLibrary>\n\
            <ProgramDataBaseFile>%s</ProgramDataBaseFile>\n\
        </Link>\n\
    </ItemDefinitionGroup>\n\
</Project>\n\
";

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s file.project file_without_ext addition_part", argv[0]);
        return 1;
    }

    string base = argv[3];
    string lib = base + ".lib";
    string pdb = base + ".pdb";

    char buf[8192] = { 0 };
    sprintf(buf, user.c_str(), argv[2], lib.c_str(), pdb.c_str());

    ofstream ofile(argv[1]);
    if (!ofile)
    {
        printf("Cannot open the output file: %s", argv[1]);
        return 1;
    }
    ofile << buf;

    return 0;
}

