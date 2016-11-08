//------------------------------------------------------------------------------
// <copyright file="AttachPackage.cs" company="Company">
//     Copyright (c) Company.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace MaditorPlugin
{
    /// <summary>
    /// This is the class that implements the package exposed by this assembly.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The minimum requirement for a class to be considered a valid package for Visual Studio
    /// is to implement the IVsPackage interface and register itself with the shell.
    /// This package uses the helper classes defined inside the Managed Package Framework (MPF)
    /// to do it: it derives from the Package class that provides the implementation of the
    /// IVsPackage interface and uses the registration attributes defined in the framework to
    /// register itself and its components with the shell. These attributes tell the pkgdef creation
    /// utility what data to put into .pkgdef file.
    /// </para>
    /// <para>
    /// To get loaded into VS, the package must be referred by &lt;Asset Type="Microsoft.VisualStudio.VsPackage" ...&gt; in .vsixmanifest file.
    /// </para>
    /// </remarks>
    [PackageRegistration(UseManagedResourcesOnly = true)]
    [InstalledProductRegistration("#110", "#112", "1.0", IconResourceID = 400)] // Info on this package for Help/About
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [Guid(AttachPackage.PackageGuidString)]
    [ProvideAutoLoad(UIContextGuids.NoSolution)]
    [SuppressMessage("StyleCop.CSharp.DocumentationRules", "SA1650:ElementDocumentationMustBeSpelledCorrectly", Justification = "pkgdef, VS and vsixmanifest are valid VS terms")]
    public sealed class AttachPackage : Package
    {
        /// <summary>
        /// AttachPackage GUID string.
        /// </summary>
        public const string PackageGuidString = "a592a018-4881-4daa-9707-a359e29c4376";

        /// <summary>
        /// Initializes a new instance of the <see cref="Attach"/> class.
        /// </summary>
        public AttachPackage()
        {
            // Inside this method you can place any initialization code that does not require
            // any Visual Studio service because at this point the package object is created but
            // not sited yet inside Visual Studio environment. The place to do all the other
            // initialization is the Initialize method.

        }

        public VSLink talker {
            get{
                return mTalker;
            }
        }

        public bool enabled
        {
            get
            {
                return !Object.ReferenceEquals(mTalker, null);
            }
        }

        #region Package Members

        private VSLink mTalker;

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put all the initialization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Attach.Initialize(this);
            base.Initialize();
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
            mTalker.Dispose();
        }

        internal void Enable()
        {
            EnvDTE.DTE dte = this.GetService(typeof(Microsoft.VisualStudio.Shell.Interop.SDTE)) as EnvDTE.DTE;
            mTalker = new VSLink(dte, this);
        }

        #endregion
    }
}
