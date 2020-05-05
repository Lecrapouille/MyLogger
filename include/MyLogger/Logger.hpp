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

#ifndef MYLOGGER_LOGGER_HPP
#  define MYLOGGER_LOGGER_HPP

#  include "Singleton.tpp"
#  include "IFileLogger.hpp"
#  include "File.hpp"

// *****************************************************************************
//! \brief Forward declaration concerning the project that will be stored in
//! the header of the logger.
// *****************************************************************************
namespace config
{
//! \brief Project compiled in release or debug mode ?
extern const bool debug;
//! \brief Either create a new log file or smash the older log.
extern const bool separated_logs;
//! \brief Used for logs and GUI.
extern const std::string project_name;
//! \brief Major version of project
extern const uint32_t major_version;
//! \brief Minor version of project
extern const uint32_t minor_version;
//! \brief Save the git SHA1
extern const std::string git_sha1;
//! \brief Save the git branch
extern const std::string git_branch;
//! \brief Pathes where default project resources have been installed
//! (when called  by the shell command: sudo make install).
extern const std::string data_path;
//! \brief Location for storing temporary files
extern const std::string tmp_path;
//! \brief Give a name to the default project log file.
extern const std::string log_name;
//! \brief Define the full path for the project.
extern const std::string log_path;
}

namespace tool { namespace log {

// *****************************************************************************
//! \brief File Logger service. Manage a single file.
// *****************************************************************************
class Logger: public IFileLogger, public Singleton<Logger>
{
    friend class Singleton<Logger>;

public:

    //! \brief
    Logger();

    //! \brief Open the file.
    Logger(std::string const& logfile);

    //! \brief Close the file.
    virtual ~Logger();

    //! \brief Reopen the log (old content is removed).
    bool changeLog(std::string const& logfile);

    //! \brief Log in the style of C++.
    ILogger& operator<<(const Severity& severity);

    //! \brief Log in the style of C++.
    ILogger& operator<<(const char *msg);

private:

    //! \brief Open the file.
    virtual bool open(std::string const& filename) override;

    //! \brief Close the file.
    virtual void close() override;

    //! \brief Write in the file.
    inline virtual void write(std::string const& message) override
    {
        write(message.c_str());
    }

    //! \brief Write in the file.
    virtual void write(const char *message, const int length = -1) override;

    //! \brief Write the header of the file.
    virtual void header() override;

    //! \brief Write the footer of the file.
    virtual void footer() override;

    //! \brief Format the begining of log lines.
    virtual void beginOfLine() override;

private:

    std::ofstream m_file;
};

#  define SHORT_FILENAME File::fileName(__FILE__).c_str()

//! \brief Log C++ like. Example:  CPP_LOG(tool::log::Fatal) << "test\n";
#  define CPP_LOG(severity, ...)                                        \
    tool::log::Logger::instance() << tool::log::Logger::instance().strtime(); \
    tool::log::Logger::instance() << severity << '[' << SHORT_FILENAME << "::" << __LINE__ << "] "

//! \brief Basic log without severity or file and line information. 'B' for Basic.
#  define LOGB_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::None, __VA_ARGS__); } while (0)
#  define LOGB(...) LOGB_HELPER(__VA_ARGS__, "")

//! \brief Information Log.
#  define LOGI_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Info, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGI(...) LOGI_HELPER(__VA_ARGS__, "")

//! \brief Debug Log.
#  define LOGD_HELPER(format, ...)                                      \
    do { if (config::debug) {                                           \
            tool::log::Logger::instance().log(nullptr, tool::log::Debug, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); \
        } } while (0)
#  define LOGD(...) LOGD_HELPER(__VA_ARGS__, "")

//! \brief Warning Log.
#  define LOGW_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Warning, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGW(...) LOGW_HELPER(__VA_ARGS__, "")

//! \brief Failure Log.
#  define LOGF_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Failed, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGF(...) LOGF_HELPER(__VA_ARGS__, "")

//! \brief Error Log.
#  define LOGE_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Error, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGE(...) LOGE_HELPER(__VA_ARGS__, "")

//! \brief Throw signal Log.
#  define LOGS_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Signal, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGS(...) LOGS_HELPER(__VA_ARGS__, "")

//! \brief Throw exception Log.
#  define LOGX_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Exception, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGX(...) LOGX_HELPER(__VA_ARGS__, "")

//! \brief Catch exception Log.
#  define LOGC_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Catch, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGC(...) LOGC_HELPER(__VA_ARGS__, "")

//! \brief Fatal Log.
#  define LOGA_HELPER(format, ...)                                      \
    do { tool::log::Logger::instance().log(nullptr, tool::log::Fatal, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGA(...) LOGA_HELPER(__VA_ARGS__, "")

#  define LOGIS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cout, tool::log::Info, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGIS(...) LOGIS_HELPER(__VA_ARGS__, "")

#  define LOGDS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cout, tool::log::Debug, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGDS(...) LOGDS_HELPER(__VA_ARGS__, "")

#  define LOGWS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cerr, tool::log::Warning, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGWS(...) LOGWS_HELPER(__VA_ARGS__, "")

#  define LOGFS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cerr, tool::log::Failed, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGFS(...) LOGFS_HELPER(__VA_ARGS__, "")

#  define LOGES_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cerr, tool::log::Error, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGES(...) LOGES_HELPER(__VA_ARGS__, "")

#  define LOGXS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cerr, tool::log::Exception, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGXS(...) LOGXS_HELPER(__VA_ARGS__, "")

#  define LOGCS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cerr, tool::log::Catch, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGCS(...) LOGCS_HELPER(__VA_ARGS__, "")

#  define LOGAS_HELPER(format, ...)                                     \
    do { tool::log::Logger::instance().log(&std::cerr, tool::log::Fatal, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGAS(...) LOGAS_HELPER(__VA_ARGS__, "")

} } // namespace tool::log

#endif /* MYLOGGER_LOGGER_HPP */
