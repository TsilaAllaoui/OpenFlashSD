#include "bn_array.h"
#include "bn_string_view.h"
#include <cstdint>

#include "rom_infos.h"
#include "rom_info_api.h"
#include "utilities/async.h"

// Mock respone from server (esp32) side
namespace mock
{
    constexpr int gba_header_size = 0xC0;

    struct mock_rom_info
    {
        bn::string_view filename;
        bn::string_view path;
        bn::string_view title;
        bn::string_view game_code;
        bn::string_view maker_code;
        openflash::save_type savetype;
    };

    using openflash::save_type;

    constexpr bn::array<mock_rom_info, 26> mock_roms =
        {{{"Pokemon Emerald.gba",
           "/GAMES/Pokemon/Pokemon Emerald.gba",
           "POKEMON EMER",
           "BPEE",
           "01",
           save_type::FLASH_128K},
          {"Pokemon FireRed.gba",
           "/GAMES/Pokemon/Pokemon FireRed.gba",
           "POKEMON FIRE",
           "BPRE",
           "01",
           save_type::FLASH_128K},
          {"Pokemon Unbound.gba",
           "/GAMES/Pokemon/Hacks/Pokemon Unbound.gba",
           "PKMN UNBOUND",
           "UNBD",
           "01",
           save_type::FLASH_128K},
          {"Pokemon Radical Red.gba",
           "/GAMES/Pokemon/Hacks/Pokemon Radical Red.gba",
           "RADICAL RED",
           "RRED",
           "01",
           save_type::FLASH_128K},
          {"Patcher.gba",
           "/GAMES/Pokemon/Hacks/Tools/Patcher.gba",
           "PATCHER",
           "PTCH",
           "01",
           save_type::UNKNOWN},
          {"Editor.gba",
           "/GAMES/Pokemon/Hacks/Tools/Editor.gba",
           "EDITOR",
           "EDIT",
           "01",
           save_type::UNKNOWN},
          {"Super Mario Advance.gba",
           "/GAMES/Mario/Super Mario Advance.gba",
           "SUPER MARIO",
           "AMAE",
           "01",
           save_type::EEPROM_512B},
          {"Mario Kart Super Circuit.gba",
           "/GAMES/Mario/Mario Kart Super Circuit.gba",
           "MARIO KART",
           "AMKE",
           "01",
           save_type::FLASH_64K},
          {"Mario Party Advance.gba",
           "/GAMES/Mario/Mario Party Advance.gba",
           "MARIO PARTY",
           "B8ME",
           "01",
           save_type::EEPROM_8K},
          {"Mario USA.gba",
           "/GAMES/Mario/Versions/USA/Mario USA.gba",
           "MARIO USA",
           "MAUE",
           "01",
           save_type::UNKNOWN},
          {"Mario Europe.gba",
           "/GAMES/Mario/Versions/Europe/Mario Europe.gba",
           "MARIO EUROPE",
           "MAUP",
           "01",
           save_type::UNKNOWN},
          {"Mario Japan.gba",
           "/GAMES/Mario/Versions/Japan/Mario Japan.gba",
           "MARIO JAPAN",
           "MAUJ",
           "01",
           save_type::UNKNOWN},
          {"The Minish Cap.gba",
           "/GAMES/Zelda/The Minish Cap.gba",
           "ZELDA MC",
           "BZME",
           "01",
           save_type::EEPROM_8K},
          {"A Link to the Past.gba",
           "/GAMES/Zelda/A Link to the Past.gba",
           "ZELDA ALTTP",
           "AZLE",
           "01",
           save_type::EEPROM_8K},
          {"Minish Cap Randomizer.gba",
           "/GAMES/Zelda/Randomizers/Minish Cap Randomizer.gba",
           "MC RANDOM",
           "MCRN",
           "01",
           save_type::EEPROM_8K},
          {"Wario Land 4.gba",
           "/GAMES/Wario/Wario Land 4.gba",
           "WARIOLAND4",
           "AWAE",
           "01",
           save_type::SRAM_32K},
          {"WarioWare Twisted.gba",
           "/GAMES/Wario/WarioWare Twisted.gba",
           "WARIOWARE",
           "RZWE",
           "01",
           save_type::SRAM_32K},
          {"Metroid Fusion.gba",
           "/ROMS/GBA/Metroid Fusion.gba",
           "METROID4",
           "AMTE",
           "01",
           save_type::SRAM_32K},
          {"Metroid Zero Mission.gba",
           "/ROMS/GBA/Metroid Zero Mission.gba",
           "METROID ZERO",
           "BMXE",
           "01",
           save_type::FRAM_32K},
          {"Castlevania - Aria of Sorrow.gba",
           "/ROMS/GBA/Castlevania - Aria of Sorrow.gba",
           "CASTLEVANIA",
           "A2CE",
           "01",
           save_type::FRAM_32K},
          {"FlashTool.gba",
           "/APPS/Flash/FlashTool.gba",
           "FLASHTOOL",
           "FLSH",
           "01",
           save_type::UNKNOWN},
          {"Flash1M Patcher.gba",
           "/APPS/Flash/Flash1M Patcher.gba",
           "FLASH1M",
           "F1MP",
           "01",
           save_type::UNKNOWN},
          {"Save Manager.gba",
           "/APPS/Utilities/Save Manager.gba",
           "SAVE MANAGER",
           "SAVE",
           "01",
           save_type::UNKNOWN},
          {"ROM Checker.gba",
           "/TOOLS/Development/GBA/ROM Checker.gba",
           "ROM CHECKER",
           "ROMC",
           "01",
           save_type::UNKNOWN},
          {"Header Editor.gba",
           "/TOOLS/Development/GBA/Header Editor.gba",
           "HEADER EDIT",
           "HEDT",
           "01",
           save_type::UNKNOWN},
          {"boot.gba",
           "/boot.gba",
           "OPENFLASHSD",
           "OFSD",
           "01",
           save_type::NONE}}};

    bn::array<uint8_t, gba_header_size> get_gba_header(
        bn::string_view filename_or_path)
    {
        bn::array<uint8_t, gba_header_size> header = {};

        const mock_rom_info *found = nullptr;

        for (const mock_rom_info &rom : mock_roms)
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

    openflash::rom_infos get_gba_file_info(uint8_t *rom_bytes)
    {
        openflash::rom_infos infos;

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
        auto result = bn::find_if(mock::mock_roms.begin(), mock::mock_roms.end(), [&](const mock::mock_rom_info &rom_infos)
                                  { return rom_infos.title == infos.name; });

        if (result == mock::mock_roms.end())
            infos.savetype = save_type::UNKNOWN;
        else
            infos.savetype = result->savetype;

        return infos;
    }
}

namespace openflash
{
    namespace api
    {
        rom_info_api &api::rom_info_api::instance()
        {
            static rom_info_api api;
            return api;
        }

        bn::optional<rom_infos> api::rom_info_api::get_current_rom_infos(const file_entry &file)
        {
            // simulate wait time
            async::delay(60);

            // get current rom infos from api (esp32)
            auto header = mock::get_gba_header(file.path);
            auto rom_infos = mock::get_gba_file_info(header.data());
            rom_infos.file_path = file.path;

            _current_rom_infos.emplace(rom_infos);
            return _current_rom_infos;
        }
    }
}
