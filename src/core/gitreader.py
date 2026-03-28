#!/usr/bin/env python3
"""
@project  devpulse
@org      Zenith Open Source Projects
@license  MIT License
"""

import subprocess
import sys
import os
import json

def get_git_info(path="."):
    """Returns git info as JSON. Returns empty JSON if not a git repo."""
    result = {
        "is_git": False,
        "branch": "",
        "status": [],
        "commits": []
    }

    try:
        # Check if git repo
        check = subprocess.run(
            ["git", "-C", path, "rev-parse", "--is-inside-work-tree"],
            capture_output=True, text=True
        )
        if check.returncode != 0:
            print(json.dumps(result))
            return

        result["is_git"] = True

        # Get branch
        branch = subprocess.run(
            ["git", "-C", path, "branch", "--show-current"],
            capture_output=True, text=True
        )
        result["branch"] = branch.stdout.strip()

        # Get status (short)
        status = subprocess.run(
            ["git", "-C", path, "status", "--short"],
            capture_output=True, text=True
        )
        result["status"] = [
            line for line in status.stdout.strip().split("\n") if line
        ]

        # Get last 5 commits
        log = subprocess.run(
            ["git", "-C", path, "log", "--oneline", "-5"],
            capture_output=True, text=True
        )
        result["commits"] = [
            line for line in log.stdout.strip().split("\n") if line
        ]

    except Exception as e:
        result["error"] = str(e)

    print(json.dumps(result))

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "."
    get_git_info(path)
