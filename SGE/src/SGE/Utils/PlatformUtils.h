#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#pragma once

namespace SGE {
    class FileDialogs
    {
    public:
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}

#endif