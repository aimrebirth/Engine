void build(Solution &s)
{
    auto &Engine = s.addLibrary("Polygon4.Engine", "master");
    //auto &Engine = s.addSharedLibrary("Polygon4.Engine", "master");

    auto &logger = Engine.addStaticLibrary("logger");
    {
        logger.CPPVersion = CPPLanguageStandard::CPP17;
        logger.setRootDirectory("src/tools");
        logger += "Logger.*"_rr;
        logger.Public += "org.sw.demo.boost.log"_dep;
        logger.Public += "USE_LOGGER"_def;
    }

    {
        Engine.ApiName = "P4_ENGINE_API";
        Engine.CPPVersion = CPPLanguageStandard::CPP17;
        Engine += "include/Polygon4/.*"_rr;
        Engine += "src/.*"_r;
        Engine += "src/tools/Hotpatch.*"_rr;

        auto d = Engine.Public + "pub.lzwdgc.polygon4.datamanager.memory-master"_dep;
        //d->getOptions()["alligned-allocator"] = "1";
        //d->getOptions()["alligned-allocator"].setRequired();

        Engine.Public += logger,
            "pub.egorpugin.primitives.executor-master"_dep,
            "pub.egorpugin.primitives.command-master"_dep,
            "pub.lzwdgc.polygon4.datamanager-master"_dep,
            "org.sw.demo.lua"_dep
            ;
        Engine += "dbghelp.lib"_slib;

        {
            String prefix = "ScriptAPI";
            auto c = Engine.addCommand();
            c << cmd::prog("org.sw.demo.swig"_dep)
                << "-c++"
                << "-lua"
                << "-o"
                << cmd::out(prefix + "_lua.cpp", cmd::Skip)
                << cmd::in("src/" + prefix + ".i")
                ;
        }
    }

    auto &pdbfix = Engine.addExecutable("tools.pdbfix");
    pdbfix += "src/tools/PdbFix.cpp";
    pdbfix += "dbghelp.lib"_slib;
    pdbfix += "pub.egorpugin.primitives.filesystem-master"_dep;

    auto &fixproject = Engine.addExecutable("tools.fixproject");
    fixproject += "src/tools/FixProject.cpp";

    auto &prepare_sw_info = Engine.addExecutable("tools.prepare_sw_info", "0.0.1");
    {
        prepare_sw_info += cpp17;
        prepare_sw_info += "src/tools/prepare_sw_info.cpp";
        prepare_sw_info += "pub.egorpugin.primitives.sw.main-master"_dep;
        prepare_sw_info += "org.sw.demo.nlohmann.json"_dep;
    }
}
