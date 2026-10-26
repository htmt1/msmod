"""Generate verified native hook locations for the supplied game's in-game UI."""
import json,sys
from pathlib import Path
classes={t['name']:t for t in json.load(open(sys.argv[1]))}
queries={
 'ButtonPress':('UnityEngine.UI.Button','Press'),
 'TypingGet':('MoonCleaners.MoonInput','get_Typing'),
 'TypingSet':('MoonCleaners.MoonInput','set_Typing'),
 'BlocksGameplay':('MoonCleaners.MoonPauseMenu','get_BlocksGameplay'),
 'CursorLockSet':('UnityEngine.Cursor','set_lockState'),
 'CursorVisibleSet':('UnityEngine.Cursor','set_visible'),
}
lines=['// Generated from the exact-build IL2CPP metadata and native code map.','#pragma once','#include <cstdint>','namespace game::ui {']
for name,(typ,method) in queries.items():
 matches=[m for m in classes[typ]['methods'] if m['name']==method];assert len(matches)==1
 lines.append(f'constexpr uintptr_t {name}=0x{matches[0]["rva"]:X}; // {typ}.{method}')
lines+=['}'];Path('src/ui_build.hpp').write_text('\n'.join(lines)+'\n')
