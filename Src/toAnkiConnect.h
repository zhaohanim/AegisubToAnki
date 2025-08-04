#ifndef TOANKICONNECT_H
#define TOANKICONNECT_H

// 发送 HTTP POST 请求到 AnkiConnect
char *anki_connect_request(const char *action, const char *params);
// 存储媒体文件到 Anki 的媒体库
void store_media_file(const char *filename, const char *path);
// 将文件内容转换为 Base64 编码
char *file_to_base64(const char *file_path);
#endif
