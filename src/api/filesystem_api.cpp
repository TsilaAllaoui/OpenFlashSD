#include "bn_core.h"
#include "filesystem_api.h"
#include "utilities/async.h"
#include "utilities/pop_up.h"

namespace mock
{
    bn::vector<openflash::file_entry, max_file_count> mockfiles()
    {
        bn::vector<openflash::file_entry, max_file_count> files;

        files.push_back(openflash::file_entry("GAMES", "/GAMES", openflash::file_type::FOLDER, 0, -1, 0, 0));

        // GAMES/Pokemon
        files.push_back(openflash::file_entry("Pokemon", "/GAMES/Pokemon", openflash::file_type::FOLDER, 1, 0, 1, 0));
        files.push_back(openflash::file_entry("Pokemon Emerald.gba", "/GAMES/Pokemon/Pokemon Emerald.gba", openflash::file_type::GBA_FILE, 2, 1, 2, 16777216));
        files.push_back(openflash::file_entry("Pokemon FireRed.gba", "/GAMES/Pokemon/Pokemon FireRed.gba", openflash::file_type::GBA_FILE, 3, 1, 2, 16777216));

        files.push_back(openflash::file_entry("Hacks", "/GAMES/Pokemon/Hacks", openflash::file_type::FOLDER, 4, 1, 2, 0));
        files.push_back(openflash::file_entry("Pokemon Unbound.gba", "/GAMES/Pokemon/Hacks/Pokemon Unbound.gba", openflash::file_type::GBA_FILE, 5, 4, 3, 33554432));
        files.push_back(openflash::file_entry("Pokemon Radical Red.gba", "/GAMES/Pokemon/Hacks/Pokemon Radical Red.gba", openflash::file_type::GBA_FILE, 6, 4, 3, 33554432));

        files.push_back(openflash::file_entry("Tools", "/GAMES/Pokemon/Hacks/Tools", openflash::file_type::FOLDER, 7, 4, 3, 0));
        files.push_back(openflash::file_entry("Patcher.gba", "/GAMES/Pokemon/Hacks/Tools/Patcher.gba", openflash::file_type::GBA_FILE, 8, 7, 4, 524288));
        files.push_back(openflash::file_entry("Editor.gba", "/GAMES/Pokemon/Hacks/Tools/Editor.gba", openflash::file_type::GBA_FILE, 9, 7, 4, 1048576));

        // GAMES/Mario
        files.push_back(openflash::file_entry("Mario", "/GAMES/Mario", openflash::file_type::FOLDER, 10, 0, 1, 0));
        files.push_back(openflash::file_entry("Super Mario Advance.gba", "/GAMES/Mario/Super Mario Advance.gba", openflash::file_type::GBA_FILE, 11, 10, 2, 4194304));
        files.push_back(openflash::file_entry("Mario Kart Super Circuit.gba", "/GAMES/Mario/Mario Kart Super Circuit.gba", openflash::file_type::GBA_FILE, 12, 10, 2, 4194304));
        files.push_back(openflash::file_entry("Mario Party Advance.gba", "/GAMES/Mario/Mario Party Advance.gba", openflash::file_type::GBA_FILE, 13, 10, 2, 8388608));

        files.push_back(openflash::file_entry("Versions", "/GAMES/Mario/Versions", openflash::file_type::FOLDER, 14, 10, 2, 0));

        files.push_back(openflash::file_entry("USA", "/GAMES/Mario/Versions/USA", openflash::file_type::FOLDER, 15, 14, 3, 0));
        files.push_back(openflash::file_entry("Mario USA.gba", "/GAMES/Mario/Versions/USA/Mario USA.gba", openflash::file_type::GBA_FILE, 16, 15, 4, 4194304));

        files.push_back(openflash::file_entry("Europe", "/GAMES/Mario/Versions/Europe", openflash::file_type::FOLDER, 17, 14, 3, 0));
        files.push_back(openflash::file_entry("Mario Europe.gba", "/GAMES/Mario/Versions/Europe/Mario Europe.gba", openflash::file_type::GBA_FILE, 18, 17, 4, 4194304));

        files.push_back(openflash::file_entry("Japan", "/GAMES/Mario/Versions/Japan", openflash::file_type::FOLDER, 19, 14, 3, 0));
        files.push_back(openflash::file_entry("Mario Japan.gba", "/GAMES/Mario/Versions/Japan/Mario Japan.gba", openflash::file_type::GBA_FILE, 20, 19, 4, 4194304));

        // GAMES/Zelda
        files.push_back(openflash::file_entry("Zelda", "/GAMES/Zelda", openflash::file_type::FOLDER, 21, 0, 1, 0));
        files.push_back(openflash::file_entry("The Minish Cap.gba", "/GAMES/Zelda/The Minish Cap.gba", openflash::file_type::GBA_FILE, 22, 21, 2, 16777216));
        files.push_back(openflash::file_entry("A Link to the Past.gba", "/GAMES/Zelda/A Link to the Past.gba", openflash::file_type::GBA_FILE, 23, 21, 2, 8388608));

        files.push_back(openflash::file_entry("Randomizers", "/GAMES/Zelda/Randomizers", openflash::file_type::FOLDER, 24, 21, 2, 0));
        files.push_back(openflash::file_entry("Minish Cap Randomizer.gba", "/GAMES/Zelda/Randomizers/Minish Cap Randomizer.gba", openflash::file_type::GBA_FILE, 25, 24, 3, 16777216));

        // GAMES/Wario
        files.push_back(openflash::file_entry("Wario", "/GAMES/Wario", openflash::file_type::FOLDER, 26, 0, 1, 0));
        files.push_back(openflash::file_entry("Wario Land 4.gba", "/GAMES/Wario/Wario Land 4.gba", openflash::file_type::GBA_FILE, 27, 26, 2, 8388608));
        files.push_back(openflash::file_entry("WarioWare Twisted.gba", "/GAMES/Wario/WarioWare Twisted.gba", openflash::file_type::GBA_FILE, 28, 26, 2, 16777216));

        // Empty folder
        files.push_back(openflash::file_entry("EmptyFolder", "/GAMES/EmptyFolder", openflash::file_type::FOLDER, 29, 0, 1, 0));

        // ============================================================
        // SAVES
        // ============================================================

        files.push_back(openflash::file_entry("SAVES", "/SAVES", openflash::file_type::FOLDER, 30, -1, 0, 0));

        files.push_back(openflash::file_entry("Pokemon", "/SAVES/Pokemon", openflash::file_type::FOLDER, 31, 30, 1, 0));
        files.push_back(openflash::file_entry("Pokemon Emerald.sav", "/SAVES/Pokemon/Pokemon Emerald.sav", openflash::file_type::NORMAL_FILE, 32, 31, 2, 131072));
        files.push_back(openflash::file_entry("Pokemon FireRed.sav", "/SAVES/Pokemon/Pokemon FireRed.sav", openflash::file_type::NORMAL_FILE, 33, 31, 2, 131072));

        files.push_back(openflash::file_entry("Backups", "/SAVES/Backups", openflash::file_type::FOLDER, 34, 30, 1, 0));
        files.push_back(openflash::file_entry("2026", "/SAVES/Backups/2026", openflash::file_type::FOLDER, 35, 34, 2, 0));
        files.push_back(openflash::file_entry("January", "/SAVES/Backups/2026/January", openflash::file_type::FOLDER, 36, 35, 3, 0));
        files.push_back(openflash::file_entry("backup.sav", "/SAVES/Backups/2026/January/backup.sav", openflash::file_type::NORMAL_FILE, 37, 36, 4, 131072));

        // ============================================================
        // ROMS
        // ============================================================

        files.push_back(openflash::file_entry("ROMS", "/ROMS", openflash::file_type::FOLDER, 38, -1, 0, 0));

        files.push_back(openflash::file_entry("GBA", "/ROMS/GBA", openflash::file_type::FOLDER, 39, 38, 1, 0));
        files.push_back(openflash::file_entry("Metroid Fusion.gba", "/ROMS/GBA/Metroid Fusion.gba", openflash::file_type::GBA_FILE, 40, 39, 2, 8388608));
        files.push_back(openflash::file_entry("Metroid Zero Mission.gba", "/ROMS/GBA/Metroid Zero Mission.gba", openflash::file_type::GBA_FILE, 41, 39, 2, 8388608));
        files.push_back(openflash::file_entry("Castlevania - Aria of Sorrow.gba", "/ROMS/GBA/Castlevania - Aria of Sorrow.gba", openflash::file_type::GBA_FILE, 42, 39, 2, 8388608));

        files.push_back(openflash::file_entry("GBC", "/ROMS/GBC", openflash::file_type::FOLDER, 43, 38, 1, 0));
        files.push_back(openflash::file_entry("Pokemon Crystal.gbc", "/ROMS/GBC/Pokemon Crystal.gbc", openflash::file_type::NORMAL_FILE, 44, 43, 2, 2097152));
        files.push_back(openflash::file_entry("Zelda Oracle of Ages.gbc", "/ROMS/GBC/Zelda Oracle of Ages.gbc", openflash::file_type::NORMAL_FILE, 45, 43, 2, 1048576));

        // ============================================================
        // APPS
        // ============================================================

        files.push_back(openflash::file_entry("APPS", "/APPS", openflash::file_type::FOLDER, 46, -1, 0, 0));

        files.push_back(openflash::file_entry("Flash", "/APPS/Flash", openflash::file_type::FOLDER, 47, 46, 1, 0));
        files.push_back(openflash::file_entry("FlashTool.gba", "/APPS/Flash/FlashTool.gba", openflash::file_type::GBA_FILE, 48, 47, 2, 524288));
        files.push_back(openflash::file_entry("Flash1M Patcher.gba", "/APPS/Flash/Flash1M Patcher.gba", openflash::file_type::GBA_FILE, 49, 47, 2, 262144));

        files.push_back(openflash::file_entry("Utilities", "/APPS/Utilities", openflash::file_type::FOLDER, 50, 46, 1, 0));
        files.push_back(openflash::file_entry("Save Manager.gba", "/APPS/Utilities/Save Manager.gba", openflash::file_type::GBA_FILE, 51, 50, 2, 524288));

        // ============================================================
        // CONFIG
        // ============================================================

        files.push_back(openflash::file_entry("CONFIG", "/CONFIG", openflash::file_type::FOLDER, 52, -1, 0, 0));

        files.push_back(openflash::file_entry("Themes", "/CONFIG/Themes", openflash::file_type::FOLDER, 53, 52, 1, 0));
        files.push_back(openflash::file_entry("Default", "/CONFIG/Themes/Default", openflash::file_type::FOLDER, 54, 53, 2, 0));
        files.push_back(openflash::file_entry("theme.cfg", "/CONFIG/Themes/Default/theme.cfg", openflash::file_type::NORMAL_FILE, 55, 54, 3, 2048));

        files.push_back(openflash::file_entry("settings.ini", "/CONFIG/settings.ini", openflash::file_type::NORMAL_FILE, 56, 52, 1, 1024));

        // ============================================================
        // TOOLS
        // ============================================================

        files.push_back(openflash::file_entry("TOOLS", "/TOOLS", openflash::file_type::FOLDER, 57, -1, 0, 0));

        files.push_back(openflash::file_entry("Development", "/TOOLS/Development", openflash::file_type::FOLDER, 58, 57, 1, 0));

        files.push_back(openflash::file_entry("GBA", "/TOOLS/Development/GBA", openflash::file_type::FOLDER, 59, 58, 2, 0));
        files.push_back(openflash::file_entry("ROM Checker.gba", "/TOOLS/Development/GBA/ROM Checker.gba", openflash::file_type::GBA_FILE, 60, 59, 3, 262144));
        files.push_back(openflash::file_entry("Header Editor.gba", "/TOOLS/Development/GBA/Header Editor.gba", openflash::file_type::GBA_FILE, 61, 59, 3, 262144));

        files.push_back(openflash::file_entry("Debug", "/TOOLS/Development/Debug", openflash::file_type::FOLDER, 62, 58, 2, 0));
        files.push_back(openflash::file_entry("debug.log", "/TOOLS/Development/Debug/debug.log", openflash::file_type::NORMAL_FILE, 63, 62, 3, 8192));

        // ============================================================
        // ROOT FILES
        // ============================================================

        files.push_back(openflash::file_entry("README.txt", "/README.txt", openflash::file_type::NORMAL_FILE, 64, -1, 0, 4096));
        files.push_back(openflash::file_entry("boot.gba", "/boot.gba", openflash::file_type::GBA_FILE, 65, -1, 0, 262144));
        files.push_back(openflash::file_entry("LICENSE.txt", "/LICENSE.txt", openflash::file_type::NORMAL_FILE, 66, -1, 0, 2048));
        files.push_back(openflash::file_entry("VERSION.txt", "/VERSION.txt", openflash::file_type::NORMAL_FILE, 67, -1, 0, 32));

        return files;
    }
}

namespace openflash
{
    namespace api
    {
        filesystem_api &api::filesystem_api::instance()
        {
            static filesystem_api api;
            return api;
        }

        bn::vector<file_entry, max_file_count> api::filesystem_api::get_files()
        {
            // simulating wait time
            async::delay(60);

            // get files from server side (esp32)
            auto files = mock::mockfiles();
            _files.clear();
            for (const auto &file : files)
                _files.emplace_back(file);
            return _files;
        }
    }
}
