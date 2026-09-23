#include "mocks.h"

#include "bn_vector.h"
#include "file_entry.h"

namespace openflash
{
    namespace mock
    {
        const bn::vector<file_entry, max_file_count> &mock_file_entries()
        {
            static bn::vector<file_entry, max_file_count> files;

            if (!files.empty())
                return files;

            files.push_back(file_entry("GAMES", "/GAMES", file_type::FOLDER, 0, -1, 0, 0));

            // GAMES/Pokemon
            files.push_back(file_entry("Pokemon", "/GAMES/Pokemon", file_type::FOLDER, 1, 0, 1, 0));
            files.push_back(file_entry("Pokemon Emerald.gba", "/GAMES/Pokemon/Pokemon Emerald.gba", file_type::GBA_FILE, 2, 1, 2, 16777216));
            files.push_back(file_entry("Pokemon FireRed.gba", "/GAMES/Pokemon/Pokemon FireRed.gba", file_type::GBA_FILE, 3, 1, 2, 16777216));

            files.push_back(file_entry("Hacks", "/GAMES/Pokemon/Hacks", file_type::FOLDER, 4, 1, 2, 0));
            files.push_back(file_entry("Pokemon Unbound.gba", "/GAMES/Pokemon/Hacks/Pokemon Unbound.gba", file_type::GBA_FILE, 5, 4, 3, 33554432));
            files.push_back(file_entry("Pokemon Radical Red.gba", "/GAMES/Pokemon/Hacks/Pokemon Radical Red.gba", file_type::GBA_FILE, 6, 4, 3, 33554432));

            files.push_back(file_entry("Tools", "/GAMES/Pokemon/Hacks/Tools", file_type::FOLDER, 7, 4, 3, 0));
            files.push_back(file_entry("Patcher.gba", "/GAMES/Pokemon/Hacks/Tools/Patcher.gba", file_type::GBA_FILE, 8, 7, 4, 524288));
            files.push_back(file_entry("Editor.gba", "/GAMES/Pokemon/Hacks/Tools/Editor.gba", file_type::GBA_FILE, 9, 7, 4, 1048576));

            // GAMES/Mario
            files.push_back(file_entry("Mario", "/GAMES/Mario", file_type::FOLDER, 10, 0, 1, 0));
            files.push_back(file_entry("Super Mario Advance.gba", "/GAMES/Mario/Super Mario Advance.gba", file_type::GBA_FILE, 11, 10, 2, 4194304));
            files.push_back(file_entry("Mario Kart Super Circuit.gba", "/GAMES/Mario/Mario Kart Super Circuit.gba", file_type::GBA_FILE, 12, 10, 2, 4194304));
            files.push_back(file_entry("Mario Party Advance.gba", "/GAMES/Mario/Mario Party Advance.gba", file_type::GBA_FILE, 13, 10, 2, 8388608));

            files.push_back(file_entry("Versions", "/GAMES/Mario/Versions", file_type::FOLDER, 14, 10, 2, 0));

            files.push_back(file_entry("USA", "/GAMES/Mario/Versions/USA", file_type::FOLDER, 15, 14, 3, 0));
            files.push_back(file_entry("Mario USA.gba", "/GAMES/Mario/Versions/USA/Mario USA.gba", file_type::GBA_FILE, 16, 15, 4, 4194304));

            files.push_back(file_entry("Europe", "/GAMES/Mario/Versions/Europe", file_type::FOLDER, 17, 14, 3, 0));
            files.push_back(file_entry("Mario Europe.gba", "/GAMES/Mario/Versions/Europe/Mario Europe.gba", file_type::GBA_FILE, 18, 17, 4, 4194304));

            files.push_back(file_entry("Japan", "/GAMES/Mario/Versions/Japan", file_type::FOLDER, 19, 14, 3, 0));
            files.push_back(file_entry("Mario Japan.gba", "/GAMES/Mario/Versions/Japan/Mario Japan.gba", file_type::GBA_FILE, 20, 19, 4, 4194304));

            // GAMES/Zelda
            files.push_back(file_entry("Zelda", "/GAMES/Zelda", file_type::FOLDER, 21, 0, 1, 0));
            files.push_back(file_entry("The Minish Cap.gba", "/GAMES/Zelda/The Minish Cap.gba", file_type::GBA_FILE, 22, 21, 2, 16777216));
            files.push_back(file_entry("A Link to the Past.gba", "/GAMES/Zelda/A Link to the Past.gba", file_type::GBA_FILE, 23, 21, 2, 8388608));

            files.push_back(file_entry("Randomizers", "/GAMES/Zelda/Randomizers", file_type::FOLDER, 24, 21, 2, 0));
            files.push_back(file_entry("Minish Cap Randomizer.gba", "/GAMES/Zelda/Randomizers/Minish Cap Randomizer.gba", file_type::GBA_FILE, 25, 24, 3, 16777216));

            // GAMES/Wario
            files.push_back(file_entry("Wario", "/GAMES/Wario", file_type::FOLDER, 26, 0, 1, 0));
            files.push_back(file_entry("Wario Land 4.gba", "/GAMES/Wario/Wario Land 4.gba", file_type::GBA_FILE, 27, 26, 2, 8388608));
            files.push_back(file_entry("WarioWare Twisted.gba", "/GAMES/Wario/WarioWare Twisted.gba", file_type::GBA_FILE, 28, 26, 2, 16777216));

            // Empty folder
            files.push_back(file_entry("EmptyFolder", "/GAMES/EmptyFolder", file_type::FOLDER, 29, 0, 1, 0));

            // ============================================================
            // SAVES
            // ============================================================

            files.push_back(file_entry("SAVES", "/SAVES", file_type::FOLDER, 30, -1, 0, 0));

            files.push_back(file_entry("Pokemon", "/SAVES/Pokemon", file_type::FOLDER, 31, 30, 1, 0));
            files.push_back(file_entry("Pokemon Emerald.sav", "/SAVES/Pokemon/Pokemon Emerald.sav", file_type::SAVE_FILE, 32, 31, 2, 131072));
            files.push_back(file_entry("Pokemon FireRed.sav", "/SAVES/Pokemon/Pokemon FireRed.sav", file_type::SAVE_FILE, 33, 31, 2, 131072));

            files.push_back(file_entry("Backups", "/SAVES/Backups", file_type::FOLDER, 34, 30, 1, 0));
            files.push_back(file_entry("2026", "/SAVES/Backups/2026", file_type::FOLDER, 35, 34, 2, 0));
            files.push_back(file_entry("January", "/SAVES/Backups/2026/January", file_type::FOLDER, 36, 35, 3, 0));
            files.push_back(file_entry("backup.sav", "/SAVES/Backups/2026/January/backup.sav", file_type::SAVE_FILE, 37, 36, 4, 131072));

            // ============================================================
            // ROMS
            // ============================================================

            files.push_back(file_entry("ROMS", "/ROMS", file_type::FOLDER, 38, -1, 0, 0));

            files.push_back(file_entry("GBA", "/ROMS/GBA", file_type::FOLDER, 39, 38, 1, 0));
            files.push_back(file_entry("Metroid Fusion.gba", "/ROMS/GBA/Metroid Fusion.gba", file_type::GBA_FILE, 40, 39, 2, 8388608));
            files.push_back(file_entry("Metroid Zero Mission.gba", "/ROMS/GBA/Metroid Zero Mission.gba", file_type::GBA_FILE, 41, 39, 2, 8388608));
            files.push_back(file_entry("Castlevania - Aria of Sorrow.gba", "/ROMS/GBA/Castlevania - Aria of Sorrow.gba", file_type::GBA_FILE, 42, 39, 2, 8388608));

            files.push_back(file_entry("GBC", "/ROMS/GBC", file_type::FOLDER, 43, 38, 1, 0));
            files.push_back(file_entry("Pokemon Crystal.gbc", "/ROMS/GBC/Pokemon Crystal.gbc", file_type::NORMAL_FILE, 44, 43, 2, 2097152));
            files.push_back(file_entry("Zelda Oracle of Ages.gbc", "/ROMS/GBC/Zelda Oracle of Ages.gbc", file_type::NORMAL_FILE, 45, 43, 2, 1048576));

            // ============================================================
            // APPS
            // ============================================================

            files.push_back(file_entry("APPS", "/APPS", file_type::FOLDER, 46, -1, 0, 0));

            files.push_back(file_entry("Flash", "/APPS/Flash", file_type::FOLDER, 47, 46, 1, 0));
            files.push_back(file_entry("FlashTool.gba", "/APPS/Flash/FlashTool.gba", file_type::GBA_FILE, 48, 47, 2, 524288));
            files.push_back(file_entry("Flash1M Patcher.gba", "/APPS/Flash/Flash1M Patcher.gba", file_type::GBA_FILE, 49, 47, 2, 262144));

            files.push_back(file_entry("Utilities", "/APPS/Utilities", file_type::FOLDER, 50, 46, 1, 0));
            files.push_back(file_entry("Save Manager.gba", "/APPS/Utilities/Save Manager.gba", file_type::GBA_FILE, 51, 50, 2, 524288));

            // ============================================================
            // CONFIG
            // ============================================================

            files.push_back(file_entry("CONFIG", "/CONFIG", file_type::FOLDER, 52, -1, 0, 0));

            files.push_back(file_entry("Themes", "/CONFIG/Themes", file_type::FOLDER, 53, 52, 1, 0));
            files.push_back(file_entry("Default", "/CONFIG/Themes/Default", file_type::FOLDER, 54, 53, 2, 0));
            files.push_back(file_entry("theme.cfg", "/CONFIG/Themes/Default/theme.cfg", file_type::NORMAL_FILE, 55, 54, 3, 2048));

            files.push_back(file_entry("settings.ini", "/CONFIG/settings.ini", file_type::NORMAL_FILE, 56, 52, 1, 1024));

            // ============================================================
            // TOOLS
            // ============================================================

            files.push_back(file_entry("TOOLS", "/TOOLS", file_type::FOLDER, 57, -1, 0, 0));

            files.push_back(file_entry("Development", "/TOOLS/Development", file_type::FOLDER, 58, 57, 1, 0));

            files.push_back(file_entry("GBA", "/TOOLS/Development/GBA", file_type::FOLDER, 59, 58, 2, 0));
            files.push_back(file_entry("ROM Checker.gba", "/TOOLS/Development/GBA/ROM Checker.gba", file_type::GBA_FILE, 60, 59, 3, 262144));
            files.push_back(file_entry("Header Editor.gba", "/TOOLS/Development/GBA/Header Editor.gba", file_type::GBA_FILE, 61, 59, 3, 262144));

            files.push_back(file_entry("Debug", "/TOOLS/Development/Debug", file_type::FOLDER, 62, 58, 2, 0));
            files.push_back(file_entry("debug.log", "/TOOLS/Development/Debug/debug.log", file_type::NORMAL_FILE, 63, 62, 3, 8192));

            // ============================================================
            // ROOT FILES
            // ============================================================

            files.push_back(file_entry("README.txt", "/README.txt", file_type::NORMAL_FILE, 64, -1, 0, 4096));
            files.push_back(file_entry("boot.gba", "/boot.gba", file_type::GBA_FILE, 65, -1, 0, 262144));
            files.push_back(file_entry("LICENSE.txt", "/LICENSE.txt", file_type::NORMAL_FILE, 66, -1, 0, 2048));
            files.push_back(file_entry("VERSION.txt", "/VERSION.txt", file_type::NORMAL_FILE, 67, -1, 0, 32));

            return files;
        }

        bn::array<uint8_t, gba_header_size> get_gba_header(
            bn::string_view filename_or_path)
        {
            bn::array<uint8_t, gba_header_size> header = {};

            const mock_rom_info *found = nullptr;

            for (const mock_rom_info &rom : mock_roms_infos)
            {
                if (filename_or_path == rom.filename ||
                    filename_or_path == rom.path)
                {
                    found = &rom;
                    break;
                }
            }

            if (!found)
            {
                return header;
            }

            for (int i = 0; i < 12 && i < found->title.size(); ++i)
            {
                header[0xA0 + i] = static_cast<uint8_t>(found->title[i]);
            }

            for (int i = 0; i < 4 && i < found->game_code.size(); ++i)
            {
                header[0xAC + i] = static_cast<uint8_t>(found->game_code[i]);
            }

            for (int i = 0; i < 2 && i < found->maker_code.size(); ++i)
            {
                header[0xB0 + i] = static_cast<uint8_t>(found->maker_code[i]);
            }

            header[0xB2] = 0x96;

            uint8_t checksum = 0;

            for (int i = 0xA0; i < 0xBD; ++i)
            {
                checksum -= header[i];
            }

            header[0xBD] = checksum - 0x19;

            return header;
        }

        rom_infos get_gba_file_info(uint8_t *rom_bytes)
        {
            rom_infos infos;

            if (!rom_bytes)
                return infos;

            // game name
            for (int i = 0xA0; i < 0xAC; ++i)
            {
                uint8_t character = rom_bytes[i];

                if (character == 0x00 || character == 0xFF)
                {
                    break;
                }

                if (character >= 32 && character <= 126)
                {
                    infos.name.push_back(char(character));
                }
            }

            // game code
            for (int i = 0xAC; i < 0xB0; ++i)
            {
                uint8_t character = rom_bytes[i];

                if (character >= 32 && character <= 126)
                {
                    infos.game_code.push_back(char(character));
                }
            }

            // game maker code
            for (int i = 0xB0; i < 0xB2; ++i)
            {
                uint8_t character = rom_bytes[i];

                if (character >= 32 && character <= 126)
                {
                    infos.maker_code.push_back(char(character));
                }
            }

            // complement checksum
            infos.complement_checksum = rom_bytes[0xBD];

            // checking header checksum (simple for now, add more complex check later)
            uint32_t checksum = 0;
            for (int i = 0x04; i < 0xA0; ++i)
                checksum += rom_bytes[i];
            infos.header_valid = checksum == 0x4B1B;

            // Save type
            auto result = bn::find_if(mock_roms_infos.begin(), mock_roms_infos.end(), [&](const mock_rom_info &rom_infos)
                                      { return rom_infos.title == infos.name; });

            if (result == mock_roms_infos.end())
                infos.savetype = save_type::UNKNOWN;
            else
                infos.savetype = result->savetype;

            return infos;
        }
    }
}