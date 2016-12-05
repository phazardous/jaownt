using System;
using System.Runtime.CompilerServices;

namespace SHARPSV {

	public static class SVAPI_OUT {

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void PrintStr(string str);

		public static void Print<T>(T v) {
			PrintStr(v.ToString());
		}
		public static void PrintLine<T>(T v) {
			PrintStr(v.ToString() + "\n");
		}

	}
	
	public static class SVAPI_IN {
		
		public static void Event_Frame(int time) {
			SVAPI_OUT.PrintLine (time);
		}
	}
}
