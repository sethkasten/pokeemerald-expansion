#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
echo "=== groups.inc: gMapGroup_ labels ==="
grep '^gMapGroup_' data/maps/groups.inc || true
echo
echo "=== groups.inc: last 30 lines (should show gMapGroups:: and its entries) ==="
tail -n 60 data/maps/groups.inc
echo
echo "=== map_groups.json group_order ==="
python3 - <<'PY'
import json
d = json.load(open("data/maps/map_groups.json"))
order = d["group_order"]
print(len(order), "groups total")
for i, n in enumerate(order):
    print(f"  [{i}] {n}")
PY
