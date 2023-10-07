#include "CodeGenerator.h"

#include <ContextualException.h>

#include "MioBlobHolder.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <span>

#include "ifc/File.h"
#include "ifc/Environment.h"
#include "ifc/MSVCEnvironment.h"
#include "reflifc/Module.h"


bool convert_ifc_file(const std::filesystem::path& ifc_filename, const std::filesystem::path& cpp_filename) try
{
    ContextArea filename_context{ std::format("While loading ifc file: '{0}'. And preparing to output to: '{1}'", ifc_filename.string(), cpp_filename.string()) };

    MioBlobHolder ifc_file_blob{ ifc_filename };
    ifc::File ifc_file{ ifc_file_blob.view() };

    ifc::Environment environment{ ifc::read_msvc_config(ifc_filename.string() + ".d.json"), &MioBlobHolder::create_unique };
   
    std::ofstream file_stream{ cpp_filename, std::ofstream::out | std::ofstream::trunc };
    if (!file_stream.good())
    {
        std::cout << "ERROR: Could not open output file '" << cpp_filename << "' for writing. Reason: " << strerror(errno) << '\n';
        return false;
    }

    CodeGenerator code_generator{ ifc_file, environment };
    code_generator.write_cpp_file(reflifc::Module{&ifc_file}, file_stream);

    return true;
}
catch (ContextualException& e)
{
    std::cerr << "Error occured running NeatReflectionCodeGen, more info: \n\n" << e.what() << '\n';
    return false;
}
catch (...)
{
    std::cerr << "Unknown exception has been raised from NeatReflectionCodeGen, exiting.\n";
    return false;
}

struct CodeGenArgs
{
    std::filesystem::path input_ifc_path;
    std::filesystem::path output_cpp_path;
};

bool parse_command_line_args(std::span<const char*> in_args, CodeGenArgs& out_parsed_args)
{
    constexpr auto USAGE = R"(Usage: 
    NeatReflectionCodeGen.exe <in_ifc_file> <out_cpp_file>)";

    if (in_args.size() < 3) {
        std::cout << USAGE << '\n';
        return false;
    }

    out_parsed_args.input_ifc_path = in_args[1];
    if (!std::filesystem::exists(out_parsed_args.input_ifc_path)) {
        std::cout << "ERROR: input ifc file: '" << out_parsed_args.input_ifc_path << "' does not exist.";
        return false;
    }
    if (out_parsed_args.input_ifc_path.extension() != ".ifc") {
        std::cout << "ERROR: in_ifc_file: '" << out_parsed_args.input_ifc_path 
            << "' is not a .ifc file. It's extension is: " << out_parsed_args.input_ifc_path.extension() << '\n';
        return false;
    }

    out_parsed_args.output_cpp_path = in_args[2];
    if (out_parsed_args.output_cpp_path.extension() != ".cpp") {
        std::cout << "ERROR: out_cpp_file: '" << out_parsed_args.output_cpp_path 
            << "' is not a .cpp file. It's extension is: " << out_parsed_args.output_cpp_path.extension() << '\n';
        return false;
    }

    return true;
}

int main(int argc, const char* argv[])
{
    std::cout << "Running NeatReflectionCodeGen!\n";

    CodeGenArgs code_gen_args{};
    if(!parse_command_line_args({ argv, (size_t)argc }, code_gen_args)) {
        return 1;
    }

    if (!convert_ifc_file(code_gen_args.input_ifc_path, code_gen_args.output_cpp_path))
    {
        return 1;
    }

    return 0;
}