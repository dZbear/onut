﻿#include <algorithm>
#include <codecvt>
#include <cassert>
#include <sstream>
#include "dirent.h"
#include "StringUtils.h"

namespace onut
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> g_stringConverter;

    std::wstring utf8ToUtf16(const std::string& utf8)
    {
        return g_stringConverter.from_bytes(utf8);
    }

    std::string utf16ToUtf8(const std::wstring& utf16)
    {
        return g_stringConverter.to_bytes(utf16);
    }

    std::vector<std::string> splitString(const std::string& in_string, char in_delimiter)
    {
        std::vector<std::string> elems;
        unsigned int start = 0;
        unsigned int end = 0;
        for (; end < in_string.length(); ++end)
        {
            if (in_string[end] == in_delimiter)
            {
                if (end - start)
                {
                    elems.push_back(in_string.substr(start, end - start));
                }
                start = end + 1;
            }
        }
        if (start < in_string.length())
        {
            if (end - start)
            {
                elems.push_back(in_string.substr(start, end - start));
            }
        }
        return elems;
    }

    int hash(const char* pStr)
    {
        int hash = 0;
        static const int offset = 'a' - 1;
        while (pStr)
        {
            hash = hash << 1 | (*pStr - offset);
            ++pStr;
        }
        return hash;
    }

    template<>
    std::string findFile<true>(const std::string& name, const std::string& lookIn, bool deepSearch)
    {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(lookIn.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (!strcmp(ent->d_name, "."))
                {
                    continue;
                }
                else if (!strcmp(ent->d_name, ".."))
                {
                    continue;
                }

                if (name == ent->d_name)
                {
                    auto ret = lookIn + "/" + ent->d_name;
                    closedir(dir);
                    return ret;
                }

                if (ent->d_type & DT_DIR && deepSearch)
                {
                    auto ret = findFile<true>(name, lookIn + "/" + ent->d_name);
                    if (!ret.empty())
                    {
                        closedir(dir);
                        return ret;
                    }
                }
            }
            closedir(dir);
        }
        else
        {
            assert(false);
        }

        return "";
    }

    template<>
    std::string findFile<false>(const std::string& in_name, const std::string& in_lookIn, bool deepSearch)
    {
        DIR *dir;
        struct dirent *ent;

        auto name = in_name;
        auto lookIn = in_lookIn;
        auto pos = in_name.find_last_of("\\/");
        if (pos != std::string::npos)
        {
            name = in_name.substr(pos + 1);
            lookIn = in_lookIn + "/" + in_name.substr(0, in_name.find_last_of("\\/"));
        }

        if ((dir = opendir(lookIn.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (!strcmp(ent->d_name, "."))
                {
                    continue;
                }
                else if (!strcmp(ent->d_name, ".."))
                {
                    continue;
                }

                if (!_stricmp(name.c_str(), ent->d_name))
                {
                    auto ret = lookIn + "/" + ent->d_name;
                    closedir(dir);
                    return ret;
                }

                if (ent->d_type & DT_DIR && deepSearch)
                {
                    auto ret = findFile<false>(name, lookIn + "/" + ent->d_name);
                    if (!ret.empty())
                    {
                        closedir(dir);
                        return ret;
                    }
                }
            }
            closedir(dir);
        }

        return "";
    }

    std::string getPath(const std::string& filename)
    {
        return filename.substr(0, filename.find_last_of("\\/"));
    }

    std::string makeRelativePath(const std::string& in_path, const std::string& in_relativeTo)
    {
        auto path = in_path;
        std::replace(path.begin(), path.end(), '\\', '/');
        auto pathSplit = splitString(path, '/');

        auto relativeTo = in_relativeTo;
        std::replace(relativeTo.begin(), relativeTo.end(), '\\', '/');
        auto relativeSplit = splitString(relativeTo, '/');

        while (pathSplit.size() && relativeSplit.size() && pathSplit.front() == relativeSplit.front())
        {
            pathSplit.erase(pathSplit.begin());
            relativeSplit.erase(relativeSplit.begin());
        }

        std::stringstream ss;
        bool bFirst = true;
        for (auto& folder : relativeSplit)
        {
            if (!bFirst) ss << "/";
            bFirst = false;
            ss << "..";
        }
        for (auto& folder : pathSplit)
        {
            if (!bFirst) ss << "/";
            bFirst = false;
            ss << folder;
        }
        return std::move(ss.str());
    }
}
