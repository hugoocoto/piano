#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

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

int
piano_length()
{
/*   */ #define KEY(...) +1
        return
/*   */ #include "config.h"
        ;
/*   */ #undef KEY
}

void
print_piano(char press)
{
        int len = piano_length();
        int i = 0;

        printf("\033[H\033[2J");
        printf("\033[?25l");

#define KEY(_key_, _sound_)                                                         \
        do {                                                                        \
                char *ncol = press == _key_ ? "\033[41;30m" : "\033[47;30m";        \
                char *xcol = "\033[40;30m";                                         \
                char *none = "\033[0m";                                             \
                if (i == 0) {                                                       \
                        i++;                                                        \
                        printf("%s   %s  ", ncol, xcol);                            \
                        printf("%s%s   %s  ", "\033[5D\033[B", ncol, xcol);         \
                        printf("%s%s   %s  ", "\033[5D\033[B", ncol, xcol);         \
                        printf("%s%s %c  %s ", "\033[5D\033[B", ncol, _key_, none); \
                        printf("%s%s    %s ", "\033[5D\033[B", ncol, none);         \
                        printf("\033[0m\033[4A");                                   \
                } else if (i == len - 1) {                                          \
                        i++;                                                        \
                        printf("%s %s   ", xcol, ncol);                             \
                        printf("%s%s %s   ", "\033[4D\033[B", xcol, ncol);          \
                        printf("%s%s %s   ", "\033[4D\033[B", xcol, ncol);          \
                        printf("%s %c  ", "\033[4D\033[B", _key_);                  \
                        printf("%s    ", "\033[4D\033[B");                          \
                        printf("\033[0m\033[4A");                                   \
                } else {                                                            \
                        i++;                                                        \
                        printf("%s %s  %s  ", xcol, ncol, xcol);                    \
                        printf("%s%s %s  %s  ", "\033[5D\033[B", xcol, ncol, xcol); \
                        printf("%s%s %s  %s  ", "\033[5D\033[B", xcol, ncol, xcol); \
                        printf("%s%s %c  %s ", "\033[5D\033[B", ncol, _key_, none); \
                        printf("%s%s    %s ", "\033[5D\033[B", ncol, none);         \
                        printf("\033[0m\033[4A");                                   \
                }                                                                   \
        } while (0);
#include "config.h"
#undef KEY
        fflush(stdout);
}

#define KEY(_key_, _sound_)                                       \
        case _key_:                                               \
                ma_engine_play_sound(&engine, DIR _sound_, NULL); \
                print_piano(_key_);                               \
                alarm(1);                                         \
                break;

void
reset(int)
{
        print_piano(0);
}

int
main()
{
        ma_result result;
        ma_engine engine;
        char k;

        signal(SIGALRM, reset);
        signal(SIGINT, SIG_IGN);

        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
                printf("Failed to initialize audio engine\n");
                return result;
        }

        toggle_raw_mode();

        print_piano(0);

        for (;;) {
                if (read(STDIN_FILENO, &k, 1) <= 0) break;
                switch (k) {
/*           */ #include "config.h"
                case '':
                        goto exit;
                }
        }
exit:
        toggle_raw_mode();
        ma_engine_uninit(&engine);
        printf("\033[?25h");

        return 0;
}
