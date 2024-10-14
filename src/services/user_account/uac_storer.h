#ifndef _UAC_STORER_H
#define _UAC_STORER_H

#include <string>
#include <vector>
#include <memory>  // for std::shared_ptr

using std::string;
using std::vector;

class sqlite3;
class UserBaseInfo;
using UserBaseInfoPtr = std::shared_ptr<UserBaseInfo>;

class UACStorer {
public:
    UACStorer(const char* db_uri)
        : db_uri_(db_uri), db_(nullptr)
    {
        OpenDatabase();
    }
    ~UACStorer()
    {
        CloseDatabase();
        db_ = nullptr;
    }
    int OpenDatabase();
    void CloseDatabase();

    string ListAllUserBaseInfo(int indent=-1) const;

    UserBaseInfoPtr GetUserBaseInfo(uint32_t user_id) const;
    std::vector<UserBaseInfoPtr> GetAllUserBaseInfo(uint32_t user_id=0) const;

private:
    string db_uri_;
    sqlite3 *db_;
};

#endif  // _UAC_STORER_H
