// 2015-01-30T09:48+08:00

#include "exit_command.h"

bool ExitCommand::parse(const std::vector<utility::string_t> &/*vargs*/)
{
    return true;
}

bool ExitCommand::run(azure::storage::cloud_storage_account &/*storage_account*/)
{
    return false;
}
