#include "package_format.h"
#include "package_tool_version.h"
#include "file_util.h"
#include "stx_editor.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#endif

namespace {

void printUsage() {
    std::cout
        << dingoo::kPackageToolDisplayName << "\n"
        << "Usage:\n"
        << "  dingoo-package-tool --version\n"
        << "  dingoo-package-tool info <input.app|input.cc>\n"
        << "  dingoo-package-tool unpack <input.app|input.cc> <output-dir>\n"
        << "  dingoo-package-tool pack <manifest.json> <output.app|output.cc>\n"
        << "  dingoo-package-tool stx-info <input.stx>\n"
        << "  dingoo-package-tool stx-export <input.stx> <output.tsv>\n"
        << "  dingoo-package-tool stx-import <input.stx> <input.tsv> <output.stx>\n";
}

void printVersion() {
    std::cout << dingoo::kPackageToolDisplayName << "\n";
}

#ifdef _WIN32
std::vector<std::wstring> getWideArgs() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        throw std::runtime_error("failed to read Windows command line");
    }

    std::vector<std::wstring> args;
    args.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    LocalFree(argv);
    return args;
}
#endif

} // namespace

int main(int argc, char** argv) {
    try {
#ifdef _WIN32
        (void)argc;
        (void)argv;
        const auto args = getWideArgs();
        if (args.size() == 2 && args[1] == L"--version") {
            printVersion();
            return 0;
        }
        if (args.size() < 3) {
            printUsage();
            return 2;
        }

        const std::wstring command = args[1];
        if (command == L"info") {
            const std::filesystem::path inputPath(args[2]);
            const auto image = dingoo::parsePackageImage(
                dingoo::readFile(inputPath),
                dingoo::packageFormatFromPath(inputPath));
            std::cout << dingoo::describePackage(image);
            return 0;
        }

        if (command == L"unpack") {
            if (args.size() != 4) {
                printUsage();
                return 2;
            }
            dingoo::unpackPackage(std::filesystem::path(args[2]), std::filesystem::path(args[3]));
            std::cout << "unpacked: " << dingoo::pathToUtf8(std::filesystem::path(args[3])) << "\n";
            return 0;
        }

        if (command == L"pack") {
            if (args.size() != 4) {
                printUsage();
                return 2;
            }
            dingoo::packPackage(std::filesystem::path(args[2]), std::filesystem::path(args[3]));
            std::cout << "packed: " << dingoo::pathToUtf8(std::filesystem::path(args[3])) << "\n";
            return 0;
        }

        if (command == L"stx-info") {
            const auto entries = dingoo::scanStxText(dingoo::readFile(std::filesystem::path(args[2])));
            std::cout << dingoo::describeStxText(entries);
            return 0;
        }

        if (command == L"stx-export") {
            if (args.size() != 4) {
                printUsage();
                return 2;
            }
            dingoo::exportStxText(std::filesystem::path(args[2]), std::filesystem::path(args[3]));
            std::cout << "exported STX text: " << dingoo::pathToUtf8(std::filesystem::path(args[3])) << "\n";
            return 0;
        }

        if (command == L"stx-import") {
            if (args.size() != 5) {
                printUsage();
                return 2;
            }
            const auto edited = dingoo::importStxText(
                std::filesystem::path(args[2]),
                std::filesystem::path(args[3]),
                std::filesystem::path(args[4]));
            std::cout << "imported STX text edits: " << edited << "\n";
            std::cout << "written: " << dingoo::pathToUtf8(std::filesystem::path(args[4])) << "\n";
            return 0;
        }
#else
        if (argc == 2 && std::string(argv[1]) == "--version") {
            printVersion();
            return 0;
        }
        if (argc < 3) {
            printUsage();
            return 2;
        }

        const std::string command = argv[1];
        if (command == "info") {
            const std::filesystem::path inputPath(argv[2]);
            const auto image = dingoo::parsePackageImage(
                dingoo::readFile(inputPath),
                dingoo::packageFormatFromPath(inputPath));
            std::cout << dingoo::describePackage(image);
            return 0;
        }

        if (command == "unpack") {
            if (argc != 4) {
                printUsage();
                return 2;
            }
            dingoo::unpackPackage(argv[2], argv[3]);
            std::cout << "unpacked: " << argv[3] << "\n";
            return 0;
        }

        if (command == "pack") {
            if (argc != 4) {
                printUsage();
                return 2;
            }
            dingoo::packPackage(argv[2], argv[3]);
            std::cout << "packed: " << argv[3] << "\n";
            return 0;
        }

        if (command == "stx-info") {
            const auto entries = dingoo::scanStxText(dingoo::readFile(argv[2]));
            std::cout << dingoo::describeStxText(entries);
            return 0;
        }

        if (command == "stx-export") {
            if (argc != 4) {
                printUsage();
                return 2;
            }
            dingoo::exportStxText(argv[2], argv[3]);
            std::cout << "exported STX text: " << argv[3] << "\n";
            return 0;
        }

        if (command == "stx-import") {
            if (argc != 5) {
                printUsage();
                return 2;
            }
            const auto edited = dingoo::importStxText(argv[2], argv[3], argv[4]);
            std::cout << "imported STX text edits: " << edited << "\n";
            std::cout << "written: " << argv[4] << "\n";
            return 0;
        }
#endif

        printUsage();
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}
