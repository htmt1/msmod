# Implementation notes

This is an experimental native Windows x64 mod for the exact user-provided Moon Sweepers build, Unity 6000.6.0f1, IL2CPP metadata v108. It does not replace or redistribute game binaries and does not depend on a managed mod loader's support for v108.

## Evidence and build gate

- GameAssembly.dll SHA-256: `82b2e7f2a049c332e836fd9b492ce2fa16c1600de1d276b4b79742daf15adea0`
- global-metadata.dat SHA-256: `f5ac72f32a8d9f5992ec793265b6dec0092a7358c342194fd0afb4e193119c1b`
- LibCpp2IL source used for analysis: SamboyCoding/Cpp2IL `b5ad444b82267cb1e4b88b8b373c008105bdea52`.
- MinHook vendored source: TsudaKageyu/minhook `8af6b4acae5a9388fd742b56fa79ece89d96f823` (BSD 2-Clause).

The distribution uses a native `version.dll` proxy and a plugin at `MoonWallet/plugins/MoonWallet.dll`. The proxy loads only that exact plugin and only in MoonSweepers.exe. It forwards the 17 Windows version-information APIs to the absolute System32 version.dll. Generated x64 tail stubs preserve integer/pointer and XMM0-3 arguments plus stack arguments, and have unwind metadata. The resolver preserves the incoming last-error value. No global loader, system-directory installation or separate executable is required.

A worker validates both game hashes and waits for GameAssembly.dll. It calls no managed game APIs before IL2CPP startup. The plugin detours the exported il2cpp_runtime_invoke, identifies a Unity active-scene callback or a game MonoBehaviour Awake/OnEnable/Start/Update, then installs gameplay detours on that callback's thread. The bootstrap detour is disabled but its trampoline retained for in-flight calls. Gameplay activation failure rolls back only the owned gameplay hooks. The old optional manual launcher source is retained for development, but no executable is included in the autoload package.

Startup via version.dll assumes Unity/Windows loads the local proxy. UnityPlayer.dll and the game executable were not available for import inspection; actual autoload and native forwarding against Windows System32 have not been executed. Machine-code emulation checks the compiled proxy ABI, not the Windows loader path. An existing local version.dll must not be overwritten; coexistence with another loader needs separate validation.

BepInEx and Il2CppInterop upstream sources were inspected (BepInEx `b782da4ac54e1072d2857f4a2f77c80c3ef08005`, Il2CppInterop `81a6f78c8b653e0da4a3420ac4cd00819e8b6292`). No compatible end-to-end BepInEx setup for this game's metadata v108 was demonstrated. This native DLL must not be advertised as a normal managed BepInEx/plugins assembly.

The metadata and disassembly establish that `MoonWorld.TeamWallet.get_Item(player)` at RVA `0x76C370` ignores `player` and returns `Balance` at offset `0x10`. `MoonWorld.WalletDollars` at `0x74B880` does the same through the world wallet at `0x538`. There are inlined Balance reads inside purchase checks; hooking just the public getter would leave purchases using another person's money.

## Integration

`game_build.hpp` and `economy_hooks.inc` are generated ABI maps for the exact build. They cover the world economy entry points, including both CheckConveyor overloads and both SaveProgress overloads. `TeamWallet.set_Item`, `WalletDollars`, `SetTeamDollars`, `RestoreEconomy`, `SnapshotEconomy`, `MoonCrewSession.Count`, and the session update loop have explicit detours.

A scoped balance is presented to the original affordability and purchase code. The setter converts the game's absolute balance assignment back into a delta, attributes it to the supplied player ID, and invokes the original event machinery with the corresponding per-player change. Multi-recipient payout loops retain a fixed baseline. Direct Balance writes in the legacy depot/hex retirement paths are detected on scope exit and credited to the host. Other game-side pricing, eligibility, inventories and reward formulas remain in the original functions.

The native Decimal layout is checked against metadata: flags/hi/lo/mid, 16 bytes. Arithmetic and comparison call the original game's Decimal functions with explicit Windows x64 result-buffer/by-reference ABI. Balances preserve the game's fractional Decimal values; transfers accept cents without floating point. Inputs and persisted values are bounded to a magnitude of 10^15 dollars. This bound is a defensive implementation limit, not a game-discovered money cap.

## Transport and authority

SteamNetworkingMessages channel 28741 is separate from the game channels. Steam authenticates the sender; no claimed sender ID is accepted from a packet. Membership is refreshed from the current Steam lobby. Only the active host accepts transfer requests and writes persistent accounts. Client snapshots are accepted only from the game's joined host, and each recipient gets only its own balance.

HELLO is sent every second. Host snapshots are sent roughly every 250ms. Purchases and transfers wait while required peers are missing the handshake. A client predicts only its own debit. The host's counted vanilla action sequence is included in snapshots, so unacknowledged client purchases remain subtracted until the corresponding game action is processed. Only one transfer request is pending locally; retries preserve its ID, recipient, amount, and host epoch.

The ledger commits both balances and the last request receipt in one atomic file replacement before replying with success. Identical replay returns the stored result without another debit; stale or conflicting replays are rejected. Host epochs prevent automatic replay of an old request against a new loaded session. The full sidecar is checksummed and validated before replacing live account state. This guards accidental corruption; it is not tamper-proofing against a host editing their own save.

## Limits that require runtime validation

The Windows game and Steam multiplayer were not executed in this Linux environment. ABI extraction and native cross-compilation are evidence of compatibility work, not proof of functional gameplay. In particular, all original callback/event ordering, client purchase reconciliation, save-slot transitions, and real Steam session behavior require the supplied manual integration procedure.

The sidecar and the vanilla game save are independently committed. A process crash between them can leave different save times; restoring both from a consistent backup is the supported recovery. Enforcing a transaction across all game inventory/world saves is outside this implementation. Transfer replay receipts are part of the sidecar transaction.

The normal game snapshot receives the sum of individual wallets for a reversible return to shared-wallet play, but that sum represents the last vanilla save, not every newer sidecar write. Unmodded play does not update the sidecar. All participants must use this mod. Offline recipients, cross-save/cross-host balances, game-version auto-detection, and interoperability with other native hooks are not implemented.


## v0.3 in-game UI

The Win32 window, message loop, OS confirmation dialog, and global key polling were removed from the native plugin. The panel is a Unity uGUI canvas built by the game's own MoonWorkshopKit. It reuses the actual workshop fonts, Cream/Muted/Cyan/Orange palette, Plate/Socket/Coin assets, native buttons and hover/click behavior. It uses a TMP_InputField and the game's InputSystem EventSystem. No managed interop assemblies or injected MonoBehaviour type are required.

`il2cpp_ui.hpp` invokes metadata-resolved original methods through the supported IL2CPP runtime API, boxes/unboxes value results, dispatches virtual methods using the object's virtual method, and retains persistent managed objects using GC handles. `ui_contract.hpp` preflights required APIs, and `validate_ui_contract.py` checks their real signatures and inherited methods against the supplied private LibCpp2IL metadata dump. The dump and original game binaries are not distributed.

The exact-build Button.Press entry is detoured; unrelated game buttons immediately forward to the original. Owned buttons retain the original native click path (including mouse-button filtering in OnPointerClick, interactability, submit behavior, audio callbacks and transitions). The game's button closure is verified to null-check its Action callback, allowing the mod to dispatch its own queued UI actions without inventing IL2CPP delegate layouts.

The wallet model separates draft, immutable review, sending and result states. Recipient IDs remain stable across roster ordering, a departed reviewed recipient invalidates confirmation, and the latest local balance is rechecked at confirmation. The ledger remains authoritative. Input editing is native TMP; shortcuts and full-balance entry use integer cents without floating point. Enter advances only from the amount field to review; actual submission requires the explicit native Confirm button. Long names use autosizing/ellipsis and untrusted name markup is neutralized.

While open, the plugin holds MoonInput.Typing and MoonPauseMenu.BlocksGameplay, unlocks/shows the cursor, and restores the previous cursor, typing and selected UI object when closed. Other cursor setters are constrained only while this panel owns input. It creates an InputSystem EventSystem only when none exists and destroys only the one it created. Scene/save generations close the panel and reset draft state. There is no Time.timeScale change; the multiplayer world continues running.

The UI model and API contract checks are not substitutes for actually rendering and using the UI in Windows. That integration remains unexecuted in this cloud environment. There is intentionally no fallback to an external window if the game UI API fails; the failure is logged and input is released.
