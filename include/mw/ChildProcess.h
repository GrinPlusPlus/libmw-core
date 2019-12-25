#pragma once

#include <mw/exceptions/ProcessException.h>
#include <reproc++/reproc.hpp>

#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <array>

class ChildProcess
{
public:
    using CPtr = std::shared_ptr<const ChildProcess>;

    static ChildProcess::CPtr Create(const std::vector<std::string>& args)
    {
        assert(!args.empty());

        std::shared_ptr<ChildProcess> pChildProcess = std::shared_ptr<ChildProcess>(new ChildProcess());

        reproc::stop_actions stop_actions = {
            { reproc::stop::terminate, reproc::milliseconds(5000) },
            { reproc::stop::kill, reproc::milliseconds(2000) },
            {}
        };

        reproc::options options;
        options.stop_actions = stop_actions;

        std::error_code ec = pChildProcess->m_process.start(args, options);

        if (ec == std::errc::no_such_file_or_directory) {
            // std::cerr << "Program not found. Make sure it's available from the PATH.";
            return nullptr;
        } else if (ec) {
            throw ProcessEx_F("Error occurred while starting process {}", args[0]);
        }

        return pChildProcess;
    }

    ~ChildProcess()
    {
        reproc::stop_actions stop_actions = {
            { reproc::stop::terminate, reproc::milliseconds(5000) },
            { reproc::stop::kill, reproc::milliseconds(2000) },
            {}
        };
        m_process.stop(stop_actions);
    }

private:
    ChildProcess() = default;

    reproc::process m_process;
};