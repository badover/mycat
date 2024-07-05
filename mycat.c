//made by https://github.com/badover
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>

typedef struct options {
    int number_nonblank, number_all, squeeze_blank, show_ends, show_tabs, show_nonprint, new_line;
}options;

options parse_options(int argc,char **argv);
void process_files(options *opts, int argc, char **argv);
void process_line(options *opts, char *line, size_t length);
char handle_nonprintable(char ch);

int main(int argc, char **argv){

    options opts = parse_options(argc, argv);

    process_files(&opts, argc, argv);
    
    return 0;
}

options parse_options(int argc,char **argv){
    options opts = {0};
    struct option long_opts[] = {
        {"number-nonblank", no_argument, NULL, 'b'},
        {"squeeze-blank", no_argument, NULL, 's'},
        {"number", no_argument, NULL, 'n'},
        {0,0,0,0},
    };
    int opt;
    while((opt = getopt_long(argc, argv, "nbseEtTv", long_opts, NULL)) != -1){
        switch (opt){
            case 'b':
                opts.number_nonblank = 1;
                break;
            case 'n':
                opts.show_ends = 1;
            case 'e':
                opts.show_ends = 1;
                opts.show_nonprint = 1;
                break;
            case 'E':
                opts.show_ends = 1;
                break;
            case 't':
                opts.show_tabs = 1;
                opts.show_nonprint;
                break;
            case  'T':
                opts.show_tabs = 1;
                break;
            case 'v':
                opts.show_nonprint = 1;
                break;
            case '?':
            perror("ERROR");
            exit(1);
        }
    }

    if (opts.number_nonblank)
        opts.number_all = 0;
    
    return opts;
    
}



void process_files(options *opts, int argc, char **argv){
    int line_count = 1;
    int blank_line_count = 0;

    for (int i = optind; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL){
            perror("Error while opening file");
            exit(1);
        }

        char *buffer = NULL;
        size_t bufsize = 0;
        ssize_t length;
        while (length = getline(&buffer, &bufsize, file) != -1) {
            if (buffer[0] == '\n'){
                blank_line_count++;
            }else{
                blank_line_count = 0;
            }

            if (!(opts->squeeze_blank && blank_line_count > 1)) {
                if (opts->number_nonblank && buffer[0] != '\n'){
                    printf("%6d\t", line_count++);
                }else if (opts -> number_all) {
                    printf("%6d\t", line_count++);
                }



            
            }
        }


        fclose(file);
    }
}

void process_files(options *opts, char *line, size_t length){
    for(size_t i = 0; i < length; i++){
        int special_case = 0;

        if(opts->show_tabs && line[i] == '\t'){
            printf("^I");
            special_case = 1;
        }

        if (opts->show_ends && line[i] == '\n'){
            putchar("$");
        }

        if(opts ->show_nonprint){
            line[i] = handle_nonprintable(line[i]);
        }
        if(!special_case){
            putchar(line[i]);
        }

        opts->new_line = (line[i] == '\n') ? 0 : 1;

    }
}

char handle_nonprintable(char ch){
    if (ch == '\n' || ch == '\t'){
        return ch;
    }
    if (ch < 32){
        putchar('^');
        ch = ch + 64;
    } else if (ch == 127) {
        putchar('^');
        ch = '?';
    }
    return ch;
}
