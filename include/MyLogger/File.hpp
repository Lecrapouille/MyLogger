//=====================================================================
// MyLogger: A basic logger.
// Copyright 2018 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of MyLogger.
//
// MyLogger is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef MYLOGGER_FILE_HPP
#  define MYLOGGER_FILE_HPP

#  include <string.h>
#  include <iostream>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <time.h>
#  include <algorithm>

// **************************************************************
// Architecture specific
// **************************************************************
#  ifdef _WIN32
#    include <direct.h>
#    define MKDIR(p, m) (void) m, ::_mkdir(p)
#    define DIR_SEP '\\'
#  else
#    define MKDIR(p, m) ::mkdir(p, m)
#    define DIR_SEP '/'
#  endif

namespace mylogger {

// **************************************************************
//! \brief Utility of file name manipulation
// **************************************************************
class File
{
public:

    //--------------------------------------------------------------------------
    //! \brief give the file name with its extension from a given path
    //--------------------------------------------------------------------------
    inline static std::string fileName(std::string const& path)
    {
        std::string::size_type pos = path.find_last_of("\\/");
        if (pos != std::string::npos)
            return path.substr(pos + 1, std::string::npos);
        return path;
    }

    //--------------------------------------------------------------------------
    //! \brief Get the directory name of a path.
    //!
    //! \note the path is considered refering to a file not to a directory
    //! except if a '/' or '\\' is set at the end of the name.
    //!
    //! Examples:
    //!  - dirName("/tmp/") will return dirName("/tmp/")
    //!  - but dirName("/tmp") will return dirName("/")
    //--------------------------------------------------------------------------
    inline static std::string dirName(std::string const& path)
    {
        std::string::size_type pos = path.find_last_of("\\/");
        if (pos == path.length() - 1)
        {
            return path;
        }
        if (pos != std::string::npos)
        {
            return path.substr(0, pos + 1);
        }
        return "";
    }

    //--------------------------------------------------------------------------
    //! \brief Create a path of directory.
    //! \param last element of path shall not be considerate as a file but as a
    //! directory.
    //--------------------------------------------------------------------------
    static bool mkdir(std::string const& path, mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO)
    {
        struct stat st;
        std::string::const_iterator begin = path.begin();
        std::string::const_iterator end = path.end();
        std::string::const_iterator iter = begin;

        while (iter != end)
        {
            std::string::const_iterator new_iter = std::find(iter, end, DIR_SEP);
            std::string new_path = DIR_SEP + std::string(begin, new_iter);

            if (stat(new_path.c_str(), &st) != 0)
            {
                if ((MKDIR(new_path.c_str(), mode) != 0) && (errno != EEXIST))
                {
                    std::cout << "cannot create folder [" << new_path << "] : "
                              << strerror(errno) << std::endl;
                    return false;
                }
            }
            else if (!(st.st_mode & S_IFDIR))
            {
                errno = ENOTDIR;
                std:: cout << "path [" << new_path << "] not a dir " << std::endl;
                return false;
            }
            else
            {
                //std::cout << "path [" << new_path << "] already exists " << std::endl;
            }

            iter = new_iter;
            if (new_iter != path.end())
            {
                ++iter;
            }
        }
        return true;
    }
};

} // namespace namespace mylogger

#endif // MYLOGGER_FILE_HPP
