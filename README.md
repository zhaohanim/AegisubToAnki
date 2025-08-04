# 介绍
此插件可以将视频中选中行的音频、截图、字幕以及用户输入的内容添加到anki中
# 前提
1. 安装ankiconnect
# 用法
1. 将toanki.lua加载进aegisub中
2. 在aegisub安装根目录中新建toanki_temp文件夹
3. 将config.json和toanki.exe放入toanki_temp文件夹
4. 修改config.json :
    deck_name ：牌组名称
    model_name : 模板名称
    Audio ： 音频在模板中的字段
    Snapshot : 截图在模板中的字段
    Meaning : 字幕在模板中的字段
    Note : 笔记在模板中的字段
5. 在aegisub中运行脚本，脚本会自动生成音频文件、截图文件并通过toanki发送到anki中