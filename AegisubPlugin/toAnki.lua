local tr = aegisub.gettext

script_name = tr("Anki制卡工具")
script_description = tr("自动制作Anki卡片")
script_author = "Han.Zhao"
script_version = "1.0"

-- 确保目录存在
function ensureDirectoryExists(path)
    if not path then return false end
    local cmd
    if package.config:sub(1,1) == '\\' then -- Windows
        cmd = string.format('mkdir "%s" 2>nul', path)
    else -- macOS/Linux
        cmd = string.format('mkdir -p "%s" 2>/dev/null', path)
    end
    return os.execute(cmd)
end

-- 检查FFmpeg是否可用
function checkFFmpeg()
    local ffmpeg_check = io.popen('ffmpeg -version 2>&1')
    if ffmpeg_check then
        local result = ffmpeg_check:read('*a')
        ffmpeg_check:close()
        return result:find("ffmpeg version") ~= nil
    end
    return false
end

-- 使用FFmpeg提取音频
function extractAudioWithFFmpeg(input_file, output_file, start_time, end_time)
    -- 转换毫秒为秒（FFmpeg需要的时间格式）
    local start_sec = start_time / 1000
    local end_sec = end_time / 1000
    local duration = end_sec - start_sec
    
    -- 构建FFmpeg命令
    local cmd = string.format(
        'ffmpeg -y -ss %.3f -i "%s" -t %.3f -vn -acodec pcm_s16le -ar 44100 -ac 2 "%s"',
        start_sec, input_file, duration, output_file
    )
    
    -- 执行命令
    local result = os.execute(cmd)
    return result == 0 or result == true
end

-- 主处理函数
function process(subtitles, selected_lines, active_line)
    local proj = aegisub.project_properties()
    
    -- 检查是否有选中行
    if #selected_lines == 0 then
        aegisub.debug.out("请至少选择一行字幕！")
        return
    end
    
    -- 获取第一行选中的字幕
    local line = subtitles[selected_lines[1]]
    
    -- 检查视频/音频状态
    local has_video = proj.video_file ~= nil and proj.video_file ~= ""
    local has_audio = proj.audio_file ~= nil and proj.audio_file ~= ""
    
    if not has_video and not has_audio then
        aegisub.debug.out("错误：未找到视频或音频文件！")
        return
    end
    
    -- 检查FFmpeg是否可用
    if not checkFFmpeg() then
        aegisub.debug.out("错误：FFmpeg未安装或不在系统路径中！")
        return
    end
    
    -- 获取Aegisub目录并创建audio_clips子目录
    local aegisub_dir = aegisub.decode_path("?user")
    local output_dir = aegisub_dir .. "/toanki_temp/"
    ensureDirectoryExists(output_dir)
    
    -- 第一步：保存音频
    local audio_path = output_dir .. "aegisub_audio_temp.wav"
    local audio_saved = false
    local input_file = ""
    
    -- 确定输入文件
    if has_video then
        input_file = proj.video_file
    else
        input_file = proj.audio_file
    end
    
    -- 使用FFmpeg提取音频
    if extractAudioWithFFmpeg(input_file, audio_path, line.start_time, line.end_time) then
        audio_saved = true
    else
        aegisub.debug.out("错误：音频提取失败！")
        return
    end
    
    -- 第二步：保存视频截图（如果有视频）
    local screenshot_path = ""
    if has_video then
        screenshot_path = output_dir .. "aegisub_snapshot_temp.png"
        local save_snapshot_time = string.format("%.3f", line.start_time / 1000)
        local save_snapshot_cmd = string.format('ffmpeg -ss %s -i "%s" -vframes 1 -y "%s"',
                                  save_snapshot_time, input_file, screenshot_path)
        
        -- 执行命令
        local ret = os.execute(save_snapshot_cmd)
    end
    
    -- 第三步：获取用户输入 普适性不好 先注释掉
    local user_input = ""
    -- local dialog = {
    --     {class="label", label="请输入文本内容:", x=0, y=0, width=1, height=1},
    --     {class="edit", name="inputText", text="", x=0, y=1, width=10, height=5}
    -- }
    
    -- -- 设置按钮
    -- local buttons = {"确定", "取消"}
    
    -- -- 显示对话框
    -- local pressed, note_diaglog_result = aegisub.dialog.display(dialog, buttons)
    
    -- -- 如果点击确定按钮
    -- if pressed == "确定" then
    --     user_input = note_diaglog_result.inputText
    -- else
    --     aegisub.debug.out("操作已取消\n")
    --     return 
    -- end
    
    -- 第四步：调用外部程序
    local subtitle_text = line.text
    
    -- 获取当前脚本目录
    local exe_path = output_dir .. "toanki.exe"
    
    local par_audio_path = ""
    local par_screenshot_path = ""
    local par_subtitle_text = ""
    local par_user_input = ""

    if audio_path ~= "" then
        par_audio_path = ' -a ' .. '"' .. audio_path .. '"'
    end

    if screenshot_path ~= "" then
        par_screenshot_path = ' -s ' .. '"' .. screenshot_path .. '"'
    end

    if subtitle_text ~= "" then
        par_subtitle_text = ' -m ' .. '"' .. subtitle_text:gsub('"', '""') .. '"'
    end

    if user_input ~= "" then
        par_user_input = ' -n ' .. '"' .. user_input:gsub('"', '""') .. '"'
    end

    -- 构建参数（处理路径中的空格）
    local args = string.format(
        ' %s%s%s%s',
        par_audio_path,
        par_screenshot_path,  -- 确保空值转为空字符串
        par_subtitle_text,
        par_user_input
    )

    -- 执行外部程序
    local cmd = exe_path .. args

    os.execute(cmd)
    
    aegisub.debug.out("制卡完成")
end

-- 注册到Aegisub菜单
aegisub.register_macro(script_name, script_description, process)