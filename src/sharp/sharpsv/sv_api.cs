using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;



namespace JAOWNT {

	public class SVException : Exception {
		public SVException(string message) : base(message) {}
	}

	public static class SVI {

		public abstract class UnmanagedGameResource : IDisposable {
			protected abstract void Cleanup ();
			private bool disposed = false;

			~UnmanagedGameResource() {
				if (!disposed) Dispose();
			}

			public void Dispose() {
				disposed = true;
				Cleanup ();
				GC.SuppressFinalize (this);
			}
		}

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

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void PrintStr(string str);

		public static int Load_Manifest(string manifest) {
			return Nexus.OpenNode (manifest);
		}

		public static void Event_Frame(int time) {
			Nexus.EventFrame (time);
		}
	}

	public static class SV {

		// ====================== //
		// -------- FILE -------- //
		// ====================== //

		public class File : SVI.UnmanagedGameResource {

			public enum OpenMode {
				READ,
				WRITE,
			}
			
			public File(string path, OpenMode om) {
				switch (om) {
				case OpenMode.READ:
					file_handle = SVI.FS_OpenR(path);
					break;
				case OpenMode.WRITE:
					file_handle = SVI.FS_OpenW(path);
					break;
				}
				openMode = om;
				if (file_handle == IntPtr.Zero) throw new SVException("file not found");
				length = SVI.FS_Length(file_handle);
			}

			public byte[] Read (int len) {
				if (openMode != OpenMode.READ) throw new SVException("read operation not allowed");
				IntPtr data_ptr = Marshal.AllocHGlobal (len);
				int alen = SVI.FS_Read (file_handle, data_ptr, len);
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
				int len = SVI.FS_Write (file_handle, data_ptr, data.Length);
				Marshal.FreeHGlobal (data_ptr);
				return len;
			}

			protected override void Cleanup() {
				if (file_handle != IntPtr.Zero) SVI.FS_Close (file_handle);
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
			SVI.PrintStr(v.ToString());
		}
		public static void PrintLine<T>(T v) {
			SVI.PrintStr(v.ToString() + "\n");
		}

	}

	public static class SVA {

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
	}
}