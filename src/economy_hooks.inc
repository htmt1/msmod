// Generated native-ABI wrappers; see tools/generate_hooks.py.
using economy_0_fn=void(*)(Obj, void*);
economy_0_fn economy_0_original=nullptr;
void economy_0(Obj world, void* method) {
    Scope s(world,nullptr,false,false);
    economy_0_original(world, method);
}
using economy_1_fn=bool(*)(Obj, Obj, Obj, float, Obj*, Obj, int32_t, void*);
economy_1_fn economy_1_original=nullptr;
bool economy_1(Obj world, Obj builder, Obj direction, float yaw, Obj* reason, Obj checkedSite, int32_t kind, void* method) {
    if(fatal || (active && !synchronized())) {
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr),false,false);
    return economy_1_original(world, builder, direction, yaw, reason, checkedSite, kind, method);
}
using economy_2_fn=bool(*)(Obj, Obj, Obj, Obj*, Obj*, Dec*, void*);
economy_2_fn economy_2_original=nullptr;
bool economy_2(Obj world, Obj builder, Obj plan, Obj* reason, Obj* path, Dec* price, void* method) {
    if(fatal || (active && !synchronized())) {
        if(path)*path={};
        if(price)*price={};
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr),false,false);
    return economy_2_original(world, builder, plan, reason, path, price, method);
}
using economy_3_fn=bool(*)(Obj, Obj, Obj, Obj*, Obj*, Dec*, bool*, void*);
economy_3_fn economy_3_original=nullptr;
bool economy_3(Obj world, Obj builder, Obj plan, Obj* reason, Obj* path, Dec* price, bool* routable, void* method) {
    if(fatal || (active && !synchronized())) {
        if(path)*path={};
        if(price)*price={};
        if(routable)*routable={};
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr),false,false);
    return economy_3_original(world, builder, plan, reason, path, price, routable, method);
}
using economy_4_fn=bool(*)(Obj, Obj, Obj, Obj, void*);
economy_4_fn economy_4_original=nullptr;
bool economy_4(Obj world, Obj builder, Obj plan, Obj route, void* method) {
    if(fatal || (active && !synchronized())) {
        return false;
    }
    Scope s(world,(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr),false,false);
    return economy_4_original(world, builder, plan, route, method);
}
using economy_5_fn=bool(*)(Obj, Obj, Obj, Obj*, Obj, void*);
economy_5_fn economy_5_original=nullptr;
bool economy_5(Obj world, Obj builder, Obj plans, Obj* reason, Obj ids, void* method) {
    if(fatal || (active && !synchronized())) {
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr),false,false);
    return economy_5_original(world, builder, plans, reason, ids, method);
}
using economy_6_fn=bool(*)(Obj, Obj, Obj, Obj, int32_t, void*);
economy_6_fn economy_6_original=nullptr;
bool economy_6(Obj world, Obj player, Obj kind, Obj id, int32_t level, void* method) {
    if(fatal || (active && !synchronized())) {
        return false;
    }
    Scope s(world,player,false,false);
    return economy_6_original(world, player, kind, id, level, method);
}
using economy_7_fn=void(*)(Obj, Obj, Obj, void*);
economy_7_fn economy_7_original=nullptr;
void economy_7(Obj world, Obj player, Obj amount, void* method) {
    Scope s(world,player,false,false);
    economy_7_original(world, player, amount, method);
}
using economy_8_fn=void(*)(Obj, Obj, Obj, void*);
economy_8_fn economy_8_original=nullptr;
void economy_8(Obj world, Obj player, Obj amount, void* method) {
    Scope s(world,player,false,false);
    economy_8_original(world, player, amount, method);
}
using economy_9_fn=void(*)(Obj, Obj, Obj, void*);
economy_9_fn economy_9_original=nullptr;
void economy_9(Obj world, Obj playerId, Obj dollars, void* method) {
    Scope s(world,playerId,false,false);
    economy_9_original(world, playerId, dollars, method);
}
using economy_10_fn=void(*)(Obj, int32_t, Obj, void*);
economy_10_fn economy_10_original=nullptr;
void economy_10(Obj world, int32_t region, Obj cleaner, void* method) {
    Scope s(world,cleaner,false,false);
    economy_10_original(world, region, cleaner, method);
}
using economy_11_fn=bool(*)(Obj, Obj, void*);
economy_11_fn economy_11_original=nullptr;
bool economy_11(Obj world, Obj fraction, void* method) {
    Scope s(world,nullptr,true,false);
    return economy_11_original(world, fraction, method);
}
using economy_12_fn=bool(*)(Obj, Obj, Obj, Obj, Obj*, void*);
economy_12_fn economy_12_original=nullptr;
bool economy_12(Obj world, Obj builder, Obj plan, Obj check, Obj* reason, void* method) {
    if(fatal || (active && !synchronized())) {
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr),false,false);
    return economy_12_original(world, builder, plan, check, reason, method);
}
using economy_13_fn=bool(*)(Obj, Obj, int32_t, int32_t, Obj*, void*);
economy_13_fn economy_13_original=nullptr;
bool economy_13(Obj world, Obj player, int32_t garage, int32_t vehicle, Obj* message, void* method) {
    if(fatal || (active && !synchronized())) {
        if(message)*message=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_13_original(world, player, garage, vehicle, message, method);
}
using economy_14_fn=void(*)(Obj, void*);
economy_14_fn economy_14_original=nullptr;
void economy_14(Obj world, void* method) {
    Scope s(world,nullptr,false,false);
    economy_14_original(world, method);
}
using economy_15_fn=bool(*)(Obj, Obj, int32_t, Obj*, void*);
economy_15_fn economy_15_original=nullptr;
bool economy_15(Obj world, Obj player, int32_t index, Obj* reason, void* method) {
    if(fatal || (active && !synchronized())) {
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_15_original(world, player, index, reason, method);
}
using economy_16_fn=bool(*)(Obj, Obj, int32_t, void*);
economy_16_fn economy_16_original=nullptr;
bool economy_16(Obj world, Obj player, int32_t index, void* method) {
    if(fatal || (active && !synchronized())) {
        return false;
    }
    Scope s(world,player,false,false);
    return economy_16_original(world, player, index, method);
}
using economy_17_fn=void(*)(Obj, bool, void*);
economy_17_fn economy_17_original=nullptr;
void economy_17(Obj world, bool pay, void* method) {
    Scope s(world,nullptr,true,false);
    economy_17_original(world, pay, method);
}
using economy_18_fn=Obj(*)(Obj, void*);
economy_18_fn economy_18_original=nullptr;
Obj economy_18(Obj world, void* method) {
    Scope s(world,nullptr,true,true);
    return economy_18_original(world, method);
}
using economy_19_fn=Obj(*)(Obj, bool, void*);
economy_19_fn economy_19_original=nullptr;
Obj economy_19(Obj world, bool completeStrokes, void* method) {
    Scope s(world,nullptr,true,true);
    return economy_19_original(world, completeStrokes, method);
}
using economy_20_fn=bool(*)(Obj, Obj, Obj*, void*);
economy_20_fn economy_20_original=nullptr;
bool economy_20(Obj world, Obj player, Obj* reason, void* method) {
    if(fatal || (active && !synchronized())) {
        if(reason)*reason=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_20_original(world, player, reason, method);
}
using economy_21_fn=bool(*)(Obj, Obj, Obj, int32_t, Obj*, void*);
economy_21_fn economy_21_original=nullptr;
bool economy_21(Obj world, Obj player, Obj toolId, int32_t expectedTier, Obj* message, void* method) {
    if(fatal || (active && !synchronized())) {
        if(message)*message=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_21_original(world, player, toolId, expectedTier, message, method);
}
using economy_22_fn=void(*)(Obj, Obj, Obj, void*);
economy_22_fn economy_22_original=nullptr;
void economy_22(Obj world, Obj player, Obj dollars, void* method) {
    Scope s(world,player,false,false);
    economy_22_original(world, player, dollars, method);
}
using economy_23_fn=void(*)(Obj, Obj, Obj, void*);
economy_23_fn economy_23_original=nullptr;
void economy_23(Obj world, Obj player, Obj dollars, void* method) {
    Scope s(world,player,false,false);
    economy_23_original(world, player, dollars, method);
}
using economy_24_fn=bool(*)(Obj, Obj, Obj, int32_t, Obj*, void*);
economy_24_fn economy_24_original=nullptr;
bool economy_24(Obj world, Obj player, Obj id, int32_t expectedLevel, Obj* message, void* method) {
    if(fatal || (active && !synchronized())) {
        if(message)*message=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_24_original(world, player, id, expectedLevel, message, method);
}
using economy_25_fn=bool(*)(Obj, Obj, Obj, int32_t, Obj*, void*);
economy_25_fn economy_25_original=nullptr;
bool economy_25(Obj world, Obj player, Obj id, int32_t expectedLevel, Obj* message, void* method) {
    if(fatal || (active && !synchronized())) {
        if(message)*message=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_25_original(world, player, id, expectedLevel, message, method);
}
using economy_26_fn=void(*)(Obj, Obj, void*);
economy_26_fn economy_26_original=nullptr;
void economy_26(Obj world, Obj snapshot, void* method) {
    Scope s(world,nullptr,true,false);
    economy_26_original(world, snapshot, method);
}
using economy_27_fn=bool(*)(Obj, Obj, Obj, Obj, int32_t, Obj*, void*);
economy_27_fn economy_27_original=nullptr;
bool economy_27(Obj world, Obj player, Obj storage, Obj kind, int32_t expectedLevel, Obj* message, void* method) {
    if(fatal || (active && !synchronized())) {
        if(message)*message=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");
        return false;
    }
    Scope s(world,player,false,false);
    return economy_27_original(world, player, storage, kind, expectedLevel, message, method);
}
using economy_28_fn=void(*)(Obj, Obj, Obj, int32_t, void*);
economy_28_fn economy_28_original=nullptr;
void economy_28(Obj world, Obj player, Obj id, int32_t level, void* method) {
    Scope s(world,player,false,false);
    economy_28_original(world, player, id, level, method);
}
bool installEconomyHooks(){bool ok=true;
    ok=hook(0x7133B0,reinterpret_cast<void*>(economy_0),reinterpret_cast<void**>(&economy_0_original))&&ok; // RetireDustDepots
    ok=hook(0x73C950,reinterpret_cast<void*>(economy_1),reinterpret_cast<void**>(&economy_1_original))&&ok; // TryBuildDepot
    ok=hook(0x6A2C90,reinterpret_cast<void*>(economy_2),reinterpret_cast<void**>(&economy_2_original))&&ok; // CheckConveyor
    ok=hook(0x6A2860,reinterpret_cast<void*>(economy_3),reinterpret_cast<void**>(&economy_3_original))&&ok; // CheckConveyor
    ok=hook(0x715CA0,reinterpret_cast<void*>(economy_4),reinterpret_cast<void**>(&economy_4_original))&&ok; // RouteConveyor
    ok=hook(0x73BDA0,reinterpret_cast<void*>(economy_5),reinterpret_cast<void**>(&economy_5_original))&&ok; // TryBuildConveyorRoute
    ok=hook(0x6B2D00,reinterpret_cast<void*>(economy_6),reinterpret_cast<void**>(&economy_6_original))&&ok; // CrewPurchase
    ok=hook(0x6B1730,reinterpret_cast<void*>(economy_7),reinterpret_cast<void**>(&economy_7_original))&&ok; // CrewBuildMoney
    ok=hook(0x6A2110,reinterpret_cast<void*>(economy_8),reinterpret_cast<void**>(&economy_8_original))&&ok; // ChargeBuild
    ok=hook(0x6BAF00,reinterpret_cast<void*>(economy_9),reinterpret_cast<void**>(&economy_9_original))&&ok; // DevAddMoney
    ok=hook(0x6ED1A0,reinterpret_cast<void*>(economy_10),reinterpret_cast<void**>(&economy_10_original))&&ok; // PaySection
    ok=hook(0x6B0B10,reinterpret_cast<void*>(economy_11),reinterpret_cast<void**>(&economy_11_original))&&ok; // CreditShare
    ok=hook(0x73D420,reinterpret_cast<void*>(economy_12),reinterpret_cast<void**>(&economy_12_original))&&ok; // TryBuildStructure
    ok=hook(0x73DFC0,reinterpret_cast<void*>(economy_13),reinterpret_cast<void**>(&economy_13_original))&&ok; // TryBuyVehicle
    ok=hook(0x7138B0,reinterpret_cast<void*>(economy_14),reinterpret_cast<void**>(&economy_14_original))&&ok; // RetireHexPieces
    ok=hook(0x7410A0,reinterpret_cast<void*>(economy_15),reinterpret_cast<void**>(&economy_15_original))&&ok; // TryRaiseLandmark
    ok=hook(0x6B2F60,reinterpret_cast<void*>(economy_16),reinterpret_cast<void**>(&economy_16_original))&&ok; // CrewRaiseLandmark
    ok=hook(0x6ECE70,reinterpret_cast<void*>(economy_17),reinterpret_cast<void**>(&economy_17_original))&&ok; // PayPremiums
    ok=hook(0x719C50,reinterpret_cast<void*>(economy_18),reinterpret_cast<void**>(&economy_18_original))&&ok; // SaveProgress
    ok=hook(0x719420,reinterpret_cast<void*>(economy_19),reinterpret_cast<void**>(&economy_19_original))&&ok; // SaveProgress
    ok=hook(0x73FCA0,reinterpret_cast<void*>(economy_20),reinterpret_cast<void**>(&economy_20_original))&&ok; // TryPayTeleport
    ok=hook(0x73FF00,reinterpret_cast<void*>(economy_21),reinterpret_cast<void**>(&economy_21_original))&&ok; // TryPurchaseContainer
    ok=hook(0x6ED4A0,reinterpret_cast<void*>(economy_22),reinterpret_cast<void**>(&economy_22_original))&&ok; // PayTrick
    ok=hook(0x6B3BE0,reinterpret_cast<void*>(economy_23),reinterpret_cast<void**>(&economy_23_original))&&ok; // CrewTrickMoney
    ok=hook(0x740890,reinterpret_cast<void*>(economy_24),reinterpret_cast<void**>(&economy_24_original))&&ok; // TryPurchaseTreeNode
    ok=hook(0x7403B0,reinterpret_cast<void*>(economy_25),reinterpret_cast<void**>(&economy_25_original))&&ok; // TryPurchaseTool
    ok=hook(0x7122F0,reinterpret_cast<void*>(economy_26),reinterpret_cast<void**>(&economy_26_original))&&ok; // RestoreUpgrades
    ok=hook(0x741A10,reinterpret_cast<void*>(economy_27),reinterpret_cast<void**>(&economy_27_original))&&ok; // TryUpgradeVehicle
    ok=hook(0x7084D0,reinterpret_cast<void*>(economy_28),reinterpret_cast<void**>(&economy_28_original))&&ok; // RefundOldVehicleTree
return ok;}
