Materia Mundi:  A Tabletop Fantasy System for Shared UNIX Terminals
===================================================================

OVERVIEW
--------
Materia Mundi is a human-refereed fantasy role-playing game built for minimalism
and transparency.  The original tabletop rules (see Players Guide & Referee’s Guide)
emphasize simple throws and narrative judgment over automation.  This UNIX edition
adapts that spirit to a shared text environment: players log into the same host
(via telnet, ssh, or local terminals), `cd` into a campaign directory, and run a
set of shell tools that coordinate rolls, logs, and maps.  The referee still rules.

Worlds are directories.  Adventures are files

Nothing here "runs the game"—it only helps you *run it together*.




WHAT YOU NEED
-------------
•  A POSIX system with:
      sh / bash,  awk,  sed,  make,  gcc (or cc),  tmux (optional)
•  Multiple user accounts in the same UNIX group
•  A place to install the kit, e.g.  /games/mm
•  One referee account (group mmref)
•  Player accounts (group mmplayers)


INSTALLATION
------------
  1.  Copy the tarball and unpack:
        sudo mkdir -p /games
        sudo tar -xzf mm-unix-kit-with-tmux.tar.gz -C /games
        cd /games/mm-unix-kit && make
        chmod +x bin/mm-*

  2.  (optional convenience)
        sudo ln -s /games/mm-unix-kit /games/mm
        export PATH=/games/mm/bin:$PATH

  3.  Create user groups and permissions as root:
        groupadd mmref
        groupadd mmplayers
        chgrp -R mmref /games/mm
        chmod -R g+rX /games/mm
        find /games/mm -type d -exec chmod 2775 {} \;
        chgrp -R mmplayers /games/mm/campaign/Antwerp-HYW/pcs
        chmod -R 2755 /games/mm/campaign/Antwerp-HYW/pcs
        chmod -R 2750 /games/mm/campaign/Antwerp-HYW/npcs


FIRST STEPS
-----------
Referee:
    mm-session start Antwerp-HYW 1
    export MM_SESSION=/games/mm/campaign/Antwerp-HYW/sessions/1991-12-25_1
    tail -f "$MM_SESSION/log.txt" &

Players:
    mm-session join Antwerp-HYW recent
    export MM_SESSION=/games/mm/campaign/Antwerp-HYW/sessions/1991-12-25_1
    mm-say "Ready!"
    mm-roll d20
    mm-skill 5+ easy

Optional tmux helpers:
    mm-tmux-ref Antwerp-HYW gm       # Referee’s multi-pane view
    mm-tmux-player Antwerp-HYW hero  # Player’s two-pane layout
    cp /games/mm/tmux.conf.example ~/.tmux.conf  # for nicer defaults


ABOUT THE SETTING
-----------------
The sample campaign *Antwerp — Hundred Years’ War* begins in 1430s Flanders,
after the fall of Gilles de Rais.  The world mixes low fantasy with moral
naturalism: relics, alchemy, and divine visitations are real but costly.
Technology and sorcery grow from the same roots.  Expect a grim but luminous
Europe where saints and scientists vie for the soul of matter.

(You can of course replace this with any world; each campaign lives in its
own directory under  /games/mm/campaign/.)


BASIC CONCEPTS
--------------
Characters live as plain text files under  pcs/.
NPCs and hidden data live under  npcs/  (referee only).
Each game session is a subdirectory under  campaign/<world>/sessions/.
Session files act as a shared notebook:
    log.txt      — the public chat and action log
    gm.log       — private referee notes
    rng.log      — every die roll with seed and outcome
    map.txt      — current ASCII map
    initiative.txt — turn order
    chat.fifo, gm.fifo — live message pipes (created at start)

All dice, rolls, and actions append to text logs; there’s no secret state.


CORE COMMANDS
--------------
mm-session   — start/join/end sessions (creates logs and FIFOs)
mm-say       — speak to the table (appends to log.txt)
mm-note      — GM-only note to gm.log (+marker in log.txt)
mm-roll EXPR — roll dice; EXPR like  d20,  2d6+1,  n6:5+,  coin
mm-skill N+  — n-in-6 skill test; add “easy” or “hard” to roll twice
mm-save TYPE ABILITY LEVEL — returns d20 target number
mm-attack TYPE ABILITY LEVEL — same for to-hit rolls
mm-char      — create, view, or edit characters
mm-initiative — manage turn order
mm-table FILE.tsv — choose a weighted random entry from a table
mm-map       — show, edit, or update the ASCII map
mm-tmux-*    — optional tmux interfaces (see README_tmux.txt)


DICE & SKILLS
--------------
•  Attack, Save, and similar throws:  roll d20 ≥ threshold.
   Threshold = table lookup of (Ability Modifier + Level).
•  Skills:  roll 1d6; succeed if result ≥ skill value (usually 5+).
   “easy” = roll twice, keep best; “hard” = roll twice, keep worst.
•  Every roll is logged to rng.log with timestamp and seed.


SESSION LOGGING
---------------
All activity—speech, rolls, system messages—is appended to log.txt.
This serves as the permanent record of play.
You can mark important scenes manually:
    mm-note "END OF SESSION 3"
The referee can later audit rng.log to verify results.


ASCII MAPS
----------
Maps are just text files.  Example in  lib/maps/sample_map.txt :
    @ legend: #=wall .=floor D=door
    ########################################
    #......D..............#...............#
    #..#######..####..###.#..####...####..#
    ########################################
Players may read them with `less map.txt`; the GM may edit live
(using `mm-map edit` or the tmux map pane).


RHYTHM OF PLAY
--------------
1.  Referee describes the scene.
2.  Players declare actions via mm-say.
3.  Rolls are made as needed (mm-roll or mm-skill).
4.  Ref adjudicates, updates map.txt or notes.
5.  Initiative system handles combat.
6.  End of session: GM archives the directory or copies logs.

You can simply `tar czf sessions/<date>.tgz` to preserve history.


EXAMPLE TURN
-------------
  Player> mm-say "I lunge at the ghoul."
  Player> mm-attack melee +1 1     # gives target number, e.g. 14+
  Player> mm-roll d20
  Referee> mm-note "Ghoul takes 4 dmg, recoils."
  Player> mm-skill 5+ easy         # Finesse test to parry
  Everyone sees rolls appear in log.txt.


FOR BEGINNERS
-------------
If you’ve never played a TTRPG:
  •  Start with two friends and one referee.
  •  Each player edits a copy of lib/templates/character.mm.
  •  Treat numbers loosely: the system rewards description.
  •  “n-in-6” means you roll 1d6; succeed on 6 unless trained (5+) or advantaged.
  •  Ask the referee before using any unfamiliar command.

For your first session, stay in one room of the sample Antwerp map,
fight a bandit, and find a relic.  You’ll learn everything you need.


HOUSEKEEPING
------------
•  Use `cron` or `tar` to back up sessions nightly.
•  Referee can lock turns with:  flock $MM_SESSION/turnlock mm-initiative next
•  To verify dice: compare log.txt vs rng.log.
•  `chattr +a log.txt` (if available) makes logs append-only.


HACKABILITY
-----------
Every command is a shell script or tiny C program.
Extend it with your own:
  bin/mm-loot, bin/mm-morale, etc.
Use the same text-in/text-out pattern so it composes with pipes.
Nothing depends on daemons, databases, or graphics—just text.


ACKNOWLEDGMENTS
---------------
Inspired by decades of TTRPG tradition, MUDs, and the ethos of
“make it human-readable first.”  The Materia Mundi rules and setting
are by their original authors; this UNIX edition is a fan adaptation
for educational and archival purposes.


--  END OF README
