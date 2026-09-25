#ifndef PROCESS_INFOS_H
#define PROCESS_INFOS_H

namespace openflash
{
    struct time
    {
        int minutes;
        int seconds;
    };

    enum class process_status
    {
        WRITING,
        READING,
        ERASING,
        IDLE
    };

    enum class process_type
    {
        DUMPING,
        FLASHING,
        BACKUP_SAVE,
        RESTORE_SAVE
    };

    struct process_infos
    {
        int progress;
        int speed;
        time elapsed_time;
        time estimated_time;
        process_status status;
        process_type type;
    };
}

#endif // PROCESS_INFOS_H
