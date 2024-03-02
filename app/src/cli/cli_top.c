
#include <zephyr/shell/shell.h>

static int cmd_echo(const struct shell *shell, size_t argc, char *argv[]) {
    if(argc != 2) {
        shell_fprintf(shell, SHELL_WARNING, "MUST PROVIDE INPUT\r\n");
        return -1;
    }
    shell_fprintf(shell, SHELL_NORMAL, "%s\r\n", argv[1]);
    return 0;
}

SHELL_CMD_REGISTER(echo, NULL, "Echos input", cmd_echo);
