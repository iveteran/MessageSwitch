#include "uac_storer.h"
#include "uac_messages.h"
#include <sqlite3.h>
#include <sstream>

static int db_result_callback(void *data, int argc, char **argv, char **columns) {
    auto results = (std::vector<UserBaseInfoPtr>*)data;

    /*
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", columns[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    */

    int i = 0;
    auto ubi = new UserBaseInfo{
        uint32_t(std::stoi(argv[i++])),     // uid
        argv[i++],                          // name
        std::stoi(argv[i++]),               // age
        std::stoi(argv[i++]),               // gender, 1: male, 0: female
        time_t(std::stoi(argv[i++])),       // creation time, unix timestamp
        argv[i++],                          // lang
        argv[i++],                          // country
        argv[i++]                           // address
    };
    results->push_back(std::shared_ptr<UserBaseInfo>(ubi));

    return 0;
}

int UACStorer::OpenDatabase() {
    int rc = sqlite3_open(db_uri_.c_str(), &db_);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_));
        return -1;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    return 0;
}

void UACStorer::CloseDatabase() {
    sqlite3_close(db_);
}

string UACStorer::ListAllUserBaseInfo(int indent) const {
    std::stringstream ss;
    ss << "[\n";
    auto user_bi_list = GetAllUserBaseInfo();
    for (auto user_bi : user_bi_list) {
        ss << user_bi->EncodeToJSON() << ",\n";
    }
    ss << "]";
    return ss.str();
}

UserBaseInfoPtr
UACStorer::GetUserBaseInfo(uint32_t user_id) const {
    auto results = GetAllUserBaseInfo(user_id);
    if (user_id > 0 and ! results.empty()) {
        return results[0];
    } else {
        return nullptr;
    }
}

std::vector<UserBaseInfoPtr>
UACStorer::GetAllUserBaseInfo(uint32_t user_id) const {
    char *errmsg = nullptr;

    const char* sql_tmpl = "SELECT * FROM t_user_base_info WHERE uid=%d OR 0=%d";
    char sql[128];
    snprintf(sql, sizeof(sql), sql_tmpl, user_id, user_id);

    std::vector<UserBaseInfoPtr> results;
    int rc = sqlite3_exec(db_, sql, db_result_callback, (void*)(&results), &errmsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }

    /*
    uint32_t uid = 1;
    string name = "Yuu";
    int age = 18;
    int gender = 1;
    string lang = "zh_cn";
    time_t creation_time = 1728881091;
    string country = "CN";
    string address = "ShenZhen";
    auto ubi = new UserBaseInfo{
        uid,
        name,
        age,
        gender,
        lang,
        creation_time,
        country,
        address,
    };
    auto ubi_ptr = std::shared_ptr<UserBaseInfo>(ubi);
    std::vector<UserBaseInfoPtr> results{ ubi_ptr };
    */

    return results;
}
