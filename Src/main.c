#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <locale.h>
#include "config.h"
#include "fileTOAnkiCard.h"


int getRealPath(char* input_path, char** output_Path) {

    char* full_path = malloc(PATH_MAX);

    if(input_path == NULL)
    {
        return 1;
    }

    if (input_path[0] != '.') {
        strcpy(full_path, input_path);
    } else {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd))) {
            snprintf(full_path, PATH_MAX, "%s%s", cwd,&input_path[1]);
        } else {
            return 1;
        }
    }

    *output_Path = full_path;
    return 0;
}

void replaceBackslashWithSlash(char* str) {
    // 检查空指针
    if (str == NULL) return;
    
    // 遍历字符串直到结束符
    for (int i = 0; str[i] != '\0'; i++) {
        // 检查当前字符是否为反斜杠 (ASCII 92)
        if (str[i] == '\\') {
            // 替换为斜杠 (ASCII 47)
            str[i] = '/';
        }
    }
}

int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "zh-CN.UTF-8");

    char *deck_filed = NULL;
    char *model_filed = NULL;
    char *audio_filed = NULL;
    char *snapshot_filed = NULL;
    char *meaning_filed = NULL;
    char *note_filed = NULL;

    char *meaning_value = NULL;
    char *audio_path = NULL;
    char *audio_realpath = NULL;
    char *snapshot_path = NULL;
    char *snapshot_realpath = NULL;
    char *note_value = NULL;
    
    int opt;
      while ((opt = getopt(argc, argv, "m:a:s:n:")) != -1) {
        switch (opt) {
            case 'm': meaning_value = optarg; break;
            case 'a': audio_path = optarg; break;
            case 's': snapshot_path = optarg; break;
            case 'n': note_value = optarg; break;
            case '?': 
                fprintf(stderr, "未知选项或缺少参数: -%c\n", optopt);
                return 1;
        }
    }

    #ifdef DEBUG
    volatile int debug_wait = 1;
    printf("PID = %d\n", getpid());  // 打印当前进程ID
    while(debug_wait) {  // 循环等待
        sleep(1);  // 避免CPU占用
    }
    #endif

    getRealPath(audio_path, &audio_realpath);
    getRealPath(snapshot_path, &snapshot_realpath);
    replaceBackslashWithSlash(audio_realpath);
    replaceBackslashWithSlash(snapshot_realpath);


    getConfig(&deck_filed, &model_filed, &audio_filed, &snapshot_filed, &meaning_filed, &note_filed);
    setfieldsMap(deck_filed, model_filed, audio_filed, snapshot_filed, meaning_filed, note_filed);    
    
    fileToAnkiCard(audio_realpath, snapshot_realpath, meaning_value, note_value);

    free(deck_filed);
    free(model_filed);
    free(audio_filed);
    free(snapshot_filed);
    free(meaning_filed);
    free(note_filed);
    free(audio_realpath);
    free(snapshot_realpath);

    return 0;
}



