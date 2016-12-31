using System.Collections.Generic;
using System;

using JAOWNT;

static class JAOWNT_TEST {
	
	const string cc = "^7";
	static Random rnd = new Random();

	static void AIChat(string message) {
		G.Chat ("^9Facility ^3AI", cc + message);
	}
	
	[GA.EventCmd("test1")]
	public static void CmdTest1(G.Entity ent) {
		G.Entity activator = ent.activator;
		if (activator != null && activator.player != null) {
			AIChat(string.Format("{1}{0} activated ent test 1.", cc, activator.player.name));
		} else {
			AIChat("ERROR: Test 1 FAILED");
		}
	}

	[GA.EventCmd("test2")]
	public static void CmdTest2(G.Entity ent) {
		G.Entity activator = ent.activator;
		if (activator != null && activator.player != null) {
			AIChat(string.Format("{1}{0} activated ent test 2.", cc, activator.player.name));
		} else {
			AIChat("ERROR: Test 1 FAILED");
		}
	}
	
	[GA.EventCmd("test3")]
	public static void CmdTest3(G.Entity ent) {
		G.Entity activator = ent.activator;
		if (activator != null && activator.player != null) {
			AIChat(string.Format("{1}{0} activated ent test 3.", cc, activator.player.name));
		} else {
			AIChat("ERROR: Test 1 FAILED");
		}
	}

	static List<G.Entity> siths = new List<G.Entity>();
	static string[] possible_siths = new string[] {
		"desann", "tavion", "tavion_new", "reborn", "stormtrooper", "reborn_new"
	};

	[GA.EventCmd("npctest_spawnsith")]
	public static void CmdNPCTestSpawnSith(G.Entity pent) {
		G.Entity[] spawn_targets = pent.RetrieveTargets ();
		foreach(G.Entity ent in spawn_targets) {
			siths.Add(G.SpawnNPC(possible_siths[rnd.Next(possible_siths.Length)], ent.origin, 180));
		}
	}

	[GA.EventCmd("npctest_killsith")]
	public static void CmdNPCTestKillSith(G.Entity ent) {
		AIChat(string.Format("Killing ^1{1}{0} sith NPCs from NPC Test Zone.", cc, siths.Count));
		foreach (G.Entity sith in siths) {
			sith.npc.Kill ();
		}
		siths.Clear ();
	}

	static List<G.Entity> jedis = new List<G.Entity>();
	static string[] possible_jedis = new string[] {
		"luke", "jedi", "jedimaster", "rosh_penin", "chewie", "jedi2"
	};

	[GA.EventCmd("npctest_spawnjedi")]
	public static void CmdNPCTestSpawnJedi(G.Entity pent) {
		G.Entity[] spawn_targets = pent.RetrieveTargets ();
		foreach(G.Entity ent in spawn_targets) {
			jedis.Add(G.SpawnNPC(possible_jedis[rnd.Next(possible_jedis.Length)], ent.origin, 180));
		}
	}

	[GA.EventCmd("npctest_killjedi")]
	public static void CmdNPCTestKillJedi(G.Entity ent) {
		AIChat(string.Format("Killing ^4{1}{0} jedi NPCs from NPC Test Zone.", cc, jedis.Count));
		foreach (G.Entity jedi in jedis) {
			jedi.npc.Kill ();
		}
		jedis.Clear ();
	}

	[GA.EventCmd("npctest_killall")]
	public static void CmdNPCTestKillAll(G.Entity ent) {
		AIChat(string.Format("Killing all ^3{1}{0} NPCs from NPC Test Zone.", cc, jedis.Count + siths.Count));
		foreach (G.Entity sith in siths) {
			sith.npc.Kill ();
		}
		siths.Clear ();
		foreach (G.Entity jedi in jedis) {
			jedi.npc.Kill ();
		}
		jedis.Clear ();
	}

	[GA.EventCmd("movetest")]
	public static void CmdMoveTest(G.Entity ent) {
		AIChat ("test");
	}
}
