## Application Description
Process tool allows you to run applications as a normal user when you are already elevated from another process.

## Command line arguments
- Launch executable: ProcessTool -exepath="C:\Windows\System32\Notepad.exe" [opt: -workpath="C:\Windows\System32"] [opt: -args="Textfile.txt"] [opt: -normal or -admin] [opt: -nouiaccess]
- Launch Windows Store: ProcessTool -uwp="Microsoft.WindowsNotepad_8wekyb3d8bbwe!App" [opt: -args="Textfile.txt"]
- Close: ProcessTool -close -pid=1000 or -pname="Notepad.exe" or -pname="Microsoft.WindowsNotepad_8wekyb3d8bbwe!App"
- Restart: ProcessTool -restart -pid=1000 [opt: -skipargs]
- Show: ProcessTool -show -pid=1000

## Application Features
- Launch, restart, close or show processes.
- Launch Windows Store applications.
- Disable process admin elevation.
- Disable process UI access.

## Support and bug reporting
When you are walking into any problems or a bug you can go to my help page at https://help.arnoldvink.com so I can try to help you out and get everything working.

## Developer donation
If you appreciate my project and want to support me with my projects you can make a donation through https://donation.arnoldvink.com

## Requirements
- Microsoft Visual C++ Redistributable installed.

## Changelog
v1.0.0.0 (-January-2023)
- First official beta test release.