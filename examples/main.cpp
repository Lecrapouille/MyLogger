#include <MyLogger/Logger.hpp>
#include <MyLogger/File.hpp>
#include "version.h"

namespace config
{
  //! \brief
  const Mode mode = config::Debug;
  //! \brief Either create a new log file or smash the older log.
  const bool separated_logs = false;
  //! \brief Used for logs and GUI.
  const std::string project_name("Example");
  //! \brief Major version of project
  const uint32_t major_version(PROJECT_MAJOR_VERSION);
  //! \brief Minor version of project
  const uint32_t minor_version(PROJECT_MINOR_VERSION);
  //! \brief Save the git SHA1
  const std::string git_sha1(PROJECT_SHA1);
  //! \brief Save the git branch
  const std::string git_branch(PROJECT_BRANCH);
  //! \brief Location for storing temporary files
  const std::string tmp_path(false == separated_logs ? PROJECT_TEMP_DIR :
                             File::generateTempFileName(PROJECT_TEMP_DIR, "/"));
  //! \brief Give a name to the default project log file.
  const std::string log_name(project_name + ".log");
  //! \brief Define the full path for the project.
  const std::string log_path(tmp_path + log_name);
}

int main()
{
    LOGI("An information %s", "the info");
    LOGD("A debug %s", "the debug");
    LOGW("A warning %s", "the warning");
    LOGF("A failure %s", "the failure");

    return 0;
}
