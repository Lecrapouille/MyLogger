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

#  include "MyLogger/Singleton.tpp"
#  include "MyLogger/IFileLogger.hpp"
#  include "MyLogger/File.hpp"

#ifndef SINGLETON_FOR_LOGGER
#  define SINGLETON_FOR_LOGGER LongLifeSingleton<Logger>
#endif

namespace mylogger {
namespace project {

struct Info
{
    Info() = default;
    Info(bool const dbg, const char* name, uint32_t const major,
         uint32_t const minor, const char* sha1, const char* branch,
         const char* data, const char* tmp, const char* logname,
         const char* logpath)
        : debug(dbg),
          project_name(name),
          major_version(major),
          minor_version(minor),
          git_sha1(sha1),
          git_branch(branch),
          data_path(data),
          tmp_path(tmp),
          log_name(logname),
          log_path(logpath)
    {}

    //! \brief Compiled in debug or released mode
    bool debug;
    //! \brief Used for logs and GUI.
    std::string project_name;
    //! \brief Major version of project
    uint32_t major_version;
    //! \brief Minor version of project
    uint32_t minor_version;
    //! \brief Save the git SHA1
    std::string git_sha1;
    //! \brief Save the git branch
    std::string git_branch;
    //! \brief Pathes where default project resources have been installed
    //! (when called  by the shell command: sudo make install).
    std::string data_path;
    //! \brief Location for storing temporary files
    std::string tmp_path;
    //! \brief Give a name to the default project log file.
    std::string log_name;
    //! \brief Define the full path for the project.
    std::string log_path;
};

} // namespace project

// *****************************************************************************
//! \brief File Logger service. Manage a single file.
// *****************************************************************************
class Logger: public IFileLogger, public SINGLETON_FOR_LOGGER
{
    friend class SINGLETON_FOR_LOGGER;

public:

    Logger() = default;

    //! \brief Open the file.
    //! \param info structure holding all project information (name, version ...)
    Logger(mylogger::project::Info const& info);

    //! \brief Close the file.
    virtual ~Logger();

    //! \brief Reopen the log (old content is removed).
    bool changeLog(mylogger::project::Info const& info);
    bool changeLog(std::string const& filename);

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

    project::Info m_info;
    std::ofstream m_file;
};

// FIXME dans ::instance()
#  define SHORT_FILENAME mylogger::File::fileName(__FILE__).c_str()

#  define CONFIG_LOG(info) mylogger::Logger::instance().changeLog(info)

// FIXME a desactiver when NDEBUG
//! \brief Log C++ like. Example:  CPP_LOG(mylogger::Fatal) << "test\n";
#  define CPP_LOG(severity, ...)                                        \
    mylogger::Logger::instance() << mylogger::Logger::instance().strtime(); \
    mylogger::Logger::instance() << severity << '[' << SHORT_FILENAME << "::" << __LINE__ << "] "

//! \brief Basic log without severity or file and line information. 'B' for Basic.
#  define LOGB_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::None, __VA_ARGS__); } while (0)
#  define LOGB(...) LOGB_HELPER(__VA_ARGS__, "")

//! \brief Information Log.
#  define LOGI_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Info, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGI(...) LOGI_HELPER(__VA_ARGS__, "")

//! \brief Debug Log.
#  if defined(NDEBUG)
#    define LOGD(...) {}
#  else
#    define LOGD_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Debug, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#    define LOGD(...) LOGD_HELPER(__VA_ARGS__, "")
#  endif

//! \brief Warning Log.
#  define LOGW_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Warning, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGW(...) LOGW_HELPER(__VA_ARGS__, "")

//! \brief Failure Log.
#  define LOGF_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Failed, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGF(...) LOGF_HELPER(__VA_ARGS__, "")

//! \brief Error Log.
#  define LOGE_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Error, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGE(...) LOGE_HELPER(__VA_ARGS__, "")

//! \brief Throw signal Log.
#  define LOGS_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Signal, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGS(...) LOGS_HELPER(__VA_ARGS__, "")

//! \brief Throw exception Log.
#  define LOGX_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Exception, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGX(...) LOGX_HELPER(__VA_ARGS__, "")

//! \brief Catch exception Log.
#  define LOGC_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Catch, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGC(...) LOGC_HELPER(__VA_ARGS__, "")

//! \brief Fatal Log.
#  define LOGA_HELPER(format, ...)                                      \
    do { mylogger::Logger::instance().log(nullptr, mylogger::Fatal, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGA(...) LOGA_HELPER(__VA_ARGS__, "")

#  define LOGIS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cout, mylogger::Info, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGIS(...) LOGIS_HELPER(__VA_ARGS__, "")

#  define LOGDS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cout, mylogger::Debug, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGDS(...) LOGDS_HELPER(__VA_ARGS__, "")

#  define LOGWS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cerr, mylogger::Warning, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGWS(...) LOGWS_HELPER(__VA_ARGS__, "")

#  define LOGFS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cerr, mylogger::Failed, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGFS(...) LOGFS_HELPER(__VA_ARGS__, "")

#  define LOGES_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cerr, mylogger::Error, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGES(...) LOGES_HELPER(__VA_ARGS__, "")

#  define LOGXS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cerr, mylogger::Exception, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGXS(...) LOGXS_HELPER(__VA_ARGS__, "")

#  define LOGCS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cerr, mylogger::Catch, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGCS(...) LOGCS_HELPER(__VA_ARGS__, "")

#  define LOGAS_HELPER(format, ...)                                     \
    do { mylogger::Logger::instance().log(&std::cerr, mylogger::Fatal, "[%s::%d] " format, SHORT_FILENAME, __LINE__, __VA_ARGS__); } while (0)
#  define LOGAS(...) LOGAS_HELPER(__VA_ARGS__, "")

} // namespace mylogger

#endif /* MYLOGGER_LOGGER_HPP */
