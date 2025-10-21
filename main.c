#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


void
toggle_raw_mode()
{
        static struct termios origin_termios;
        static bool enabled = false;
        struct termios raw_opts;

        /* Disable raw mode if enabled */
        if (enabled) {
                tcsetattr(STDIN_FILENO, TCSANOW, &origin_termios);
                enabled = false;
                return;
        }

        /* Enable raw mode if disabled */
        tcgetattr(STDIN_FILENO, &origin_termios);
        raw_opts = origin_termios;
        cfmakeraw(&raw_opts);
        raw_opts.c_lflag |= ISIG; // enable C-c
        raw_opts.c_oflag |= (OPOST | ONLCR);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw_opts);
        enabled = true;
}

void
print_piano()
{
#define KB_ACTION(_key_, _sound_) printf("%c ", _key_);
#include "config.h"
#undef KB_ACTION
        putchar(10);
}

#define KB_ACTION(_key_, _sound_)                                 \
        case _key_:                                               \
                ma_engine_play_sound(&engine, DIR _sound_, NULL); \
                printf("Play sound: %s\n", _sound_);              \
                break;

int
main()
{
        ma_result result;
        ma_engine engine;
        char k;

        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
                printf("Failed to initialize audio engine\n");
                return result;
        }

        toggle_raw_mode();

        print_piano();

        for (;;) {
                if (read(STDIN_FILENO, &k, 1) <= 0) break;
                switch (k) {
/*           */ #include "config.h"
                case '':
                        goto exit;
                default:
                        printf("Char %c\n", k);
                        break;
                }
        }
exit:


        toggle_raw_mode();
        ma_engine_uninit(&engine);

        return 0;
}
