#pragma once

#include "package_format.h"

#include <string>

namespace dingoo {

struct Manifest {
    PackageImage image;
    std::string originalImagePath;
    std::string rawPayloadPath;
};

std::string writeManifest(const PackageImage& image, const std::string& originalImagePath, const std::string& rawPayloadPath);
Manifest readManifest(const std::string& text);

} // namespace dingoo
