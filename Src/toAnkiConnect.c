#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

// AnkiConnect 的 URL
#define ANKI_CONNECT_URL "http://localhost:8765"

// 用于存储 HTTP 响应数据
struct string {
    char *ptr;
    size_t len;
};

// 初始化字符串结构
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

// 用于 libcurl 的回调函数，将响应数据写入字符串
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}

// 发送 HTTP POST 请求到 AnkiConnect
char *anki_connect_request(const char *action, const char *params) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    curl = curl_easy_init();
    if (curl) {
        // 构建 JSON 请求体
        char request_body[1024];
        snprintf(request_body, sizeof(request_body), "{\"action\":\"%s\",\"params\":%s,\"version\":6}", action, params);

        // 设置 HTTP 请求选项
        curl_easy_setopt(curl, CURLOPT_URL, ANKI_CONNECT_URL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        // 发送请求
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // 清理
        curl_easy_cleanup(curl);
    }

    return s.ptr;
}

// 将文件内容转换为 Base64 编码 
// char *file_to_base64(const char *file_path) {
//     FILE *file = fopen(file_path, "rb");
//     if (!file) {
//         fprintf(stderr, "Failed to open file: %s\n", file_path);
//         return NULL;
//     }

//     // 获取文件大小
//     fseek(file, 0, SEEK_END);
//     long file_size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     // 读取文件内容
//     unsigned char *file_data = malloc(file_size);
//     fread(file_data, 1, file_size, file);
//     fclose(file);

//     // 使用 OpenSSL 进行 Base64 编码
//     BIO *bio, *b64;
//     BUF_MEM *bufferPtr;

//     b64 = BIO_new(BIO_f_base64());
//     bio = BIO_new(BIO_s_mem());
//     bio = BIO_push(b64, bio);

//     // 写入数据到 BIO
//     BIO_write(bio, file_data, file_size);
//     BIO_flush(bio);

//     // 获取编码后的数据
//     BIO_get_mem_ptr(bio, &bufferPtr);
//     char *base64_data = malloc(bufferPtr->length + 1);
//     memcpy(base64_data, bufferPtr->data, bufferPtr->length);
//     base64_data[bufferPtr->length] = '\0'; // 添加字符串结束符

//     // 清理
//     BIO_free_all(bio);
//     free(file_data);

//     return base64_data;
// }

// 存储媒体文件到 Anki 的媒体库
void store_media_file(const char *filename, const char *path) {
    char params[1024];
    snprintf(params, sizeof(params), "{\"filename\":\"%s\",\"path\":\"%s\"}", filename, path);
    char *response = anki_connect_request("storeMediaFile", params);
    printf("storeMediaFile response: %s\n", response);
    free(response);
}

// 添加卡片到 Anki

//$ gcc ./invoke.c -lcurl -lssl -lcrypto -g
