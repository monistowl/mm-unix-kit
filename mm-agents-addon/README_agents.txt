Materia Mundi — Agents Add-on
=================================

Install:
  cp -r mm-agents-addon/* /games/mm-unix-kit/
  chmod +x /games/mm-unix-kit/bin/mm-agent-* /games/mm-unix-kit/agents/adapters/*

Run (toy bot):
  export MM_ROOT=/games/mm
  mm-session start Antwerp-HYW agents
  export MM_SESSION=.../sessions/<date>_agents
  mm-agent-loop Antwerp-HYW "joan martin" 5 /games/mm/bin/mm-agent-toy

Swap in a model:
  export MODEL_CMD='ollama run llama3.1:8b-instruct'
  mm-agent-loop Antwerp-HYW "joan martin" 6 /games/mm/agents/adapters/model-stdin

Simple loom:
  export MODEL_CMD='ollama run llama3.1:8b-instruct'  # or your API wrapper
  mm-agent-loop Antwerp-HYW "joan martin" 1 /games/mm/agents/adapters/model-stdin
  mm-gm-step Antwerp-HYW /games/mm/agents/adapters/model-stdin