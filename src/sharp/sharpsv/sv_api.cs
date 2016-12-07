using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace SHARPSV {

	public static class SVAPI_INTERNAL {

		public class SVAPIException : Exception {
			public SVAPIException(string message) : base(message) {}
		}

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
		public static extern void FS_Read(IntPtr fh, IntPtr buf, int len);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void FS_Write(IntPtr fh, IntPtr buf, int len);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void PrintStr(string str);

		public static IntPtr Load_Manifest(string manifest) {
			return Nexus.CreateNodeFromManifest (manifest);
		}

		public static void Event_Frame(int time) {
			//SVAPI_OUT.PrintLine (time);
		}
	}

	public static class SVAPI {

		// ====================== //
		// -------- FILE -------- //
		// ====================== //

		public class File : SVAPI_INTERNAL.UnmanagedGameResource {

			public enum OpenMode {
				READ,
				WRITE,
			}
			
			public File(string path, OpenMode om) {
				switch (om) {
				case OpenMode.READ:
					file_handle = SVAPI_INTERNAL.FS_OpenR(path);
					break;
				case OpenMode.WRITE:
					file_handle = SVAPI_INTERNAL.FS_OpenW(path);
					break;
				}
				openMode = om;
				if (file_handle == IntPtr.Zero) throw new SVAPI_INTERNAL.SVAPIException("file not found");
				length = SVAPI_INTERNAL.FS_Length(file_handle);
			}

			public byte[] Read (int len) {
				if (openMode != OpenMode.READ) throw new SVAPI_INTERNAL.SVAPIException("read operation not allowed");
				IntPtr data_ptr = Marshal.AllocHGlobal (len);
				SVAPI_INTERNAL.FS_Read (file_handle, data_ptr, len);
				byte[] data = new byte[len];
				Marshal.Copy (data_ptr, data, 0, len);
				Marshal.FreeHGlobal (data_ptr);
				return data;
			}

			public void Write (byte[] data) {
				if (openMode != OpenMode.WRITE) throw new SVAPI_INTERNAL.SVAPIException("write operation not allowed");
				IntPtr data_ptr = Marshal.AllocHGlobal (data.Length);
				Marshal.Copy (data, 0, data_ptr, data.Length);
				SVAPI_INTERNAL.FS_Write (file_handle, data_ptr, data.Length);
				Marshal.FreeHGlobal (data_ptr);
			}

			protected override void Cleanup() {
				if (file_handle != IntPtr.Zero) SVAPI_INTERNAL.FS_Close (file_handle);
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
			SVAPI_INTERNAL.PrintStr(v.ToString());
		}
		public static void PrintLine<T>(T v) {
			SVAPI_INTERNAL.PrintStr(v.ToString() + "\n");
		}

	}
}