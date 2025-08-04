#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h> // cJSON 头文件
#include <windows.h>

unsigned char getConfig(\
    char **deck_name,\
    char **model_name,\
    char **audio,\
    char **snapshot,\
    char **meaning,\
    char **note)
{    
    char exe_path[MAX_PATH];
    char file_path[MAX_PATH];
    const char* relative_path = "/config.json";  // 示例相对路径

    // 1. 获取可执行文件完整路径
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) == 0) {
        fprintf(stderr, "Error: Failed to get executable path (%lu)\n", GetLastError());
        return 1;
    }

    // 2. 提取可执行文件所在目录
    char* last_slash = strrchr(exe_path, '\\');
    if (last_slash == NULL) {
        fprintf(stderr, "Error: Invalid executable path\n");
        return 1;
    }
    *last_slash = '\0';  // 截断路径，保留目录部分

    // 3. 拼接完整文件路径
    snprintf(file_path, sizeof(file_path), "%s\\%s", exe_path, relative_path);

    // 1. 读取 JSON 文件内容
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("无法打开文件");
        return 1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 分配内存并读取内容
    char *json_data = malloc(file_size + 1);
    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0'; // 添加终止符
    fclose(file);

    // 解析 JSON
    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            fprintf(stderr, "JSON 解析错误: %s\n", error_ptr);
        }
        free(json_data);
        return 1;
    }

    //提取顶层键值
    cJSON *deck_name_item = cJSON_GetObjectItemCaseSensitive(root, "deck_name");
    if (cJSON_IsString(deck_name_item) && (deck_name_item->valuestring != NULL)) {
        *deck_name = strdup(deck_name_item->valuestring); // 复制字符串
    }

    cJSON *model_name_item = cJSON_GetObjectItemCaseSensitive(root, "model_name");
    if (cJSON_IsString(model_name_item) && (model_name_item->valuestring != NULL)) {
        *model_name = strdup(model_name_item->valuestring);
    }

    //提取嵌套的 field 对象
    cJSON *field = cJSON_GetObjectItemCaseSensitive(root, "field");
    if (field) {
        cJSON *audio_item = cJSON_GetObjectItemCaseSensitive(field, "Audio");
        if (cJSON_IsString(audio_item) && (audio_item->valuestring != NULL)) {
            *audio = strdup(audio_item->valuestring);
        }

        cJSON *snapshot_item = cJSON_GetObjectItemCaseSensitive(field, "SnapShot");
        if (cJSON_IsString(snapshot_item) && (snapshot_item->valuestring != NULL)) {
            *snapshot = strdup(snapshot_item->valuestring);
        }

        cJSON *meaning_item = cJSON_GetObjectItemCaseSensitive(field, "Meaning");
        if (cJSON_IsString(meaning_item) && (meaning_item->valuestring != NULL)) {
            *meaning = strdup(meaning_item->valuestring);
        }

        cJSON *note_item = cJSON_GetObjectItemCaseSensitive(field, "Note");
        if (cJSON_IsString(note_item) && (note_item->valuestring != NULL)) {
            *note = strdup(note_item->valuestring);
        }
    }

    free(json_data);
    cJSON_Delete(root);

    return 0;
}
