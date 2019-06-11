#pragma sw require pub.egorpugin.primitives.filesystem-master
#pragma sw require org.sw.sw.client.driver.cpp-*

void configure(Build &b)
{
    auto ss = b.createSettings();
    ss.Native.LibrariesType = LibraryType::Static;
    ss.Native.ConfigurationType = ConfigurationType::ReleaseWithDebugInformation;
    b.addSettings(ss);

    b.registerCallback([](auto &t)
    {
        auto &nt = dynamic_cast<NativeExecutedTarget&>(t);
        nt += "DATA_MANAGER_ALIGNED_ALLOCATOR"_def;
    }, "pub.lzwdgc.polygon4.datamanager.memory-master", sw::CallbackType::BeginPrepare);

    b.registerCallback([&b](auto &t, auto cbt)
    {
        if (cbt == sw::CallbackType::CreateTarget)
        {
            if (t.getPackage() == PackageId{ "pub.lzwdgc.polygon4.datamanager.schema-master" } ||
                t.getPackage() == PackageId{ "pub.lzwdgc.polygon4.datamanager-master" } ||
                t.getPackage() == PackageId{ "Polygon4.Engine-master" })
            {
                //t.Settings.Native.LibrariesType = LibraryType::Shared;
            }
        }
        else if (cbt == sw::CallbackType::EndPrepare)
        {
            auto &nt = dynamic_cast<NativeExecutedTarget&>(t);

            if (t.getPackage() == PackageId{ "Polygon4.Engine-master" })
            {
                String str;
                for (auto &i : nt.gatherIncludeDirectories())
                    str += i.u8string() + "\n";
                write_file(b.BinaryDir / ("includes_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), str);
            }

            if (t.getPackage() == PackageId{ "Polygon4.Engine-master" })
            {
                String str;
                for (auto &[k, v] : nt.Definitions)
                {
                    if (v.empty())
                        str += k;
                    else
                    {
                        str += k + "=";
                        str += v; // msvc bug workaround
                    }
                    str += "\n";
                }
                write_file(b.BinaryDir / ("definitions_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), str);
            }

            if (t.getPackage() == PackageId{ "Polygon4.Engine-master" })
            {
                String str;
                for (auto &l : nt.LinkLibraries)
                {
                    str += l.string();
                    str += "\n";
                }
                write_file(b.BinaryDir / ("link_libraries_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), str);
            }

            if (t.getPackage() == PackageId{ "Polygon4.Engine-master" })
            {
                auto tgt = nt.SourceDir.parent_path().parent_path() / "Binaries" / "Win64";
                if (1
                    && nt.SourceDir.filename() == "Engine"
                    && nt.SourceDir.parent_path().filename() == "ThirdParty"
                    && fs::exists(tgt)
                    )
                {
                    auto in = nt.getOutputFile();
                    auto out = tgt / in.filename();

                    SW_MAKE_EXECUTE_BUILTIN_COMMAND_AND_ADD(copy_cmd, nt, "sw_copy_file", nullptr);
                    copy_cmd->push_back(in.u8string());
                    copy_cmd->push_back(out.u8string());
                    copy_cmd->addInput(in);
                    copy_cmd->addOutput(out);
                    copy_cmd->name = "copy: " + normalize_path(out);
                    copy_cmd->maybe_unused = builder::Command::MU_ALWAYS;
                    copy_cmd->command_storage = builder::Command::CS_LOCAL;
                    copy_cmd->dependencies.insert(nt.getCommand());
                    nt.getCommand()->dependent_commands.insert(copy_cmd);
                }

                write_file(b.BinaryDir / ("engine_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), nt.getImportLibrary().u8string());
            }

            if (t.getPackage() == PackageId{ "pub.lzwdgc.polygon4.datamanager.schema-master" })
                write_file(b.BinaryDir / ("schema_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), nt.getImportLibrary().u8string());
            if (t.getPackage() == PackageId{ "pub.lzwdgc.polygon4.datamanager-master" })
                write_file(b.BinaryDir / ("data_manager_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), nt.getImportLibrary().u8string());
            if (t.getPackage().ppath == "org.sw.demo.sqlite3")
                write_file(b.BinaryDir / ("sqlite3_" + toString(nt.getSettings().Native.ConfigurationType) + ".txt"), nt.getImportLibrary().u8string());
        }
    });
}

void build(Solution &s)
{
    auto &Engine = s.addSharedLibrary("Polygon4.Engine", "master");

    auto &logger = Engine.addStaticLibrary("logger");
    logger.CPPVersion = CPPLanguageStandard::CPP17;
    logger.setRootDirectory("src/tools");
    logger += "Logger.*"_rr;
    logger.Public += "org.sw.demo.boost.log-1"_dep;
    logger.Public += "USE_LOGGER"_def;

    Engine.ApiName = "P4_ENGINE_API";
    Engine.CPPVersion = CPPLanguageStandard::CPP17;
    Engine += "include/Polygon4/.*"_rr;
    Engine += "src/.*"_r;
    Engine += "src/tools/Hotpatch.*"_rr;

    Engine.Public += logger,
        "pub.egorpugin.primitives.executor-master"_dep,
        "pub.egorpugin.primitives.command-master"_dep,
        "pub.lzwdgc.polygon4.datamanager-master"_dep,
        "org.sw.demo.lua-5"_dep
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

    auto &pdbfix = Engine.addExecutable("tools.pdbfix");
    pdbfix += "src/Tools/PdbFix.cpp";
    pdbfix += "dbghelp.lib"_slib;
    pdbfix += "pub.egorpugin.primitives.filesystem-master"_dep;

    auto &fixproject = Engine.addExecutable("tools.fixproject");
    fixproject += "src/Tools/FixProject.cpp";
}
