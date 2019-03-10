// See LICENSE for license details.

#pragma once

class NSStubs
{
public:
    static std::string getTempDir();
    static std::string getHomeDir();
    static std::string getPublicDir();
    static std::string localizedString(std::string key);
};
