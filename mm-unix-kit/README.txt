Materia Mundi — UNIX Telnet Kit

Full documentation and tutorial in doc/

Install:
  sudo mkdir -p /games
  sudo tar -xzf mm-unix-kit.tar.gz -C /games
  cd /games/mm-unix-kit && make && chmod +x bin/mm-*

Optional symlink as /games/mm:
  sudo ln -s /games/mm-unix-kit /games/mm

Usage (referee):
  export MM_ROOT=/games/mm
  mm-session start Antwerp-HYW 1
  export MM_SESSION=$(awk '{print $3}' <<< "$(mm-session start Antwerp-HYW 1 2>/dev/null)")

Usage (players):
  mm-session join Antwerp-HYW recent
  export MM_SESSION=.../sessions/<date>_1
  mm-say "hello"
  mm-roll d20

All tools are POSIX sh and a tiny C roller in src/

/games/mm/
├── doc/*                    # full documentation
├── bin/                     # scripts and tiny C tools
│   ├── mm-session           # start/join/end sessions; sets up FIFOs & logs
│   ├── mm-roll              # dice roller front-end (calls ./roll)
│   ├── mm-save              # compute/save throws per rules
│   ├── mm-skill             # n-in-6 skill checks; easy/hard
│   ├── mm-attack            # melee/ranged attack throw helper
│   ├── mm-char              # CRUD for character sheets (text)
│   ├── mm-initiative        # turn tracker
│   ├── mm-map               # view/edit ASCII maps
│   ├── mm-table             # pick from random tables (awk)
│   ├── mm-say               # table chat (FIFO) with timestamps
│   ├── mm-whisper           # private tells via per-user FIFOs
│   └── mm-note              # append to session log / referee notebook
├── lib/
│   ├── rules/               # rule tables as TSV for awk (saves, skills, weapons…)
│   ├── tables/              # random encounter/loot tables (TSV/CSV)
│   ├── templates/
│   │   ├── character.mm     # character sheet skeleton
│   │   └── setting.README   # per-setting README template
│   └── maps/                # sample ASCII tiles & legends
├── src/
│   └── roll.c               # tiny, auditable dice roller
├── campaign/                # each setting is a directory
│   └── Antwerp-HYW/        # example setting
│       ├── README.md
│       ├── world/           # world docs, NPCs, locations (text)
│       ├── pcs/             # characters (one file per PC)
│       ├── npcs/            # ref-controlled statblocks
│       ├── stash/           # handouts, items, treasure lists
│       ├── maps/            # ascii map files
│       └── sessions/
│           └── 1991-12-25_1/   # new session created by mm-session start
│               ├── chat.fifo
│               ├── gm.fifo
│               ├── log.txt         # immutable session log (append-only)
│               ├── gm.log          # referee private log
│               ├── initiative.txt
│               ├── turnlock        # file lock for turn-taking
│               ├── rng.log         # dice outputs, seeds, provenance
│               ├── map.txt         # active dungeon/room map
│               └── state/          # room keys, statuses, conditions
├── Makefile                # builds src/roll.c → bin/roll, installs scripts
└─- tmux.conf.example               # sane defaults for tmux config (optional)



Optional tmux helpers
====================

All helpers are optional; you can play entirely without tmux.

Prereqs: `tmux` installed; no system config required.
These commands use $MM_SESSION if set, otherwise they try to find the most recent
session in $MM_ROOT/campaign/<campaign>/sessions.

Commands
--------
- mm-tmux-ref [campaign] [name]
  Opens a tmux session with panes for command shell, table log, GM log, and a live map viewer.
  Adds extra windows for map editing and RNG audit.

- mm-tmux-player [campaign] [name]
  Opens a two-pane layout (command + live log) and windows for map and a guessed character sheet.

- mm-tmux-logtail
  Minimal tail-only dashboard for log.txt and rng.log (requires $MM_SESSION).

- mm-tmux-attach [sessionName|last]
  Attach to a running tmux session (defaults to the last).

- mm-tmux-kill <sessionName>
  Kill a tmux session.

Tips
----
- Put `tmux.conf.example` into `~/.tmux.conf` if you want nicer defaults:
    cp /games/mm-unix-kit/tmux.conf.example ~/.tmux.conf && tmux source-file ~/.tmux.conf
- Players: run `mm-session join <campaign> recent` first, then `mm-tmux-player <campaign>`.
- Ref: run `mm-session start <campaign> <label>` then `export MM_SESSION=...` then `mm-tmux-ref <campaign> gm`.


