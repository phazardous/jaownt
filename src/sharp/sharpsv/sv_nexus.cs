using System;

namespace SHARPSV {

	internal static class Nexus {

		internal class Node {
			
		}

		static readonly char[] manifestSplit = { '\n' };
		public static IntPtr CreateNodeFromManifest(string manifest) {
			string f1 = manifest.Split (manifestSplit, StringSplitOptions.RemoveEmptyEntries)[0];
			SVAPI.File f = new SVAPI.File (f1, SVAPI.File.OpenMode.READ);
			string str = new string(System.Text.Encoding.UTF8.GetString(f.Read (f.length)).ToCharArray());
			SVAPI.PrintLine (str);
			f.Dispose ();
			return IntPtr.Zero;
		}

	}

}