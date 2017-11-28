#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "inbuilt.h"
#include "utils.h"


FILE *term = NULL;

void free_cmd(command *cmd)
{
    if (cmd->next != NULL) free_cmd(cmd->next);
    if (cmd->subshell != NULL) free_cmd(cmd->subshell);
    for (int i = 0; cmd->argv[i] != NULL; i++) {
        free(cmd->argv[i]);
    }
    if (cmd->input_file != -1) close(cmd->input_file);
    if (cmd->output_file != -1) close(cmd->output_file);
    free(cmd->argv);
    free(cmd);
}

int last_sym = 256;

token get_word(char **ptr)
{
    *ptr = NULL;
    int str_size = 1;
    int quote = 0;
    int count = 0;
    int c;
    if (last_sym != 256) {
        c = last_sym;
        last_sym = 256;
    }
    else {
        c = getchar();
    }
    while (isspace(c) && c != '\n'){
        c = getchar(); /* Skip spaces */
    }
    do {
        last_sym = 256;
        if (!quote) {
            switch (c) {
                case '\"':
                    quote = 1;
                    continue;
                case '<':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    return IN;
                case '>':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    c = getchar();
                    if (c == '>') return AOUT;
                    else {
                        last_sym = c;
                        return OUT;
                    }
                case '|':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    c = getchar();
                    if (c == '|') return OR;
                    else {
                        last_sym = c;
                        return CON;
                    }
                case '&':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    c = getchar();
                    if (c == '&') return AND;
                    else {
                        last_sym = c;
                        return BCKG;
                    }
                case ';':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    return SEP;
                case '(':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    return SUBSH_ST;
                case ')':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    return END;
                case '~':
                    if (*ptr == NULL) {
                        c = getchar();
                        if (isspace(c) || c == '/') {
                            int len = strlen(getenv("HOME"));
                            str_size += len;
                            count += len;
                            *ptr = (char*)realloc(*ptr, str_size * sizeof(char));
                            strcpy(*ptr, getenv("HOME"));
                            last_sym = c;
                            continue;
                        }
                        else {
                            last_sym = c;
                            c = '~';
                        }
                    }
                    break;
                case EOF: case '\n':
                    if (*ptr != NULL) {
                        last_sym = c;
                        (*ptr)[count] = '\0';
                        return WORD;
                    }
                    return END;
            }
            if (isspace(c)) {
                if (*ptr != NULL) (*ptr)[count] = '\0';
                return WORD;
            }
        }
        else if (c == '\"') {
            quote = 0;
            continue;
        }
        count++;
        if (count == str_size) {
            str_size += 10;
            *ptr = (char*)realloc(*ptr, str_size * sizeof(char));
        }
        (*ptr)[count-1] = c;
    } while ((c = last_sym != 256 ? last_sym : getchar()) != EOF);
    /* EOF encountered */
    (*ptr)[count] = '\0';
    return END;
}

command *init_command()
{
    command *tmp = (command*)malloc(sizeof(command));
    tmp->argv = (char**)malloc(sizeof(char*));
    tmp->argv[0] = NULL;
    tmp->input_file = -1;
    tmp->output_file = -1;
    tmp->backgr = 0;
    tmp->subshell = NULL;
    tmp->link = END;
    tmp->next = NULL;
    return tmp;
}

void skip_string()
{
    char c;
    if (last_sym != 256) {
        c = last_sym;
        last_sym = 256;
    }
    else c = getchar();
    while (c != '\n' && c != EOF)
        c = getchar();
}

command *get_command()
{
    /* Init command */
    command *root = init_command();
    command *cmd = root;

    int require_command = 0;
    int num_of_args = 1; /* With last NULL */
    int may_cont = 0;
    char *str = NULL;
    token tk;
    while ((tk = get_word(&str)) != END || require_command) {
        if (may_cont) {
            may_cont = 0;
            cmd = cmd->next = init_command();
            num_of_args = 1;
        }
        int fd[2];
        if (require_command) {
            require_command = 0;
            while (tk == END) {
                if (term != NULL) {
                    fprintf(term, "> ");
                    fflush(term);
                }
                tk = get_word(&str);
            }
            if (tk != WORD && tk != SUBSH_ST) {
                fprintf(stderr, "Error: unacceptable syntax\n");
                free_cmd(root);
                skip_string();
                return NULL;
            }
        }
        switch (tk) {
            case WORD:
                num_of_args++;
                cmd->argv = (char**)realloc(cmd->argv, num_of_args * sizeof(char*));
                cmd->argv[num_of_args-2] = str;
                cmd->argv[num_of_args-1] = NULL;
                break;
            case IN:
                while ((tk = get_word(&str)) == END) {
                    if (term != NULL) {
                        fprintf(term, "> ");
                        fflush(term);
                    }
                }
                if (tk != WORD) {
                    fprintf(stderr, "Error: unacceptable syntax\n");
                    free_cmd(root);
                    skip_string();
                    return NULL;
                }

                fd[0] = open(str, O_RDONLY);
                free(str);
                if (fd[0] != -1) {
                    if (cmd->input_file != -1) close(cmd->input_file);
                    cmd->input_file = fd[0];
                }
                else { /* Error handle */
                    perror("Error");
                    free_cmd(root);
                    skip_string();
                    return NULL;
                }
                break;
            case OUT:
                while ((tk = get_word(&str)) == END) {
                    if (term != NULL) {
                        fprintf(term, "> ");
                        fflush(term);
                    }
                }
                if (tk != WORD) {
                    fprintf(stderr, "Error: unacceptable syntax\n");
                    free_cmd(root);
                    skip_string();
                    return NULL;
                }

                fd[1] = open(str, O_WRONLY | O_TRUNC | O_CREAT, 0666);
                free(str);
                if (fd[1] != -1) {
                    if (cmd->output_file != -1) close(cmd->output_file);
                    cmd->output_file = fd[1];
                }
                else { /* Error handle */
                    perror("Error");
                    free_cmd(root);
                    skip_string();
                    return NULL;
                }
                break;
            case AOUT:
                while ((tk = get_word(&str)) == END) {
                    if (term != NULL) {
                        fprintf(term, "> ");
                        fflush(term);
                    }
                }
                if (tk != WORD) {
                    fprintf(stderr, "Error: unacceptable syntax\n");
                    free_cmd(root);
                    skip_string();
                    return NULL;
                }

                fd[1] = open(str, O_WRONLY | O_APPEND | O_CREAT, 0666);
                free(str);
                if (fd[1] != -1) {
                    if (cmd->output_file != -1) close(cmd->output_file);
                    cmd->output_file = fd[1];
                }
                else { /* Error handle */
                    perror("Error");
                    free_cmd(root);
                    skip_string();
                    return NULL;
                }
                break;
            case CON:
                require_command = 1;
                cmd->link = CON;
                pipe(fd);
                cmd->output_file = fd[1];
                /* Init next command */
                cmd = cmd->next = init_command();
                cmd->input_file = fd[0];
                num_of_args = 1;
                break;
            case BCKG:
                cmd->backgr = 1;
                if (cmd->input_file == -1)
                    cmd->input_file = open("/dev/null", O_RDONLY);
                cmd->link = BCKG;
                may_cont = 1;
                break;
            case SEP:
                cmd->link = SEP;
                may_cont = 1;
                break;
            case OR:
                require_command = 1;
                cmd->link = OR;
                cmd = cmd->next = init_command();
                num_of_args = 1;
                break;
            case AND:
                require_command = 1;
                cmd->link = AND;
                cmd = cmd->next = init_command();
                num_of_args = 1;
                break;
            case SUBSH_ST:
                if (cmd->argv[0] != NULL) {
                    /* Error */
                }
                cmd->subshell = get_command();
                break;
            case SUBSH_END:
                tk = END;
                break;
            default: ;
        }
    }
    if (root->argv[0] == NULL && root->subshell == NULL) {
        free_cmd(root);
        root = NULL;
    }
    return root;
}

int execute_command(command *cmd)
{
    int status;
    pid_t child = 0;
    if (cmd->subshell != NULL) {
        if ((child = fork()) == 0) { /* Subshell branch */
            if (cmd->input_file != -1) { /* Input redirection */
                dup2(cmd->input_file, 0);
                cmd->input_file = -1;
            }
            if (cmd->output_file != -1) { /* Output redirection */
                dup2(cmd->output_file, 1);
                cmd->output_file = -1;
            }
            status = execute_command(cmd->subshell);
            exit(status);
        }
        else if (child == -1) { /* Error branch */
            perror("Error");
            return -1;
        }
    }
    else if (strcmp(cmd->argv[0], "cd") == 0) {
         status = cd(cmd->argv + 1);
         if (status == -1) {
             perror("Error");
         }
    }
    else if (strcmp(cmd->argv[0], "exit") == 0) {
        free_cmd(cmd);
        exit(0);
    }
    else {
        if ((child = fork()) > 0) { /* Parent branch */
            /* Close files */
            if (cmd->input_file != -1) close(cmd->input_file);
            cmd->input_file = -1;
            if (cmd->output_file != -1) close(cmd->output_file);
            cmd->output_file = -1;
        }
        else if (child == 0) { /* Child branch */
            if (cmd->link != BCKG) signal(SIGINT, SIG_DFL);
            if (cmd->input_file != -1) { /* Input redirection */
                dup2(cmd->input_file, 0);
            }
            if (cmd->output_file != -1) { /* Output redirection */
                dup2(cmd->output_file, 1);
            }

            execvp(cmd->argv[0], cmd->argv);

            perror("Error");
            exit(-1);
        }
        else { /* Error branch */
            perror("Error");
            return -1;
        }
    }

    switch (cmd->link) {
        case CON:
            return execute_command(cmd->next);
        case BCKG:
            return cmd->next == NULL ? 0 : execute_command(cmd->next);
        case SEP:
            if (child != 0) while (wait(&status) != child);
            kill(0, SIGINT); /* SIGINT to all children */
            return cmd->next == NULL ? status : execute_command(cmd->next);
        case OR:
            if (child != 0) while (wait(&status) != child);
            kill(0, SIGINT); /* SIGINT to all children */
            if (status == 0) {
                while (cmd->link == OR)
                    cmd = cmd->next;
            }
            cmd = cmd->next;
            return cmd == 0 ? status : execute_command(cmd);
        case AND:
            if (child != 0) while (wait(&status) != child);
            kill(0, SIGINT); /* SIGINT to all children */
            if (status != 0) {
                while (cmd->link == AND)
                    cmd = cmd->next;
            }
            cmd = cmd->next;
            return cmd == 0 ? status : execute_command(cmd);
        case END:
            if (child != 0) while (wait(&status) != child);
            kill(0, SIGINT); /* SIGINT to all children */
            return status;
        default: ; /* Cannot happen */
            return status;
    }
}

char *get_cwd_name()
{
    int size = 256;
    char *cur_dir = (char*)calloc(size, sizeof(char));
    while (getcwd(cur_dir, size) == NULL) {
        size += 100;
        cur_dir = (char*)realloc(cur_dir, size * sizeof(char));
    }
    if (cur_dir[1] == '\0') return cur_dir; /* Root directory */

    int shift;
    for (int i = 0; cur_dir[i] != '\0'; i++) {
        if (cur_dir[i] == '/') {
            shift = i;
        }
    }
    char *tmp = (char*)calloc(strlen(cur_dir + shift + 1) + 1, sizeof(char));
    strcpy(tmp, cur_dir + shift + 1);
    free(cur_dir);
    return tmp;
}

/* Finish all remaining zombies (from pipelines and background) */
void track_zombies()
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
