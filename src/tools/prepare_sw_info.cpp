#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

#include <primitives/sw/main.h>
#include <primitives/sw/cl.h>

template <class T>
struct UniqueVector : std::vector<T>
{
    using base = std::vector<T>;

    void push_back(const T &v)
    {
        if (unique.find(v) != unique.end())
            return;
        base::push_back(v);
        unique.insert(v);
    }

private:
    std::set<T> unique;
};

int main(int argc, char *argv[])
{
    cl::opt<path> output_dir(cl::Positional, cl::Required);
    cl::opt<path> input_json(cl::Positional, cl::Required);
    cl::opt<path> sw_root(cl::Positional, cl::Required);
    cl::opt<String> main_target(cl::Positional, cl::Required);

    cl::ParseCommandLineOptions(argc, argv);

    UniqueVector<String> defs;
    UniqueVector<String> idirs;
    UniqueVector<String> libs;
    UniqueVector<String> deps;

    auto j = nlohmann::json::parse(read_file(input_json));

    auto prepare_path = [&sw_root](const path &p)
    {
        if (p.is_absolute())
            return to_string(to_path_string(normalize_path(p)));
        return to_string(to_path_string(normalize_path(sw_root / p)));
    };

    std::set<nlohmann::json> visited;

    auto process_deps = [&](this auto &&f, auto &&j1) -> void
    {
        for (auto &[k, v] : j1.items())
        {
            if ((std::stoi(k) & 4) == 0)
                continue;
            auto &j2 = v;
            for (auto &[k, v] : j2["definitions"].items())
            {
                if (v.get<String>().empty())
                    defs.push_back(k);
                else
                    defs.push_back(k + "=" + v.get<String>());
            }
            for (auto &v : j2["include_directories"])
                idirs.push_back(prepare_path(v.get<String>()));
            for (auto &v : j2["link_libraries"]) {
                libs.push_back(prepare_path(v.get<String>()));
                deps.push_back(path{prepare_path(v.get<String>())}.stem().string());
            }
            for (auto &v : j2["system_link_libraries"])
                libs.push_back(v.get<String>());
            for (auto &droot : j2["dependencies"])
            {
                for (auto [k, v] : droot.items())
                {
                    for (auto &v2 : j["build"][k])
                    {
                        auto kk = v2["key"];
                        if (visited.contains(k)) {
                            continue;
                        }
                        if (kk == v)
                        {
                            visited.insert(k);
                            //std::cerr << "processing " << k << "\n";
                            f(v2["value"]["properties"]);
                            break;
                        }

                        v.erase("include_directories_only");
                        v.erase("include_directories_only_used_in_hash");
                        if (visited.contains(k)) {
                            continue;
                        }
                        if (kk == v)
                        {
                            visited.insert(k);
                            //std::cerr << "processing " << k << "\n";
                            f(v2["value"]["properties"]);
                            break;
                        }
                    }
                }
            }
        }
    };
    process_deps(j["build"][boost::to_lower_copy(String(main_target))][0]["value"]["properties"]);

    auto write_var = [&output_dir](const String &name, const auto &a)
    {
        String s;
        for (auto &v : a)
            s += v + "\n";
        write_file(output_dir / name += ".txt", s);
    };
    write_var("defs", defs);
    write_var("idirs", idirs);
    write_var("libs", libs);
    write_var("deps", deps);

    return 0;
}
