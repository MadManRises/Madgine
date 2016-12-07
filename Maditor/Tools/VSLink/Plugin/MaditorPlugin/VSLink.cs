
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using VSCommands;
using ProjectTalker;
using EnvDTE;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace MaditorPlugin
{
    public class VSLink : ProjectTalker.ProjectTalker
    {

        private System.Threading.Thread mWorkerThread;
        private BoolCarrier mRunning;
        private DTE mDTE;
        private AttachPackage mPackage;

        public VSLink(DTE dte, AttachPackage package) :
            base("VSLink", "VSInstance")
        {
            mDTE = dte;
            mPackage = package;
            mRunning = new BoolCarrier();
            mWorkerThread = new System.Threading.Thread(this.Run);
            mWorkerThread.Start();
            send(VSCommand.VSStarted);
        }

        protected override void Dispose(bool b)
        {
            mRunning.bValue = false;
            mWorkerThread.Join();
            base.Dispose(b);
        }

        private void Run()
        {
            mRunning.bValue = true;
            run(mRunning);
        }

        public override void receiveMessage(Int64 type, string arg1, Int64 arg2)
        {
            handle((VSCommand)type, arg1, arg2);
        }

        public bool send(VSCommand cmd, string arg1 = "", Int64 arg2 = -1) {
            return send((int)cmd, arg1, arg2, "Maditor");
        }

        private void handle(VSCommand cmd, string arg1, Int64 arg2)
        {
            switch (cmd)
            {
                case VSCommand.PIDAnswer:
                    attachDebugger(arg2);
                    break;
                case VSCommand.OpenSolution:
                    openSolution(arg1);
                    break;
                case VSCommand.OpenSourceFile:
                    openFile(arg1, arg2);
                    break;
                default:
                    VsShellUtilities.ShowMessageBox(
                    mPackage,
                    "Unhandled Command Received",
                    String.Format("Type: {0}, Args: {1}, {2}", cmd.ToString(), arg1, arg2),
                    OLEMSGICON.OLEMSGICON_CRITICAL,
                    OLEMSGBUTTON.OLEMSGBUTTON_OK,
                    OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
                    break;
            }
        }

        private void openSolution(string path)
        {
            mDTE.Solution.Open(path);
        }

        private void openFile(string path, Int64 lineNr)
        {
            Window w = mDTE.ItemOperations.OpenFile(path);
            if (lineNr != -1)
                ((TextSelection)mDTE.ActiveDocument.Selection).GotoLine((int)lineNr);
        }

        private bool attachDebugger(Int64 pid)
        {
            Processes processes = mDTE.Debugger.LocalProcesses;
            bool done = false;
            foreach (Process p in processes)
            {
                if (p.ProcessID == pid)
                {
                    try
                    {
                        p.Attach();
                        done = true;
                    }
                    catch (COMException e) {}
                    break;
                }
            }
            return done;
        }


        internal bool requestPID()
        {
            return send(VSCommand.PIDRequest);
        }
    }
}
