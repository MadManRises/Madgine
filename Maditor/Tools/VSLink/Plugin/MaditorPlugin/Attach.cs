//------------------------------------------------------------------------------
// <copyright file="Attach.cs" company="Company">
//     Copyright (c) Company.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.ComponentModel.Design;
using System.Globalization;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using VSCommands;
using System.Collections.Generic;

namespace MaditorPlugin
{
    /// <summary>
    /// Command handler
    /// </summary>
    internal sealed class Attach
    {
        /// <summary>
        /// Command ID.
        /// </summary>
        public const int EnableId = 0x0100;
        public const int AttachId = 0x0101;

        private OleMenuCommand enableCommand;

        /// <summary>
        /// Command menu group (command set GUID).
        /// </summary>
        public static readonly Guid CommandSet = new Guid("2b252388-a441-4416-aadc-8e4d089d6508");

        /// <summary>
        /// VS Package that provides this command, not null.
        /// </summary>
        private readonly AttachPackage package;

        /// <summary>
        /// Initializes a new instance of the <see cref="Attach"/> class.
        /// Adds our command handlers for menu (commands must exist in the command table file)
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        private Attach(AttachPackage package)
        {
            if (package == null)
            {
                throw new ArgumentNullException("package");
            }

            this.package = package;

            OleMenuCommandService commandService = this.ServiceProvider.GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if (commandService != null)
            {
                var enableCommandID = new CommandID(CommandSet, EnableId);
                enableCommand = new OleMenuCommand(this.Enable, enableCommandID);                
                commandService.AddCommand(enableCommand);

                var attachCommandID = new CommandID(CommandSet, AttachId);
                var attachItem = new OleMenuCommand(this.AttachToMaditor, attachCommandID);
                attachItem.BeforeQueryStatus += QueryDefaultButtonStatus;
                commandService.AddCommand(attachItem);
            }
        }

        private void QueryDefaultButtonStatus(object sender, EventArgs e)
        {
            ((OleMenuCommand)sender).Enabled = package.enabled;
        }

        /// <summary>
        /// Gets the instance of the command.
        /// </summary>
        public static Attach Instance
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the service provider from the owner package.
        /// </summary>
        private IServiceProvider ServiceProvider
        {
            get
            {
                return this.package;
            }
        }

        /// <summary>
        /// Initializes the singleton instance of the command.
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        public static void Initialize(AttachPackage package)
        {
            Instance = new Attach(package);
        }

        /// <summary>
        /// This function is the callback used to execute the command when the menu item is clicked.
        /// See the constructor to see how the menu item is associated with this function using
        /// OleMenuCommandService service and MenuCommand class.
        /// </summary>
        /// <param name="sender">Event sender.</param>
        /// <param name="e">Event args.</param>
        private void AttachToMaditor(object sender, EventArgs e)
        {

            if (!package.talker.requestPID())
            {
                // Show a message box to prove we were here
                VsShellUtilities.ShowMessageBox(
                    this.ServiceProvider,
                    "Maditor not opened",
                    "Attach",
                    OLEMSGICON.OLEMSGICON_CRITICAL,
                    OLEMSGBUTTON.OLEMSGBUTTON_OK,
                    OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
            }

        }

        private void Enable(object sender, EventArgs e)
        {
            package.Enable();
            enableCommand.Enabled = false;
        }

    }
}
