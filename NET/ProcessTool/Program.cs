using ArnoldVinkCode;
using System;
using System.Linq;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVProcess;

namespace ProcessTool
{
    public partial class ProcessTool
    {
        private static async Task<int> Main(string[] args)
        {
            try
            {
                AVDebug.WriteLine("Welcome to the process tool.");

                //Check launch arguments
                if (args.Count() < 1)
                {
                    AVDebug.WriteLine("\nCommand line arguments:");
                    AVDebug.WriteLine("Launch executable: ProcessTool -exepath=\"C:\\Windows\\System32\\Notepad.exe\" [opt: -workpath=\"C:\\Windows\\System32\"] [opt: -args=\"Textfile.txt\"] [opt: -normal or -admin] [opt: -allowuiaccess]");
                    AVDebug.WriteLine("Launch Windows Store: ProcessTool -uwp=\"Microsoft.WindowsNotepad_8wekyb3d8bbwe!App\" [opt: -args=\"Textfile.txt\"]");
                    AVDebug.WriteLine("Close: ProcessTool -close -pid=1000 or -hwnd=1000 or -pname=\"Notepad.exe\" or -uwp=\"Microsoft.WindowsNotepad_8wekyb3d8bbwe!App\"");
                    AVDebug.WriteLine("Restart: ProcessTool -restart -pid=1000 [opt: -withoutargs] [opt: -args=\"Textfile.txt\"]");
                    AVDebug.WriteLine("Show: ProcessTool -show -pid=1000 or -hwnd=1000");
                    Console_WaitForInput();
                    return 0;
                }

                //Filter launch arguments
                string argumentUWPAppUserModelId = string.Empty;
                string argumentExePath = string.Empty;
                string argumentWorkPath = string.Empty;
                string argumentArgs = string.Empty;
                string argumentpId = string.Empty;
                string argumentpName = string.Empty;
                string argumentHWND = string.Empty;
                bool argumentUWP = false;
                bool argumentNormal = false;
                bool argumentAdmin = false;
                bool argumentAllowUiAccess = false;
                bool argumentClose = false;
                bool argumentShow = false;
                bool argumentRestart = false;
                bool argumentWithoutArgs = false;
                bool argumentWait = false;
                foreach (string argumentRaw in args)
                {
                    try
                    {
                        string argument = argumentRaw.Trim();
                        string argumentLower = argument.ToLower();
                        AVDebug.WriteLine("Tool argument: " + argument);

                        if (argumentLower.StartsWith("-exepath="))
                        {
                            int cutLength = "-exepath=".Length;
                            argumentExePath = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-workpath="))
                        {
                            int cutLength = "-workpath=".Length;
                            argumentWorkPath = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-args="))
                        {
                            int cutLength = "-args=".Length;
                            argumentArgs = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-pid="))
                        {
                            int cutLength = "-pid=".Length;
                            argumentpId = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-pname="))
                        {
                            int cutLength = "-pname=".Length;
                            argumentpName = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-hwnd="))
                        {
                            int cutLength = "-hwnd=".Length;
                            argumentHWND = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-uwp="))
                        {
                            argumentUWP = true;
                            int cutLength = "-uwp=".Length;
                            argumentUWPAppUserModelId = argument.Substring(cutLength);
                        }

                        else if (argumentLower.StartsWith("-normal"))
                        {
                            if (!argumentAdmin)
                            {
                                argumentNormal = true;
                            }
                        }

                        else if (argumentLower.StartsWith("-admin"))
                        {
                            if (!argumentNormal)
                            {
                                argumentAdmin = true;
                            }
                        }

                        else if (argumentLower.StartsWith("-allowuiaccess"))
                        {
                            argumentAllowUiAccess = true;
                        }

                        else if (argumentLower.StartsWith("-close"))
                        {
                            argumentClose = true;
                        }

                        else if (argumentLower.StartsWith("-show"))
                        {
                            argumentShow = true;
                        }

                        else if (argumentLower.StartsWith("-restart"))
                        {
                            argumentRestart = true;
                        }

                        else if (argumentLower.StartsWith("-withoutargs"))
                        {
                            argumentWithoutArgs = true;
                        }

                        else if (argumentLower.StartsWith("-wait"))
                        {
                            argumentWait = true;
                        }
                    }
                    catch { }
                }

                //Check launch arguments
                int returnValue = 0;
                if (argumentClose)
                {
                    if (argumentUWP)
                    {
                        if (string.IsNullOrWhiteSpace(argumentUWPAppUserModelId))
                        {
                            AVDebug.WriteLine("Close Windows application user model id not set.");
                        }
                        else
                        {
                            returnValue = Close_ProcessesByAppUserModelId(argumentUWPAppUserModelId) ? 1 : 0;
                        }
                    }
                    else
                    {
                        if (!string.IsNullOrWhiteSpace(argumentpId))
                        {
                            returnValue = Close_ProcessTreeById(Convert.ToInt32(argumentpId)) ? 1 : 0;
                        }
                        else if (!string.IsNullOrWhiteSpace(argumentpName))
                        {
                            returnValue = Close_ProcessesByName(argumentpName, true) ? 1 : 0;
                        }
                        else if (!string.IsNullOrWhiteSpace(argumentHWND))
                        {
                            returnValue = Close_ProcessByWindowMessage(new IntPtr(Convert.ToInt32(argumentHWND))) ? 1 : 0;
                        }
                    }
                }
                else if (argumentRestart)
                {
                    if (!string.IsNullOrWhiteSpace(argumentpId))
                    {
                        returnValue = await Restart_ProcessById(Convert.ToInt32(argumentpId), argumentArgs, argumentWithoutArgs);
                    }
                }
                else if (argumentShow)
                {
                    if (!string.IsNullOrWhiteSpace(argumentpId) && !string.IsNullOrWhiteSpace(argumentHWND))
                    {
                        returnValue = await Show_ProcessIdAndWindowHandle(Convert.ToInt32(argumentpId), new IntPtr(Convert.ToInt32(argumentHWND))) ? 1 : 0;
                    }
                    else if (!string.IsNullOrWhiteSpace(argumentpId))
                    {
                        returnValue = await Show_ProcessById(Convert.ToInt32(argumentpId)) ? 1 : 0;
                    }
                    else if (!string.IsNullOrWhiteSpace(argumentHWND))
                    {
                        returnValue = await Show_ProcessByWindowHandle(new IntPtr(Convert.ToInt32(argumentHWND))) ? 1 : 0;
                    }
                }
                else
                {
                    if (argumentUWP)
                    {
                        if (string.IsNullOrWhiteSpace(argumentUWPAppUserModelId))
                        {
                            AVDebug.WriteLine("Launch Windows application user model id not set.");
                        }
                        else
                        {
                            returnValue = Launch_UwpApplication(argumentUWPAppUserModelId, argumentArgs);
                        }
                    }
                    else
                    {
                        if (string.IsNullOrWhiteSpace(argumentExePath))
                        {
                            AVDebug.WriteLine("Launch executable path not set.");
                        }
                        else
                        {
                            returnValue = Launch_Prepare(argumentExePath, argumentWorkPath, argumentArgs, argumentNormal, argumentAdmin, argumentAllowUiAccess);
                        }
                    }
                }

                //Wait for user key input
                if (argumentWait)
                {
                    Console_WaitForInput();
                }

                //Return result value
                return returnValue;
            }
            catch (Exception ex)
            {
                AVDebug.WriteLine("Process tool failed: " + ex.Message);
                return 0;
            }
        }

        private static void Console_WaitForInput()
        {
            try
            {
                AVDebug.WriteLine("\nPress any key to close the process tool . . .");
                Console.ReadKey();
            }
            catch { }
        }
    }
}