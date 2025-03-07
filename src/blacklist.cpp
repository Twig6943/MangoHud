#include <vector>
#include <string>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <filesystem.h>

#include "blacklist.h"
#include "string_utils.h"
#include "file_utils.h"

namespace fs = ghc::filesystem;
std::string global_proc_name;

static std::string get_proc_name() {
   // Note: It is possible to use GNU program_invocation_short_name.
   const std::string proc_name = get_wine_exe_name(/*keep_ext=*/true);
   if (!proc_name.empty()) {
       return proc_name;
   }
   return get_basename(get_exe_path());
}

static  std::vector<std::string> blacklist {
    "Amazon Games UI.exe",
    "Battle.net.exe",
    "BethesdaNetLauncher.exe",
    "EpicGamesLauncher.exe",
    "halloy",
    "IGOProxy.exe",
    "IGOProxy64.exe",
    "monado-service",
    "Origin.exe",
    "OriginThinSetupInternal.exe",
    "steam",
    "steamwebhelper",
    "vrcompositor",
    "gldriverquery",
    "vulkandriverquery",
    "Steam.exe",
    "ffxivlauncher.exe",
    "ffxivlauncher64.exe",
    "LeagueClient.exe",
    "LeagueClientUxRender.exe",
    "SocialClubHelper.exe",
    "EADesktop.exe",
    "EALauncher.exe",
    "StarCitizen_Launcher.exe",
    "InsurgencyEAC.exe",
    "GalaxyClient.exe",
    "REDprelauncher.exe",
    "REDlauncher.exe",
    "gamescope",
    "RSI Launcher.exe",
    "tabtip.exe",
    "steam.exe",
    "explorer.exe",
    "wine-preloader",
    "iexplore.exe",
    "rundll32.exe",
    "Launcher", //Paradox Interactive Launcher
    "steamwebhelper.exe",
    "EpicWebHelper.exe",
    "UplayWebCore.exe",
    "plutonium.exe",
    "plutonium-launcher-win32.exe"
};

static bool check_gtk() {
    fs::path path("/proc/self/map_files/");
    for (auto& p : fs::directory_iterator(path)) {
        auto filename = p.path().string();
        auto sym = read_symlink(filename.c_str());
        if (sym.find("gtk") != std::string::npos)
            return true;
    }

    return false;
}


static bool check_blacklisted() {
    std::string proc_name = get_proc_name();
    global_proc_name = proc_name;
    bool blacklisted = std::find(blacklist.begin(), blacklist.end(), proc_name) != blacklist.end();

    // if the app isn't blacklisted, check if the app uses GTK and blacklist anyway
    if (!blacklisted)
        blacklisted = check_gtk();

    static bool printed = false;
    if(blacklisted && !printed) {
        printed = true;
        SPDLOG_INFO("process '{}' is blacklisted in MangoHud", proc_name);
    }

    return blacklisted;
}

bool is_blacklisted(bool force_recheck) {
    static bool blacklisted = check_blacklisted();
    if (force_recheck)
        blacklisted = check_blacklisted();
    return blacklisted;
}

void add_blacklist(const std::string& new_item) {
    // check if item exits in blacklist before adding new item
    if(std::find(blacklist.begin(), blacklist.end(), new_item) != blacklist.end()) {
        return;
    }

    blacklist.push_back (new_item);
    is_blacklisted(true);
}


