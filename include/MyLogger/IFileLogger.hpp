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

#ifndef MYLOGGER_IFILELOGGER_HPP
#  define MYLOGGER_IFILELOGGER_HPP

#  include "MyLogger/ILogger.hpp"

namespace mylogger {

// *****************************************************************************
//!  \brief Interface class for file loggers.
// *****************************************************************************
class IFileLogger: public ILogger
{
private:

    //! \brief virtual method opening the media.
    virtual bool open(std::string const& filename) = 0;

    //! \brief virtual method closing the media.
    virtual void close() = 0;

    //! \brief virtual method doing the header of the file.
    virtual void header() = 0;

    //! \brief virtual method doing the footer of the file.
    virtual void footer() = 0;
};

} // namespace tool::log

#endif // MYLOGGER_IFILELOGGER_HPP
