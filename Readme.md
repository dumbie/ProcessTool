## Application Description
Process tool allows you to run applications as a normal user when you are already elevated from another process.

## Command line arguments
Process tool usage: ProcessTool [opt: -normal or -admin] [opt: -nouiaccess] [req: -exepath="notepad.exe"] [opt: -workpath="WorkingDirectory"] [opt: -args="LaunchArguments"]
- Launch: ProcessTool -exepath="C:\\Windows\\System32\\Notepad.exe" -workpath="C:\\Windows\\System32" -args="Textfile.txt"
- Launch: ProcessTool -uwp="Microsoft.WindowsCalculator_8wekyb3d8bbwe!App"
- Restart: ProcessTool -restart -pid=1000 or -pname="notepad"
- Close: ProcessTool -close -pid=1000 or -pname="notepad"

## Application Features
- Launch, restart or close processes.
- Launch Windows Store applications.
- Disable process elevation.
- Disable UI access.

## Support and bug reporting
When you are walking into any problems or a bug you can go to my help page at https://help.arnoldvink.com so I can try to help you out and get everything working.

## Developer donation
If you appreciate my project and want to support me with my projects you can make a donation through https://donation.arnoldvink.com

## Requirements
- Microsoft Visual C++ Redistributable installed.

## Changelog
v1.0.0.0 (-January-2023)
- First official beta test release.