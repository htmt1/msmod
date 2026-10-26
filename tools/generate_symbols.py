"""Generate the exact-build native ABI map from the local LibCpp2IL JSON dump.
Usage: python tools/generate_symbols.py /path/to/symbols.json /path/to/GameAssembly.dll
The proprietary inputs and dump must not be committed or redistributed.
"""
import json,sys,hashlib,pathlib
s=json.load(open(sys.argv[1])); types={t['name']:t for t in s}
queries={
 'SteamId':('MoonCleaners.MoonSteam','get_SteamId'),
 'HostId':('MoonCleaners.MoonSteamLobby','get_HostId'),
 'InLobby':('MoonCleaners.MoonSteamLobby','get_InLobby'),
 'LobbyId':('MoonCleaners.MoonSteamLobby','get_LobbyId'),
 'IsMember':('MoonCleaners.MoonSteamLobby','IsMember'),
 'Joined':('MoonCleaners.MoonCrew','get_Joined'),
 'CrewHost':('MoonCleaners.MoonCrew','get_HostId'),
 'SessionInstance':('MoonCleaners.MoonCrewSession','get_Instance'),
 'SessionUpdate':('MoonCleaners.MoonCrewSession','Update'),
 'LocalPlayer':('MoonCleaners.MoonCrew','get_LocalPlayerId'),
 'WalkerPlayer':('MoonCleaners.MoonWalker','get_UpgradePlayer'),
 'SaveDirectory':('MoonCleaners.MoonWorld','get_SaveDirectory'),
 'WalletDollars':('MoonCleaners.MoonWorld','WalletDollars'),
 'SetTeamDollars':('MoonCleaners.MoonWorld','SetTeamDollars'),
 'WalletGet':('MoonCleaners.MoonWorld+TeamWallet','get_Item'),
 'WalletSet':('MoonCleaners.MoonWorld+TeamWallet','set_Item'),
 'RestoreEconomy':('MoonCleaners.MoonWorld','RestoreEconomy'),
 'SnapshotEconomy':('MoonCleaners.MoonWorld','SnapshotEconomy'),
 'DecimalAdd':('System.Decimal','op_Addition'),
 'DecimalSub':('System.Decimal','op_Subtraction'),
 'DecimalLess':('System.Decimal','op_LessThan'),
 'DecimalEqual':('System.Decimal','op_Equality'),
}
out=['// Generated for the user-supplied Moon Sweepers build. Do not use with other builds.','#pragma once','#include <cstdint>','namespace game {']
for key,(typ,method) in queries.items():
 matches=[m for m in types[typ]['methods'] if m['name']==method]
 if len(matches)!=1:raise ValueError((key,matches))
 m=matches[0];out.append(f'constexpr uintptr_t {key}=0x{m["rva"]:X}; // {m["signature"]}')
for typ,field,key in [('MoonCleaners.MoonWorld','wallets','WorldWallet'),('MoonCleaners.MoonCrewSession','bound','SessionWorld')]:
 f=next(f for f in types[typ]['fields'] if f['name']==field);out.append(f'constexpr uintptr_t {key}=0x{f["offset"]:X};')
out+=['constexpr uintptr_t WalletBalance=0x10;',f'constexpr char AssemblySha256[]="{hashlib.sha256(open(sys.argv[2],"rb").read()).hexdigest()}";', 'constexpr char MetadataSha256[]="f5ac72f32a8d9f5992ec793265b6dec0092a7358c342194fd0afb4e193119c1b";','}']
pathlib.Path('src/game_build.hpp').write_text('\n'.join(out)+'\n')
