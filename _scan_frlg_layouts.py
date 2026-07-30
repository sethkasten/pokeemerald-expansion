import json
d = json.load(open('data/layouts/layouts.json'))
frlg = [L for L in d['layouts'] if L and L.get('layout_version') == 'frlg']
print(f'Total FRLG-only layouts (all NULL in Emerald build): {len(frlg)}')
print()
print('Layouts referenced by mod-added warp destinations:')
watch = ['MT_MOON','ROCK_TUNNEL','DIGLETTS','CERULEAN','VICTORY_ROAD','SAFARI_ZONE_CENTER','SAFARI_ZONE_EAST','SAFARI_ZONE_WEST','SAFARI_ZONE_NORTH_FRLG','SAFARI_ZONE_REST','SAFARI_ZONE_SECRET','TWO_ISLAND','ONE_ISLAND','ROUTE121_SAFARI','FUCHSIA_CITY_SAFARI']
for L in frlg:
    lid = L.get('id','')
    if any(w in lid for w in watch):
        print(f'  {lid:60}  ({L.get("name","")})')
