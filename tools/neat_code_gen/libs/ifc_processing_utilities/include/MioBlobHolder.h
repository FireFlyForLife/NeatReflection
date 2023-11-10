#pragma once

#include "ifc/Environment.h"
#include "mio/mmap.hpp"

#include <span>
#include <filesystem>
#include <memory>


class MioBlobHolder : public ifc::Environment::BlobHolder
{
public:
    MioBlobHolder(const std::filesystem::path& path)
        : memory_mapped_file(path.native())
    {}

    static ifc::Environment::BlobHolderPtr create_unique(const std::filesystem::path& path)
    {
        return std::make_unique<MioBlobHolder>(path);
    }

    ifc::File::BlobView view() const override
    {
        return std::as_bytes(std::span{ memory_mapped_file.data(), memory_mapped_file.size() });
    }

private:
    mio::mmap_source memory_mapped_file;
};
