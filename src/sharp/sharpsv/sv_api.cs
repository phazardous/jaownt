using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Linq;

namespace JAOWNT {

	public class SVException : Exception {
		public SVException(string message) : base(message) {}
	}

	public static class G_INTERNAL {

		public abstract class UnmanagedGameResource : IDisposable {
			protected abstract void Cleanup ();
			protected bool disposed = false;
			protected readonly bool manage;

			public UnmanagedGameResource(bool manage) {
				this.manage = manage;
			}

			~UnmanagedGameResource() {
				if (manage && !disposed) G.PrintLine(String.Format("DANGER: A script has leaked a server resource: ({0}), FIX IT. You MUST call Dispose() on Unmanaged resources that YOU create. (Files and Entities)", this.GetType()));
			}

			public void Dispose() {
				if (!disposed && manage) {
					disposed = true;
					Cleanup ();
					GC.SuppressFinalize (this);
				}
			}
		}

		// ENTITY + NPC + PLAYER

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr G_CreateEntity();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void G_DestroyEntity(IntPtr handle);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern bool G_IsPlayer(IntPtr handle);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern bool G_IsNPC(IntPtr handle);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr G_Activator(IntPtr ent);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr G_Other(IntPtr ent);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern string G_Target (IntPtr ent);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern string G_Targetname(IntPtr ent);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr G_Origin(IntPtr ent);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void G_NPC_Kill(IntPtr ent);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr G_EntFromPlayer(int clinum);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern int G_PlayerFromEnt(IntPtr handle);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern string G_PlayerName(int clinum);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr G_SpawnNPC (string type, IntPtr origin, float yaw, int health);

		// FILE

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr FS_OpenR(string str);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr FS_OpenW(string str);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void FS_Close(IntPtr fh);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern int FS_Length(IntPtr fh);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern int FS_Read(IntPtr fh, IntPtr buf, int len);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern int FS_Write(IntPtr fh, IntPtr buf, int len);

		// PRINT

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void G_PrintStr(string str);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void G_ChatAll(string name, string msg);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void G_ChatTo(string name, string msg, int clinum);

		// SEARCH

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr[] G_FindEntitiesBySharpTag (string tag);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern IntPtr[] G_FindEntitiesBySharpTargetname (string name);

		// ================

		public static int Load_Manifest(string manifest) {
			return Nexus.OpenNode (manifest);
		}

		public static void Close_Manifest(int manifest_id) {
			Nexus.CloseNode (manifest_id);
		}

		public static void Event_Shutdown() {
			Nexus.CloseAll ();
		}

		public static void Event_Frame(int time) {
			Nexus.EventFrame (time);
		}

		public static void Event_Chat(int clinum, string message) {
			Nexus.EventChat (new G.Player(clinum), message);
		}

		public static void Event_Cmd(IntPtr ent_ptr, string cmd) {
			Nexus.EventCmd (new G.Entity(ent_ptr), cmd);
		}
	}

	public static class G {

		// ======================== //
		// -------- ENTITY -------- //
		// ======================== //

		public class Entity : G_INTERNAL.UnmanagedGameResource {

			internal Entity(IntPtr ent_ptr) : base(false) {
				handle = ent_ptr;
				if (G_INTERNAL.G_IsPlayer (handle)) {
					player = new Player(this);
				}
				if (G_INTERNAL.G_IsNPC (handle)) {
					npc = new NPC(this);
				}
			}

			internal Entity (Player pl) : base(false) {
				handle = G_INTERNAL.G_EntFromPlayer(pl.clinum);
				player = pl;
			}

			public Entity() : base(true) {
				handle = G_INTERNAL.G_CreateEntity();
			}

			public Entity[] RetrieveTargets() {
				return G.FindEntitiesByTargetname (this.target);
			}

			public Entity activator { get { return new Entity (G_INTERNAL.G_Activator (handle)); } }
			public Entity other { get { return new Entity (G_INTERNAL.G_Other (handle)); } }
			public string target { get { return G_INTERNAL.G_Target (handle); } }
			public string targetname { get { return G_INTERNAL.G_Targetname (handle); } }
			public Vec3 origin { get { return new Vec3(G_INTERNAL.G_Origin (handle)); } }

			protected override void Cleanup () {
				G_INTERNAL.G_DestroyEntity (handle);
			}

			internal readonly IntPtr handle;
			public readonly Player player; // this stays null if entity is not a player
			public readonly NPC npc; // this stays null if entity is not an NPC
		}

		public class NPC {
			
			internal NPC (Entity ent) {
				entity = ent;
			}

			public void Kill() {
				G_INTERNAL.G_NPC_Kill (entity.handle);
			}

			public readonly Entity entity;
		}

		public class Player {
			
			internal Player(int num) {
				clinum = num;
				entity = new Entity(this);
			}

			internal Player(Entity ent) {
				clinum = G_INTERNAL.G_PlayerFromEnt(ent.handle);
				entity = ent;
			}

			public string name {
				get { return  G_INTERNAL.G_PlayerName (clinum);}
			}

			public readonly int clinum;
			public readonly Entity entity;
		}

		public static Entity SpawnNPC(string type, Vec3 origin, float yaw = 0, int health = 0) {
			return new Entity ( G_INTERNAL.G_SpawnNPC( type, origin.valp, yaw, health ));
		}

		// ====================== //
		// -------- FILE -------- //
		// ====================== //

		public class File : G_INTERNAL.UnmanagedGameResource {

			public enum OpenMode {
				READ,
				WRITE,
			}

			public File(string path, OpenMode om) : base(true) {
				switch (om) {
				case OpenMode.READ:
					file_handle = G_INTERNAL.FS_OpenR(path);
					break;
				case OpenMode.WRITE:
					file_handle = G_INTERNAL.FS_OpenW(path);
					break;
				}
				openMode = om;
				if (file_handle == IntPtr.Zero) {
					disposed = true;
					throw new SVException("file not found");
				}
				length = G_INTERNAL.FS_Length(file_handle);
			}

			public byte[] Read (int len) {
				if (openMode != OpenMode.READ) throw new SVException("read operation not allowed");
				IntPtr data_ptr = Marshal.AllocHGlobal (len);
				int alen = G_INTERNAL.FS_Read (file_handle, data_ptr, len);
				byte[] data = new byte[alen];
				Marshal.Copy (data_ptr, data, 0, alen);
				Marshal.FreeHGlobal (data_ptr);
				return data;
			}

			public byte[] ReadAll() {
				return Read (length);
			}

			public string ReadAllString() {
				return System.Text.Encoding.UTF8.GetString(ReadAll());
			}

			public int Write (byte[] data) {
				if (openMode != OpenMode.WRITE) throw new SVException("write operation not allowed");
				IntPtr data_ptr = Marshal.AllocHGlobal (data.Length);
				Marshal.Copy (data, 0, data_ptr, data.Length);
				int len = G_INTERNAL.FS_Write (file_handle, data_ptr, data.Length);
				Marshal.FreeHGlobal (data_ptr);
				return len;
			}

			protected override void Cleanup() {
				if (file_handle != IntPtr.Zero) G_INTERNAL.FS_Close (file_handle);
				file_handle = IntPtr.Zero;
			}

			private IntPtr file_handle;

			public int length { get; private set; }
			public OpenMode openMode { get; private set; }
		}

		// ======================= //
		// -------- PRINT -------- //
		// ======================= //

		public static void Print<T>(T v) {
			G_INTERNAL.G_PrintStr(v.ToString());
		}
		public static void PrintLine<T>(T v) {
			G_INTERNAL.G_PrintStr(v.ToString() + "\n");
		}

		public static void Chat(string name, string message) {
			G_INTERNAL.G_ChatAll (name, message);
		}

		public static void Chat(string name, string message, Player recipient) {
			G_INTERNAL.G_ChatTo (name, message, recipient.clinum);
		}

		// ======================== //
		// -------- SEARCH -------- //
		// ======================== //

		public static Entity[] FindEntitiesByTag(string tag) {
			IntPtr[] ent_ptrs = G_INTERNAL.G_FindEntitiesBySharpTag (tag);
			Entity[] ents = ent_ptrs.Select (x => new Entity (x)).ToArray();
			return ents;
		}

		public static Entity[] FindEntitiesByTargetname(string name) {
			IntPtr[] ent_ptrs = G_INTERNAL.G_FindEntitiesBySharpTargetname (name);
			Entity[] ents = ent_ptrs.Select (x => new Entity (x)).ToArray();
			return ents;
		}

		// ========================= //
		// -------- UTILITY -------- //
		// ========================= //

		public class Vec3 {
			
			public Vec3(float X, float Y, float Z) {
				valp = Marshal.AllocHGlobal(sizeof(float) * 3);
				val[0] = X;
				val[1] = Y;
				val[2] = Z;
				UpdatePtr();
			}

			internal Vec3(IntPtr ptr) {
				valp = Marshal.AllocHGlobal(sizeof(float) * 3);
				Marshal.Copy(ptr, val, 0, 3);
				UpdatePtr();
			}

			~Vec3() {
				Marshal.FreeHGlobal(valp);
			}

			void UpdatePtr() {
				Marshal.Copy(val, 0, valp, 3);
			}

			public float X { get { return val [0]; } set { val [0] = value; UpdatePtr(); } }
			public float Y { get { return val [1]; } set { val [1] = value; UpdatePtr(); } }
			public float Z { get { return val [2]; } set { val [2] = value; UpdatePtr(); } }

			internal float[] val = new float[3];
			internal IntPtr valp;
		}
	}

	public static class GA {

		[AttributeUsage(AttributeTargets.Method)]
		public abstract class Event : Attribute {}
		[AttributeUsage(AttributeTargets.Method)]
		public class EventInit : Event {}
		[AttributeUsage(AttributeTargets.Method)]
		public class EventShutdown : Event {}
		[AttributeUsage(AttributeTargets.Method)]
		public class EventFrame : Event {}
		[AttributeUsage(AttributeTargets.Method)]
		public class EventChat : Event {}
		[AttributeUsage(AttributeTargets.Method)]
		public class EventCmd : Event { public readonly string cmd; public EventCmd(string cmd) {this.cmd = cmd;} }
		public class EventCmdAll : Event {}
	}
}