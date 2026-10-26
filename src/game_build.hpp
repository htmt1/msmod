// Generated for the user-supplied Moon Sweepers build. Do not use with other builds.
#pragma once
#include <cstdint>
namespace game {
constexpr uintptr_t SteamId=0x596960; // System.UInt64 get_SteamId()
constexpr uintptr_t HostId=0x5948D0; // System.UInt64 get_HostId()
constexpr uintptr_t InLobby=0x594920; // System.Boolean get_InLobby()
constexpr uintptr_t LobbyId=0x594A70; // System.UInt64 get_LobbyId()
constexpr uintptr_t IsMember=0x58F9A0; // System.Boolean IsMember(System.UInt64 id)
constexpr uintptr_t Joined=0x7A0560; // System.Boolean get_Joined()
constexpr uintptr_t CrewHost=0x7A0510; // System.UInt64 get_HostId()
constexpr uintptr_t SessionInstance=0x794240; // MoonCleaners.MoonCrewSession get_Instance()
constexpr uintptr_t SessionUpdate=0x792A80; // System.Void Update()
constexpr uintptr_t LocalPlayer=0x7A05E0; // System.String get_LocalPlayerId()
constexpr uintptr_t WalkerPlayer=0x635650; // System.String get_UpgradePlayer()
constexpr uintptr_t SaveDirectory=0x75AC00; // System.String get_SaveDirectory()
constexpr uintptr_t WalletDollars=0x74B880; // System.Decimal WalletDollars(System.String playerId = local)
constexpr uintptr_t SetTeamDollars=0x71CB40; // System.Void SetTeamDollars(System.Decimal dollars)
constexpr uintptr_t WalletGet=0x76C370; // System.Decimal get_Item(System.String player)
constexpr uintptr_t WalletSet=0x76C430; // System.Void set_Item(System.String player, System.Decimal value)
constexpr uintptr_t RestoreEconomy=0x70DD50; // System.Void RestoreEconomy(MoonCleaners.MoonLooseDustSnapshot snapshot)
constexpr uintptr_t SnapshotEconomy=0x7242D0; // System.Void SnapshotEconomy(MoonCleaners.MoonLooseDustSnapshot snapshot)
constexpr uintptr_t DecimalAdd=0x14DD180; // System.Decimal op_Addition(System.Decimal d1, System.Decimal d2)
constexpr uintptr_t DecimalSub=0x14DDAD0; // System.Decimal op_Subtraction(System.Decimal d1, System.Decimal d2)
constexpr uintptr_t DecimalLess=0x14DD9D0; // System.Boolean op_LessThan(System.Decimal d1, System.Decimal d2)
constexpr uintptr_t DecimalEqual=0x14DD2C0; // System.Boolean op_Equality(System.Decimal d1, System.Decimal d2)
constexpr uintptr_t WorldWallet=0x538;
constexpr uintptr_t SessionWorld=0x1B0;
constexpr uintptr_t WalletBalance=0x10;
constexpr char AssemblySha256[]="82b2e7f2a049c332e836fd9b492ce2fa16c1600de1d276b4b79742daf15adea0";
constexpr char MetadataSha256[]="f5ac72f32a8d9f5992ec793265b6dec0092a7358c342194fd0afb4e193119c1b";
}
