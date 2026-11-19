#!/usr/bin/env bash
#
# Runs one non-interactive Claude Code turn and prints the JSON object it was
# asked for.
#
# usage: claude-structured.sh <edit|read> <json-schema> <budget-usd> < prompt
#
# `edit` lets the turn change files under the working directory and run
# cargo and the read-only git commands; `read` lets it only read files and run
# the read-only git commands. A tool call outside that set is denied at once
# instead of waiting for a person who is not there. The turn never commits
# or pushes: the caller does, so what Claude wrote is always visible in a
# commit of its own. CLAUDE_MODEL names the model (unset: Claude Code's
# default); CLAUDE_ADD_DIRS lists extra directories, space separated, that
# the turn may read; CLAUDE_TIMEOUT caps the turn's wall clock (default 45m).
# The object matching the schema goes to stdout; the turn's cost and every
# denied tool call go to stderr. Exits 1 when the turn ends in an error or
# returns no object.
set -euo pipefail

if [ "$#" -ne 3 ]; then
  echo "usage: $0 <edit|read> <json-schema> <budget-usd> < prompt" >&2
  exit 2
fi
mode=$1
schema=$2
budget=$3

# `--allowedTools` and `--add-dir` take every argument up to the next option,
# so each list is one quoted argument and the prompt travels on stdin, where
# it cannot be taken for a tool name.
read_only="Bash(git status:*) Bash(git diff:*) Bash(git log:*) Bash(git show:*) Bash(git blame:*) Bash(grep:*) Bash(rg:*) Bash(ls:*) Bash(cat:*) Bash(head:*) Bash(tail:*) Bash(wc:*) Bash(find:*)"
options=(
  --print
  --output-format json
  --json-schema "$schema"
  --max-budget-usd "$budget"
  --no-session-persistence
  --permission-prompts none
)
case "$mode" in
  edit)
    options+=(--permission-mode acceptEdits)
    options+=(--allowedTools "Read Edit Write Glob Grep $read_only Bash(cargo check:*) Bash(cargo build:*) Bash(cargo test:*) Bash(cargo clippy:*)")
    ;;
  read)
    options+=(--tools "Read,Glob,Grep,Bash")
    options+=(--allowedTools "$read_only")
    ;;
  *)
    echo "the mode must be edit or read, got: $mode" >&2
    exit 2
    ;;
esac
if [ -n "${CLAUDE_MODEL:-}" ]; then
  options+=(--model "$CLAUDE_MODEL")
fi
for dir in ${CLAUDE_ADD_DIRS:-}; do
  options+=(--add-dir "$dir")
done

# The prompt is this script's stdin, which claude inherits.
if ! result=$(timeout "${CLAUDE_TIMEOUT:-45m}" claude "${options[@]}"); then
  echo "claude exited with an error:" >&2
  printf '%s\n' "$result" | tail -c 4000 >&2
  exit 1
fi
if ! jq -e . > /dev/null 2>&1 <<< "$result"; then
  echo "claude printed no JSON result:" >&2
  printf '%s\n' "$result" | tail -c 4000 >&2
  exit 1
fi
jq -r '"Claude Code turn: \(.num_turns // "?") turns, \(.total_cost_usd // 0 | tostring | .[0:6]) USD"
       + (if (.permission_denials // []) | length > 0
          then ", denied: " + ((.permission_denials // []) | map(.tool_name) | unique | join(", "))
          else "" end)' <<< "$result" >&2
if [ "$(jq -r '.is_error' <<< "$result")" = true ]; then
  jq -r '.result // .subtype' <<< "$result" >&2
  exit 1
fi
output=$(jq -c '.structured_output // empty' <<< "$result")
if [ -z "$output" ]; then
  echo "claude returned no object matching the schema" >&2
  jq -r '.result // .subtype' <<< "$result" | tail -c 2000 >&2
  exit 1
fi
printf '%s\n' "$output"
