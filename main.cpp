#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <regex>

// -- INFOS -- //

std::string RobloxUrl = "https://www.roblox.com";
long long int TargetFavoriteItem = 17315767016; // Change this with your target item's asset id

// -- Functions and Main -- //

nlohmann::json ToJson(std::string JsonSource) {
    return nlohmann::json::parse(JsonSource.c_str());
}

std::string GetAccounts() {
    std::ifstream file("Accounts.txt");

    if (!file.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return "";
    }
    std::string fileContents((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    file.close();
    return fileContents;
}

void FavTarget(long long int TargetID) {
    std::string AccountTXT = GetAccounts();
    std::regex pattern("Username: (.+)\nCookie: (.+)");
    std::sregex_iterator iter(AccountTXT.begin(), AccountTXT.end(), pattern);
    std::sregex_iterator end;

    while (iter != end) {
        std::smatch match = *iter;
        std::string MyUser = match[1];
        std::string Cookie = match[2];

        //std::cout << Cookie << std::endl;

        cpr::Session newsession;
        newsession.SetHeader(cpr::Header{{"Content-Type", "application/json"}});
        newsession.SetCookies(cpr::Cookies{{".ROBLOSECURITY", Cookie.c_str()}});
        newsession.SetUrl(cpr::Url("https://auth.roblox.com/v2/logout"));

        cpr::Response resp = newsession.Post();

        auto it = resp.header.find("x-csrf-token");
        if (it != resp.header.end()) {
            newsession.UpdateHeader(cpr::Header({{"referer", RobloxUrl.c_str()}, {"x-csrf-token", it->second}}));
        }

        nlohmann::json BodyGetUserid;
        BodyGetUserid["usernames"] = {MyUser.c_str()};
        BodyGetUserid["excludeBannedUsers"] = true;

        newsession.SetBody(cpr::Body(BodyGetUserid.dump()));
        newsession.SetUrl(cpr::Url("https://users.roblox.com/v1/usernames/users"));
        
        cpr::Response UseridResponse = newsession.Post();
        nlohmann::json UseridJson = ToJson(UseridResponse.text);
        long long int MyUserId = UseridJson["data"][0]["id"];

        newsession.SetBody(cpr::Body()); // Set body to nothing

        std::string TargetUrl = "https://catalog.roblox.com/v1/favorites/users/" + std::to_string(MyUserId) + "/assets/" + std::to_string(TargetID) + "/favorite";
        newsession.SetUrl(cpr::Url(TargetUrl.c_str()));

        cpr::Response FavResponse = newsession.Post();

        std::cout << FavResponse.status_code << std::endl;

        iter++;

        Sleep(1500); // 1.5 sec
    }
}

int main(int, char**){
    FavTarget(TargetFavoriteItem);
}