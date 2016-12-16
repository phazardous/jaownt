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
					foreach (object attrib in mi.GetCustomAttributes(true).Where(x => x.GetType().IsSubclassOf(typeof(GA.Event)))) {
						if (attrib.GetType() == typeof(GA.EventInit)) {
							if (mi.GetParameters().Length != 0) 
								throw new SVException("incorrect parameter signature on method marked with GA.EventInit, should have no parameters");
							if (mi.ReturnType != typeof(void)) 
								throw new SVException("incorrect return type on method marked with GA.EventInit, should be: void");
							event_init.Add(mi);
						} else if (attrib.GetType() == typeof(GA.EventShutdown)) {
							if (mi.GetParameters().Length != 0) 
								throw new SVException("incorrect parameter signature on method marked with GA.EventShutdown, should have no parameters");
							if (mi.ReturnType != typeof(void)) 
								throw new SVException("incorrect return type on method marked with GA.EventShutdown, should be: void");
							event_shutdown.Add(mi);
						} else if (attrib.GetType() == typeof(GA.EventFrame)) {
							if (mi.GetParameters().Length != 1 || mi.GetParameters()[0].ParameterType != typeof(int)) 
								throw new SVException("incorrect parameter signature on method marked with GA.EventFrame, should be: (int)");
							if (mi.ReturnType != typeof(void)) 
								throw new SVException("incorrect return type on method marked with GA.EventFrame, should be: void");
							event_frame.Add(mi);
						}  else if (attrib.GetType() == typeof(GA.EventChat)) {
							if (mi.GetParameters().Length != 2 || mi.GetParameters()[0].ParameterType != typeof(G.Player) || mi.GetParameters()[1].ParameterType != typeof(string)) 
								throw new SVException("incorrect parameter signature on method marked with GA.EventChat, should be: (G.Player, string)");
							if (mi.ReturnType != typeof(void)) 
								throw new SVException("incorrect return type on method marked with GA.EventChat, should be: void");
							event_chat.Add(mi);
						}  else if (attrib.GetType() == typeof(GA.EventCmdAll)) {
							if (mi.GetParameters().Length != 2 || mi.GetParameters()[0].ParameterType != typeof(G.Entity) || mi.GetParameters()[1].ParameterType != typeof(string)) 
								throw new SVException("incorrect parameter signature on method marked with GA.EventCmdAll, should be: (G.Entity, string)");
							if (mi.ReturnType != typeof(void)) 
								throw new SVException("incorrect return type on method marked with GA.EventCmdAll, should be: void");
							event_cmd_all.Add(mi);
						}  else if (attrib.GetType() == typeof(GA.EventCmd)) {
							if (mi.GetParameters().Length != 1 || mi.GetParameters()[0].ParameterType != typeof(G.Entity)) 
								throw new SVException("incorrect parameter signature on method marked with GA.EventCmd, should be: (G.Entity)");
							if (mi.ReturnType != typeof(void)) 
								throw new SVException("incorrect return type on method marked with GA.EventCmd, should be: void");
							string cmd = ((GA.EventCmd)attrib).cmd;
							if (!event_cmd.ContainsKey(cmd)) event_cmd[cmd] = new List<MethodInfo>();
							event_cmd[cmd].Add(mi);
						}
					}
				}
				foreach (MethodInfo mi in event_init) {
					mi.Invoke(null, new object [0]);
				}
			}

			internal List<MethodInfo> event_init = new List<MethodInfo>();
			internal List<MethodInfo> event_shutdown = new List<MethodInfo>();
			internal List<MethodInfo> event_frame = new List<MethodInfo>();
			internal List<MethodInfo> event_chat = new List<MethodInfo>();

			internal List<MethodInfo> event_cmd_all = new List<MethodInfo>();
			internal Dictionary<string, List<MethodInfo>> event_cmd = new Dictionary<string, List<MethodInfo>>();
		}

		readonly static Dictionary<int, Node> node_table = new Dictionary<int, Node> ();

		internal static void Reset() {
			foreach (MethodInfo mi in node_table.Values.SelectMany(x => x.event_shutdown)) {
				mi.Invoke (null, new Object [0]);
			}
			node_table.Clear ();
		}

		static readonly char[] manifestSplit = { '\n' };
		internal static int OpenNode(string manifest) {
			string[] source_paths = manifest.Split (manifestSplit, StringSplitOptions.RemoveEmptyEntries);
			List<string> sources = new List<string>();
			foreach (string south_path in source_paths) {
				G.File source_file = new G.File ("sharp/" + south_path, G.File.OpenMode.READ);
				sources.Add(source_file.ReadAllString ());
				source_file.Dispose ();
			}

			CompilerResults c_res;
			CompilerParameters c_param = new CompilerParameters ();
			c_param.GenerateInMemory = true;
			c_param.GenerateExecutable = false;
			c_param.TreatWarningsAsErrors = false;
			c_param.CompilerOptions = "/optimize";
			c_param.ReferencedAssemblies.AddRange (new string[] {"System.dll", "System.Core.dll", Path.Combine(Environment.CurrentDirectory, "sharp", "sharpsv.dll")});
			CSharpCodeProvider c_provider = new CSharpCodeProvider ();
			c_res = c_provider.CompileAssemblyFromSource(c_param, sources.ToArray());
			if (c_res.Output.Count > 0) {
				G.PrintLine("C# compiler output:");
				foreach(string o in c_res.Output) {
					G.PrintLine(o);
				}
			}
			if (c_res.Errors.HasErrors) {
				G.PrintLine("C# compiler errors:");
				foreach(CompilerError o in c_res.Errors) {
					if (o.IsWarning) continue;
					G.PrintLine(o.ToString());
				}
				throw new SVException("C# Script compilation failed.");
			}

			int asm_hash = c_res.CompiledAssembly.GetHashCode ();
			node_table [asm_hash] = new Node(c_res.CompiledAssembly);

			return asm_hash;
		}

		internal static void CloseNode(int id) {
			if (!node_table.ContainsKey (id)) throw new SVException (String.Format("Node ({0}) not found.", id));
			Node n = node_table [id];
			node_table.Remove (id);
			foreach (MethodInfo mi in n.event_shutdown) {
				mi.Invoke(null, new object [0]);
			}
		}

		internal static void CloseAll() {
			foreach (MethodInfo mi in node_table.Values.SelectMany(x => x.event_shutdown)) {
				mi.Invoke(null, new object [0]);
			}
			node_table.Clear ();
		}

		internal static void EventFrame(int time) {
			object[] args = new object[1];
			args [0] = time;
			foreach (MethodInfo mi in node_table.Values.SelectMany (x => x.event_frame)) {
				mi.Invoke(null, args); 
			}
		}

		internal static void EventChat(G.Player ent, string message) {
			object[] args = new object[2];
			args [0] = ent;
			args [1] = message;
			foreach (MethodInfo mi in node_table.Values.SelectMany (x => x.event_chat)) {
				mi.Invoke(null, args); 
			}
		}

		internal static void EventCmd(G.Entity ent, string cmd) {
			object[] args = new object[2];
			args [0] = ent;
			args [1] = cmd;
			foreach (MethodInfo mi in node_table.Values.SelectMany (x => x.event_cmd_all)) {
				mi.Invoke(null, args); 
			}
			args = new object[1];
			args [0] = ent;
			foreach (MethodInfo mi in node_table.Values.Where(x => x.event_cmd.ContainsKey(cmd)).SelectMany(x => x.event_cmd[cmd])) {
				mi.Invoke(null, args); 
			}
		}
	}

}