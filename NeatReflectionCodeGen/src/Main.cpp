#include "CodeGenerator.h"

#include <ContextualException.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <span>

#include "docopt.h"
#include "mio/mmap.hpp"
#include "ifc/File.h"
#include "ifc/Environment.h"
//#include "ifc/MSVCEnvironment.h"


constexpr auto USAGE = R"(Usage: 
    NeatReflectionCodeGen.exe <in_ifc_file> <out_cpp_file>
    NeatReflectionCodeGen.exe scan <in_dir> <out_dir>)";

//class MioBlobHolder : public ifc::Environment::BlobHolder
//{
//public:
//    MioBlobHolder(const std::filesystem::path& path)
//        : memory_mapped_file(path)
//    {}
//
//    static ifc::Environment::BlobHolderPtr create_unique(const std::filesystem::path& path)
//    {
//        return std::make_unique<MioBlobHolder>(path);
//    }
//
//    ifc::File::BlobView view() const override
//    {
//        return std::as_bytes(std::span{ memory_mapped_file.data(), memory_mapped_file.size() });
//    }
//
//private:
//    mio::mmap_source memory_mapped_file;
//};

bool convert_ifc_file(const std::string& ifc_filename, const std::string& cpp_filename) try
{
    ContextArea filename_context{ std::format("While loading ifc file: '{0}'.\nAnd preparing to output to: '{1}'", ifc_filename, cpp_filename) };

    if (!std::filesystem::exists(ifc_filename))
    {
        std::cout << "ERROR: in_ifc_file: '" << ifc_filename << "' does not exist.";
        return false;
    }
    if (!ifc_filename.ends_with(".ifc"))
    {
        std::cout << "ERROR: in_ifc_file: '" << ifc_filename << "' is not a .ifc file. It's extension is: " << std::filesystem::path{ ifc_filename }.extension() << '\n';
        return false;
    }

    mio::mmap_source mmapped_file{ ifc_filename };
    auto file_bytes = std::as_bytes(std::span{ mmapped_file.data(), mmapped_file.size() });

    //ifc::Environment environment{ ifc::read_msvc_config(ifc_filename + ".d.json"), &MioBlobHolder::create_unique };
    ifc::File ifc_file{ file_bytes };

    std::ofstream file_stream{ cpp_filename, std::ofstream::out | std::ofstream::trunc };
    if (!file_stream.good())
    {
        std::cout << "ERROR: Could not open output file '" << cpp_filename << "' for writing. Reason: " << strerror(errno) << '\n';
        return false;
    }
    if (!cpp_filename.ends_with(".cpp"))
    {
        std::cout << "ERROR: out_cpp_file: '" << cpp_filename << "' is not a .cpp file. It's extension is: " << std::filesystem::path{ cpp_filename }.extension() << '\n';
        return false;
    }

    CodeGenerator code_generator{ ifc_file };
    code_generator.write_cpp_file(file_stream);

    return true;
}
catch (...)
{
    return false;
}

int main(int argc, char const *argv[])
{
    std::cout << "Running NeatReflectionCodeGen!\n";

    const std::vector<std::string> arguments{ argv + 1, argv + argc };
    docopt::Options parsed = docopt::docopt(USAGE, arguments, true, "0.1");

    if (parsed["scan"].asBool())
    {
        const std::filesystem::path target_dir = parsed["<in_dir>"].asString();
        const std::filesystem::path output_dir = parsed["<out_dir>"].asString();

        for (auto entry : std::filesystem::directory_iterator{ target_dir })
        {
            if (entry.is_regular_file() && entry.path().extension() == ".ifc")
            {
                auto output_filename = std::filesystem::absolute(output_dir / entry.path().filename().replace_extension("cpp"));
                
                std::cout << "Converting '" << std::filesystem::absolute(entry) << "' to '" << output_filename << "'\n";

                if (!convert_ifc_file( entry.path().string(), output_filename.string()))
                {
                    std::cout << "ERROR: Failed to convert '" << entry << "'\n";
                    return 1;
                }
            }
        }
    }
    else 
    {
        const std::string ifc = parsed["<in_ifc_file>"].asString();
        const std::string cpp = parsed["<out_cpp_file>"].asString();

        if (!convert_ifc_file(ifc, cpp))
        {
            return 1;
        }
    }

    return 0;
}