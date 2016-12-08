using System;
using System.Collections.Generic;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
using System.Reflection;
using System.Linq;
using System.IO;

namespace JAOWNT {

	static class Nexus {

		class Node {
			internal Node(Assembly asm) {
				foreach (MethodInfo mi in asm.GetModules().SelectMany(x => x.GetTypes()).SelectMany(x => x.GetMethods()).Where(x => x.IsStatic)) {
					foreach (object attrib in mi.GetCustomAttributes(true).Where(x => x.GetType().IsSubclassOf(typeof(SVA.Event)))) {
						if (attrib.GetType() == typeof(SVA.EventFrame)) {
							if (mi.GetParameters().Length != 1 || mi.GetParameters()[0].ParameterType != typeof(int)) throw new SVException("incorrect parameter signature on method marked with SVA.EventFrame, should be: (int)");
							if (mi.ReturnType != typeof(void)) throw new SVException("incorrect return type on method marked with SVA.EventFrame, should be: void");
							event_frame.Add(mi);
						}
					}
				}
			}

			internal List<MethodInfo> event_frame = new List<MethodInfo>();
		}

		static Dictionary<int, Node> node_table = new Dictionary<int, Node> ();

		static readonly char[] manifestSplit = { '\n' };
		public static int OpenNode(string manifest) {
			string[] source_paths = manifest.Split (manifestSplit, StringSplitOptions.RemoveEmptyEntries);
			List<string> sources = new List<string>();
			foreach (string south_path in source_paths) {
				SV.File source_file = new SV.File ("sharp/" + south_path, SV.File.OpenMode.READ);
				sources.Add(source_file.ReadAllString ());
			}

			CompilerResults c_res;
			CompilerParameters c_param = new CompilerParameters ();
			c_param.GenerateInMemory = true;
			c_param.GenerateExecutable = false;
			c_param.TreatWarningsAsErrors = false;
			c_param.CompilerOptions = "/optimize";
			c_param.ReferencedAssemblies.AddRange (new string[] {"System.dll", Path.Combine(Environment.CurrentDirectory, "sharp", "sharpsv.dll")});
			CSharpCodeProvider c_provider = new CSharpCodeProvider ();
			c_res = c_provider.CompileAssemblyFromSource(c_param, sources.ToArray());
			if (c_res.Output.Count > 0) {
				SV.PrintLine("C# compiler output:");
				foreach(string o in c_res.Output) {
					SV.PrintLine(o);
				}
			}
			if (c_res.Errors.HasErrors) {
				SV.PrintLine("C# compiler errors:");
				foreach(CompilerError o in c_res.Errors) {
					if (o.IsWarning) continue;
					SV.PrintLine(o.ToString());
				}
				throw new SVException("C# Script compilation failed.");
			}

			int asm_hash = c_res.CompiledAssembly.GetHashCode ();
			node_table [asm_hash] = new Node(c_res.CompiledAssembly);

			return asm_hash;
		}

		public static void CloseNode(int id) {
			if (!node_table.Remove(id)) throw new SVException (String.Format("Node ({0}) not found.", id));
		}

		public static void EventFrame(int time) {
			object[] args = new object[1];
			args [0] = time;
			foreach (MethodInfo mi in node_table.Values.SelectMany (x => x.event_frame)) {
				mi.Invoke(null, args); 
			}
		}
	}

}