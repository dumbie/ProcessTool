using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessTool
{
    public partial class ProcessTool
    {
        private static int Main(string[] args)
        {
            try
            {
                Console.WriteLine("Welcome to the process tool.");

                //Check launch arguments
                if (args.Count() < 2)
                {
                    Console.WriteLine("Command line arguments:");

                    Console_WaitForInput();
                    return 0;
                }

                //Filter launch arguments

                //Wait for user key input
                Console_WaitForInput();
            }
            catch { }
            return 0;
        }

        private static void Console_WaitForInput()
        {
            try
            {
                Console.WriteLine("\nPress any key to close the process tool . . .");
                Console.ReadKey();
            }
            catch { }
        }
    }
}