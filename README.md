# LiveStreamingPlatform
Create a live streaming platform with Qt, FFmpeg and RTMP
## Establish a RTMP server (Win10)
### Requirement
 - [ffmpeg](https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full-shared.7z)
 - [nginx 1.7.11.3 Gryphon](http://nginx-win.ecsds.eu/download/)
 - [nginx-rtmp-module](https://github.com/arut/nginx-rtmp-module)
### Setup
#### ffmpeg
 - Download ffmpeg
 - Unzip folder
 - Set the path of ffmpeg.exe to evironment variable
#### nginx
 - Download nginx 1.7.11.3 Gryphon
 - Clone nginx-rtmp-module to nginx 1.7.11.3 Gryphon (nginx 1.7.11.3 Gryphon\nginx-rtmp-module)
 - Config nginx-win.conf
 - Start nginx 
```
nginx.exe -c conf\nginx-win.conf
```
 - Test
   - Prepare a video named "test.mp4"
   - Streaming test
```
ffmpeg.exe -re -i .\test.mp4 -vcodec libx264 -acodec aac -f flv rtmp://127.0.0.1:1935/live/home
```

- Open player and input URL: rtmp://127.0.0.1:1935/live/home 


## Live Streaming Platform Development
### Todo
- VedioCapture 
- AudioRecord
- Codec
- Upstreaming
- Beautify
- UI