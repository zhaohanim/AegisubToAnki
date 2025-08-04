#ifndef FILETOANKICARD_H
#define FILETOANKICARD_H

typedef enum _tag_cardType_t
{
    cardType_audio = 0,
    cardType_video
}cardType_t;

//设置文件map
void setfieldsMap(char *deck_name,char *model_name,char *audio_field,char *picture_field,char *meaning_field,char* notes_field);
//将文件写入Anki
void fileToAnkiCard(char *audio_path, char *picture_path, char *Meaning,char* Notes);

#endif
