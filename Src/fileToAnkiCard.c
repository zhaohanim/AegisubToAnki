#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "toAnkiConnect.h"
#include "fileTOAnkiCard.h"

#define RANDOMCHARLENGTH 10
#define RANDOMCHARENDLENGTH 10

static char* sDeck_name = "Default";
static char* sModel_name = "Basic";
static char* sAudio_field = "Audio";
static char* sPicture_field = "Snapshot";
static char* sMeaning_field = "Meaning";
static char* sNote_filed = "Notes";

// 生成特定长度的随机数字字符串用于命名
static char* generate_random_filename(char* file_format) {

    char stringHead[] = "aegisub_";
    char* string = malloc(strlen(stringHead) + RANDOMCHARLENGTH + strlen(file_format));

    memcpy(string, stringHead, strlen(stringHead));

    // 设置随机种子
    srand(time(NULL));

    for (int i = 0; i < RANDOMCHARLENGTH; i++) {
        // 生成 0 到 9 的随机数
        int digit = rand() % 10;
        // 将数字转换为字符并存入字符串
        string[strlen(stringHead)+i] = digit + '0';
    }

    memcpy(&string[strlen(stringHead)+RANDOMCHARLENGTH], file_format, strlen(file_format));
    // 添加字符串终止符
    string[strlen(stringHead) + RANDOMCHARLENGTH + strlen(file_format)] = '\0';

    return string;
}

void fileToAnkiCard(char *audio_path,char *picture_path,char *Meaning,char* Notes) {
    // 存储音频文件
    char *audio_filename = NULL;
    char *picture_filename = NULL;

    if (audio_path) {
        audio_filename = generate_random_filename(".wav");
        store_media_file(audio_filename, audio_path);
    }

    // 存储图片文件
    if (picture_path) {
        picture_filename = generate_random_filename(".png");
        store_media_file(picture_filename, picture_path);
    }

    // 添加卡片
    char params[1024] ;
    char fields_buffer[1024];
    char fields[1024]= "{";
    
    if(audio_path != NULL)
    {
        snprintf(fields_buffer, sizeof(fields_buffer),"\"%s\": \"[sound:%s]\"", sAudio_field, audio_filename);
        strcat(fields, fields_buffer);
    }

    if(picture_path != NULL)
    {
        snprintf(fields_buffer, sizeof(fields_buffer),",\"%s\":\"<img src=%s>\"", sPicture_field, picture_filename);
        strcat(fields, fields_buffer);
    }

    if(Meaning != NULL)
    {
        snprintf(fields_buffer, sizeof(fields_buffer),",\"%s\":\"%s\"", sMeaning_field, Meaning);
        strcat(fields, fields_buffer);
    }
    
    if(Notes != NULL)
    {
        snprintf(fields_buffer, sizeof(fields_buffer),",\"%s\":\"%s\"",sNote_filed, Notes);
        strcat(fields, fields_buffer);
    }

    strcat(fields, "}");

    snprintf(params, sizeof(params), "{\"note\":{\"deckName\":\"%s\",\"modelName\":\"%s\",\"fields\":%s}}", sDeck_name, sModel_name, fields);

    char *response = anki_connect_request("addNote", params);
    printf("addNote response: %s\n", response);
    free(response);

    if(audio_filename != NULL)
    {
        free(audio_filename);
    }
    
    if(picture_filename != NULL)
    {
        free(picture_filename);
    }
    
}

void setfieldsMap(char *deck_name,char *model_name,char *audio_field,char *picture_field,char *meaning_field,char* notes_field)
{
    sDeck_name = deck_name;
    sModel_name = model_name;
    sAudio_field = audio_field;
    sPicture_field = picture_field;
    sMeaning_field = meaning_field;
    sNote_filed = notes_field;
}

