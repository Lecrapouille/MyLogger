#include <MyLogger/Logger.hpp>
#include "config.hpp"

int main()
{
    LOGI("An information %s", "the info");
    LOGD("A debug %s", "the debug");
    LOGW("A warning %s", "the warning");
    LOGF("A failure %s", "the failure");

    return 0;
}
