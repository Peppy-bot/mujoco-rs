#!/usr/bin/env bash
#
# Repins, in a Cargo.lock, every package that one git repository provides.
#
# usage: pin-git-dependency.sh <Cargo.lock> <repository-url> <commit> [<name>=<version>...]
#
# Every `[[package]]` whose `source` is `git+<repository-url>#<old commit>` gets
# `#<commit>` instead, and the `version` the caller names for it. This is the
# edit `cargo update -p <name>` makes when the package's own dependency list
# did not change between the two commits; unlike cargo it needs neither the
# workspace's generated path dependencies nor the network, which is why the
# sync workflow can run it on a bare clone. The lockfile is rewritten in
# place; the names of the repinned packages go to stdout. Exits 1 when the
# lockfile names no package from that repository.
set -euo pipefail

if [ "$#" -lt 3 ]; then
  echo "usage: $0 <Cargo.lock> <repository-url> <commit> [<name>=<version>...]" >&2
  exit 2
fi
lockfile=$1
repository=$2
commit=$3
shift 3

names=$(mktemp)
trap 'rm -f "$names"' EXIT

# Paragraph mode: each `[[package]]` block is one record, so a package's
# `name`, `version` and `source` lines are matched together; the header is
# the first record. Records are re-joined by exactly one blank line and the
# file keeps its single trailing newline, so an untouched lockfile comes out
# byte-identical. The repository URL is matched literally (`index`), so its
# dots and plus signs need no escaping; only the fixed `source` shape is a
# regex.
REPOSITORY="$repository" COMMIT="$commit" VERSIONS="$*" awk '
  BEGIN {
    RS = ""
    n = split(ENVIRON["VERSIONS"], pairs, " ")
    for (i = 1; i <= n; i++) {
      split(pairs[i], pair, "=")
      version[pair[1]] = pair[2]
    }
  }
  {
    if (index($0, "source = \"git+" ENVIRON["REPOSITORY"] "#") > 0 && match($0, /name = "[^"]+"/)) {
      name = substr($0, RSTART + 8, RLENGTH - 9)
      sub(/source = "git\+[^"#]+#[0-9a-f]+"/, "source = \"git+" ENVIRON["REPOSITORY"] "#" ENVIRON["COMMIT"] "\"")
      if (name in version) {
        sub(/version = "[^"]+"/, "version = \"" version[name] "\"")
      }
      print name > names
    }
    printf "%s%s", (NR > 1 ? "\n\n" : ""), $0
  }
  END { printf "\n" }
' names="$names" "$lockfile" > "$lockfile.repinned"

if [ ! -s "$names" ]; then
  rm -f "$lockfile.repinned"
  echo "$lockfile names no package from $repository" >&2
  exit 1
fi
mv "$lockfile.repinned" "$lockfile"
cat "$names"
