#include "windows_util.hpp"
#include "decimal.hpp"
#include "ledger.hpp"
#include "protocol.hpp"
#include "storage.hpp"
#include "startup_policy.hpp"
#include "wallet_ui_model.hpp"
#include "il2cpp_ui.hpp"
#include "ui_build.hpp"
#include "ui_contract.hpp"
#include "../third_party/minhook/include/MinHook.h"
#include <algorithm>
#include <atomic>
#include <sstream>

using wallet::Kind;using wallet::Result;
namespace {
uintptr_t base=0;HMODULE ownModule=nullptr;
template<class F> F fn(uintptr_t rva){return reinterpret_cast<F>(base+rva);}
template<class T>T& field(void* p,uintptr_t offset){return *reinterpret_cast<T*>(uintptr_t(p)+offset);}
using Obj=void*;using Str=void*;
namespace ingame { void transferFinished(Result); void transferAborted(const wchar_t*); }
std::wstring str(Str s){if(!s)return {};auto n=field<int32_t>(s,16);if(n<0 || n>32768)return {};return std::wstring(reinterpret_cast<wchar_t*>(uintptr_t(s)+20),n);}
uint64_t self(){return fn<uint64_t(*)(void*)>(game::SteamId)(nullptr);}
bool joined(){return fn<bool(*)(void*)>(game::Joined)(nullptr);}
uint64_t host(){auto id=joined()?fn<uint64_t(*)(void*)>(game::CrewHost)(nullptr):self();return id?id:self();}
bool authority(){return !joined();}
uint64_t account(Str s){auto name=str(s);if(name.empty()||name==L"local")return host();
    if(name.rfind(L"steam-",0)!=0)return 0;name.erase(0,6);if(name.empty()||name.size()>20)return 0;
    uint64_t v=0;for(auto c:name){if(c<L'0'||c>L'9'||v>(UINT64_MAX-uint64_t(c-L'0'))/10)return 0;v=v*10+c-L'0';}return v;
}
struct Peer {uint64_t id=0;std::wstring name;};
std::vector<Peer> roster;
wallet::Ledger<Dec> ledger;
Obj currentWorld=nullptr;std::wstring savePath;
bool active=false,fatal=false,restoring=false,firstUpdate=true,sawMenu=false,wasBound=false,steamChecked=false;
uint64_t epoch=0,serverEpoch=0,lastSnapshot=0,lastBroadcast=0,lastHello=0;
std::map<uint64_t,uint64_t> helloAt;
std::map<uint64_t,uint32_t> hostAck;
std::map<uint32_t,Dec> predictions;
std::wstring status=L"自動読み込みが完了しました。タイトル画面からセーブを読み込んでください。";

void log(const std::wstring& message){
    auto path=win::directory(win::modulePath(ownModule))+L"\\MoonWallet.log";
    HANDLE h=CreateFileW(path.c_str(),FILE_APPEND_DATA,FILE_SHARE_READ,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(h!=INVALID_HANDLE_VALUE){auto s=win::utf8(message)+"\r\n";DWORD n;WriteFile(h,s.data(),DWORD(s.size()),&n,nullptr);CloseHandle(h);}
}
void fail(const std::wstring& message){fatal=true;active=false;status=message;log(message);}
Dec& rawBalance(Obj world){auto w=field<Obj>(world,game::WorldWallet);return field<Dec>(w,game::WalletBalance);}

bool persist(const wallet::Ledger<Dec>::Accounts& next){
    if(!authority())return true;
    if(savePath.empty()||next.size()>256)return false;
    return win::atomicWrite(savePath,wallet::encodeAccounts<Dec>(next));
}
bool synchronized(){
    if(!active||fatal)return false;
    auto now=GetTickCount64();
    if(!authority())return serverEpoch && now-lastSnapshot<5000 && predictions.empty();
    for(auto& p:roster)if(p.id!=self() && (!helloAt.count(p.id)||now-helloAt[p.id]>10000))return false;
    return true;
}
void load(Obj world){
    currentWorld=world;active=false;wasBound=false;predictions.clear();hostAck.clear();fatal=false;ledger.accounts.clear();helloAt.clear();serverEpoch=0;lastSnapshot=0;
    epoch=win::timeId();savePath.clear();
    if(!sawMenu){fail(L"セーブ読込前にModを起動してください。ゲームを再起動してタイトル画面で起動してください。");return;}
    if(!self()){fail(L"SteamのプレイヤーIDを取得できません。");return;}
    ledger.persist=persist;
    if(authority()){
        auto root=str(fn<Str(*)(Obj,void*)>(game::SaveDirectory)(world,nullptr));
        if(root.empty()){fail(L"セーブ先を取得できないため、ウォレットを開始できません。");return;}
        if(!CreateDirectoryW(root.c_str(),nullptr) && GetLastError()!=ERROR_ALREADY_EXISTS){fail(L"ウォレットの保存フォルダーを準備できません。");return;}
        savePath=root+L"\\MoonWallet-v1.dat";
        auto attrs=GetFileAttributesW(savePath.c_str());
        if(attrs!=INVALID_FILE_ATTRIBUTES){std::vector<uint8_t> bytes;
            if(!win::read(savePath,bytes,1024*1024)||!wallet::decodeAccounts<Dec>(bytes,ledger.accounts)){fail(L"ウォレット保存ファイルを読み込めません。上書きせず停止しました。MoonWallet-v1.datと.bakを確認してください。");return;}
        }else{
            DWORD error=GetLastError();if(error!=ERROR_FILE_NOT_FOUND && error!=ERROR_PATH_NOT_FOUND){fail(L"ウォレット保存先へのアクセスが拒否されました。");return;}
            auto initial=rawBalance(world);if(!initial.bounded()||initial<Dec{}){fail(L"既存残高を安全に移行できません。");return;}
            ledger.accounts[self()].balance=initial;
            if(!persist(ledger.accounts)){fail(L"ウォレットの初回保存に失敗しました。");return;}
            log(L"既存の共有残高をホストへ移行しました: "+initial.text());
        }
    }
    active=true;rawBalance(world)=ledger.balance(self());status=authority()?L"個人ウォレットを読み込みました。":L"ホストからのウォレット同期を待っています。";
    log(L"ウォレットを開始しました。");
}

// Getter inlining exists in this build. A scope presents the correct account to every
// original affordability check. Multi-recipient payout loops retain a fixed baseline;
// the setter converts each absolute value into a delta for its actual player argument.
struct Scope;
thread_local Scope* scope=nullptr;
struct Scope {
    Obj world=nullptr;uint64_t actor=0;bool aggregate=false;Dec baseline{};Scope* parent=nullptr;
    Scope(Obj w,Str player=nullptr,bool multiple=false,bool total=false):aggregate(multiple){
        if(!active||fatal||restoring||w!=currentWorld)return;
        world=w;actor=player?account(player):self();parent=scope;scope=this;
        baseline=ledger.balance(actor);
        if(total){baseline={};for(auto& [id,a]:ledger.accounts){Dec next;if(Dec::add(baseline,a.balance,next))baseline=next;}}
        rawBalance(world)=baseline;
    }
    ~Scope(){
        if(!world)return;
        auto after=rawBalance(world);
        // Legacy decommission refunds write Balance directly without invoking the setter.
        if(!(after==baseline)){
            Dec delta;if(Dec::sub(after,baseline,delta)&&authority() && !ledger.delta(actor,delta))fail(L"報酬の保存に失敗しました。ゲームを終了してログを確認してください。");
        }
        scope=parent;
        if(parent && parent->world==world){
            if(!parent->aggregate)parent->baseline=ledger.balance(parent->actor);
            rawBalance(world)=parent->baseline;
        }else rawBalance(world)=ledger.balance(self());
    }
};
using Setter=void(*)(Obj,Str,const Dec*,void*);Setter originalSet=nullptr;
void setWallet(Obj wallet,Str player,const Dec* value,void* method){
    if(!active||restoring){if(!fatal)originalSet(wallet,player,value,method);return;}
    if(fatal||!currentWorld || wallet!=field<Obj>(currentWorld,game::WorldWallet))return;
    auto id=account(player);Dec before=field<Dec>(wallet,game::WalletBalance),delta;
    if(!id||!Dec::sub(*value,before,delta))return;
    auto old=ledger.balance(id);Dec next;
    if(authority()){
        if(!ledger.delta(id,delta)){fail(L"残高更新を保存できませんでした。ウォレットを停止しました。");return;}
        next=ledger.balance(id);
    }else{
        // Clients only predict their own spending. Income arrives from the host snapshot.
        if(id==self() && delta<Dec{} && Dec::add(old,delta,next) && !(next<Dec{}))ledger.accounts[id].balance=next;
        else next=old;
        if(id==self() && delta<Dec{}){
            auto session=fn<Obj(*)(void*)>(game::SessionInstance)(nullptr);
            if(session){auto seq=field<uint32_t>(session,0x230)+1;Dec sum;auto previous=predictions[seq];if(Dec::add(previous,delta,sum))predictions[seq]=sum;}
        }
    }
    // Preserve vanilla events/achievement callbacks with a per-player delta.
    field<Dec>(wallet,game::WalletBalance)=old;
    originalSet(wallet,player,&next,method);
    if(scope){if(!scope->aggregate)scope->baseline=ledger.balance(scope->actor);field<Dec>(wallet,game::WalletBalance)=scope->baseline;}
    else field<Dec>(wallet,game::WalletBalance)=ledger.balance(self());
}
using Getter=Dec*(*)(Dec*,Obj,Str,void*);Getter originalGet=nullptr;
Dec* walletGet(Dec* out,Obj w,Str player,void* m){if(!active&&!fatal)return originalGet(out,w,player,m);*out=active?ledger.balance(account(player)):Dec{};return out;}
Dec* worldGet(Dec* out,Obj world,Str player,void*){*out=fatal?Dec{}:((active&&world==currentWorld)?ledger.balance(account(player)):rawBalance(world));return out;}
using SetTeam=void(*)(Obj,const Dec*,void*);SetTeam originalSetTeam=nullptr;
void setTeam(Obj w,const Dec* d,void* m){if(!active && !fatal)originalSetTeam(w,d,m);}
using Economy=void(*)(Obj,Obj,void*);Economy originalRestore=nullptr,originalSnapshot=nullptr;
void restoreEconomy(Obj w,Obj snapshot,void* m){
    restoring=true;originalRestore(w,snapshot,m);restoring=false;load(w);
}
void snapshotEconomy(Obj w,Obj snapshot,void* m){Scope s(w,nullptr,true,true);originalSnapshot(w,snapshot,m);}

// Steam flat API transport uses a separate channel; the sender identity comes from Steam,
// never from a claimed sender field in a mod packet.
struct Identity {int32_t type=16,size=8;uint64_t id=0;uint8_t reserved[120]{};};
static_assert(sizeof(Identity)==136);
struct SteamMessagePrefix {void* data;int32_t size;uint32_t connection;Identity peer;};
static_assert(offsetof(SteamMessagePrefix,peer)==16);
struct SteamApi {
    void* messages=nullptr;void* matchmaking=nullptr;void* friends=nullptr;
    int(*send)(void*,const Identity*,const void*,uint32_t,int,int)=nullptr;
    int(*receive)(void*,int,void**,int)=nullptr;
    void(*release)(void*)=nullptr;
    int(*count)(void*,uint64_t)=nullptr;
    uint64_t(*member)(void*,uint64_t,int)=nullptr;
    const char*(*name)(void*,uint64_t)=nullptr;
} steam;
template<class F>F steamExport(HMODULE h,const char* n){return reinterpret_cast<F>(GetProcAddress(h,n));}
bool initSteam(){
    HMODULE h=GetModuleHandleW(L"steam_api64.dll");if(!h)return false;
    auto messages=steamExport<void*(*)()>(h,"SteamAPI_SteamNetworkingMessages_SteamAPI_v002");
    // Official flat API name (Steamworks SDK).
    if(!messages)messages=steamExport<void*(*)()>(h,"SteamAPI_SteamNetworkingMessages_v002");
    auto match=steamExport<void*(*)()>(h,"SteamAPI_SteamMatchmaking_v009");
    auto friends=steamExport<void*(*)()>(h,"SteamAPI_SteamFriends_v017");
    auto user=steamExport<int(*)()>(h,"SteamAPI_GetHSteamUser");
    auto find=steamExport<void*(*)(int,const char*)>(h,"SteamInternal_FindOrCreateUserInterface");
    steam.send=steamExport<decltype(steam.send)>(h,"SteamAPI_ISteamNetworkingMessages_SendMessageToUser");
    steam.receive=steamExport<decltype(steam.receive)>(h,"SteamAPI_ISteamNetworkingMessages_ReceiveMessagesOnChannel");
    steam.release=steamExport<decltype(steam.release)>(h,"SteamAPI_SteamNetworkingMessage_t_Release");
    steam.count=steamExport<decltype(steam.count)>(h,"SteamAPI_ISteamMatchmaking_GetNumLobbyMembers");
    steam.member=steamExport<decltype(steam.member)>(h,"SteamAPI_ISteamMatchmaking_GetLobbyMemberByIndex");
    steam.name=steamExport<decltype(steam.name)>(h,"SteamAPI_ISteamFriends_GetFriendPersonaName");
    if(((!messages||!match||!friends)&&(!user||!find))||!steam.send||!steam.receive||!steam.release||!steam.count||!steam.member||!steam.name)return false;
    steam.messages=messages?messages():find(user(),"SteamNetworkingMessages002");steam.matchmaking=match?match():find(user(),"SteamMatchMaking009");steam.friends=friends?friends():find(user(),"SteamFriends017");return steam.messages&&steam.matchmaking&&steam.friends;
}
bool member(uint64_t id){return std::any_of(roster.begin(),roster.end(),[id](auto& p){return p.id==id;});}
bool send(uint64_t id,const wallet::Writer& w){Identity peer;peer.id=id;return steam.send(steam.messages,&peer,w.bytes.data(),uint32_t(w.bytes.size()),8,wallet::Channel)==1;}
void refreshRoster(){
    roster.clear();uint64_t me=self();if(!me)return;
    auto lobby=fn<uint64_t(*)(void*)>(game::LobbyId)(nullptr);
    if(lobby){int count=steam.count(steam.matchmaking,lobby);if(count<0||count>64)return;
        for(int i=0;i<count;++i){uint64_t id=steam.member(steam.matchmaking,lobby,i);if(id){const char* n=steam.name(steam.friends,id);roster.push_back({id,win::utf16(n?n:"")});}}
    }
    if(!member(me))roster.push_back({me,L"自分"});
    if(authority() && active){auto next=ledger.accounts;for(auto& p:roster)if(next.size()<256)next.try_emplace(p.id);
        if(next.size()!=ledger.accounts.size()){if(persist(next))ledger.accounts.swap(next);else fail(L"参加者のウォレットを保存できません。");}}
}
struct Pending {uint64_t request=0,target=0,epoch=0;int64_t cents=0;uint64_t lastSent=0;} pending;
Pending uiCommand;
void broadcast(){
    if(!active||!authority())return;
    // Each peer receives only its own balance, never other players' wallet contents.
    for(auto& p:roster)if(p.id!=self() && helloAt.count(p.id)){
        wallet::Writer w(Kind::Snapshot,epoch);auto a=ledger.accounts[p.id];
        w.u64(p.id);w.raw(&a.balance,16);w.u64(a.last.request);w.u8(uint8_t(a.last.result));w.u32(hostAck[p.id]);send(p.id,w);
    }
}
void receive(){
    for(int batch=0;batch<8;++batch){void* messages[16]{};int n=steam.receive(steam.messages,wallet::Channel,messages,16);if(n<=0)break;
        for(int i=0;i<n;++i){auto msg=static_cast<SteamMessagePrefix*>(messages[i]);
            auto handle=[&](){
                if(!msg||msg->size<14||msg->size>1024||msg->peer.type!=16||msg->peer.size!=8||!member(msg->peer.id))return;
                auto from=msg->peer.id;wallet::Reader r(msg->data,size_t(msg->size));
                if(r.u32()!=wallet::Magic||r.u8()!=1)return;auto kind=Kind(r.u8());auto incomingEpoch=r.u64();
                if(authority()){
                    if(kind==Kind::Hello && r.done()){helloAt[from]=GetTickCount64();return;}
                    if(kind!=Kind::Transfer)return;
                    auto request=r.u64(),to=r.u64(),cents=r.u64();
                    if(!r.done()||!request||!cents||cents>99999999999999ull)return;
                    Result result=Result::NotReady;
                    if(incomingEpoch==epoch && synchronized() && member(to) && helloAt.count(from))result=ledger.transfer(from,to,Dec::cents(cents),request);
                    wallet::Writer reply(Kind::Reply,epoch);reply.u64(request);reply.u8(uint8_t(result));send(from,reply);broadcast();
                }else{
                    if(from!=host())return;
                    if(kind==Kind::Snapshot){auto id=r.u64();Dec balance;r.raw(&balance,16);auto request=r.u64();auto result=Result(r.u8());auto ack=r.u32();
                        if(!r.done()||!incomingEpoch||id!=self()||!balance.bounded()||balance<Dec{}||uint8_t(result)>uint8_t(Result::NotReady))return;
                        if(serverEpoch && serverEpoch!=incomingEpoch && pending.request){pending={};status=L"ホストのセッションが変わりました。残高を確認してください。";ingame::transferAborted(status.c_str());}
                        serverEpoch=incomingEpoch;lastSnapshot=GetTickCount64();for(auto i=predictions.begin();i!=predictions.end();){if(i->first<=ack)i=predictions.erase(i);else ++i;}
                        for(auto& [seq,delta]:predictions){Dec next;if(Dec::add(balance,delta,next))balance=next;}
                        if(balance<Dec{})balance={};
                        ledger.accounts[id].balance=balance;
                        if(pending.request && request==pending.request){pending={};status=wallet::message(result);ingame::transferFinished(result);}
                        rawBalance(currentWorld)=ledger.balance(self());
                    }else if(kind==Kind::Reply){auto request=r.u64();auto result=Result(r.u8());
                        if(!r.done()||incomingEpoch!=serverEpoch||uint8_t(result)>uint8_t(Result::NotReady))return;
                        if(pending.request==request){pending={};status=wallet::message(result);ingame::transferFinished(result);}
                    }
                }
            };handle();if(msg)steam.release(msg);
        }
        if(n<16)break;
    }
}

#include "game_ui.inc"
using Update=void(*)(Obj,void*);Update originalUpdate=nullptr;
void sessionUpdate(Obj session,void* method){
    if(firstUpdate){firstUpdate=false;
        if(!sawMenu)fail(L"セーブが開いています。ゲームを再起動し、タイトル画面でModを起動してください。");
    }
    if(!steamChecked && self()){steamChecked=true;if(!initSteam())fail(L"Steam通信APIを取得できません。MoonWallet.logを確認してください。");}
    if(!field<Obj>(session,game::SessionWorld))sawMenu=true;
    if(!active&&!fatal&&field<Obj>(session,game::SessionWorld))load(field<Obj>(session,game::SessionWorld));
    // Refresh authenticated lobby membership before polling the mod channel.
    if(steam.messages){auto now=GetTickCount64();
        if(now-lastHello>1000){refreshRoster();if(active&&!authority()){wallet::Writer hello(Kind::Hello,0);send(host(),hello);}lastHello=now;}
        if(active)receive();
        Pending command=uiCommand;uiCommand={};
        if(command.request && !pending.request){
            if(!synchronized()){status=wallet::message(Result::NotReady);ingame::transferFinished(Result::NotReady);}
            else if(!member(command.target)){status=L"送金先が退出しました。";ingame::transferFinished(Result::UnknownRecipient);}
            else if(authority()){auto result=ledger.transfer(self(),command.target,Dec::cents(command.cents),command.request);status=wallet::message(result);rawBalance(currentWorld)=ledger.balance(self());broadcast();ingame::transferFinished(result);}
            else{pending=command;pending.epoch=serverEpoch;}
        }
        if(pending.request && active && !authority() && now-pending.lastSent>1500){
            if(pending.epoch!=serverEpoch){pending={};status=L"セッションが変更されました。残高を確認してください。";ingame::transferAborted(status.c_str());}
            else{wallet::Writer w(Kind::Transfer,pending.epoch);w.u64(pending.request);w.u64(pending.target);w.u64(uint64_t(pending.cents));send(host(),w);pending.lastSent=now;}
        }
        if(active&&authority()&&now-lastBroadcast>250){broadcast();lastBroadcast=now;}
    }
    originalUpdate(session,method);
    if(active && field<Obj>(session,game::SessionWorld)==currentWorld)wasBound=true;
    if(active && wasBound && !field<Obj>(session,game::SessionWorld)){active=false;currentWorld=nullptr;pending={};ledger.accounts.clear();serverEpoch=0;}
    ingame::tick();
}

std::vector<void*> gameplayHooks;
bool hook(uintptr_t rva,void* detour,void** original){
    auto target=reinterpret_cast<void*>(base+rva);
    auto result=MH_CreateHook(target,detour,original);
    if(result!=MH_OK){log(L"フック準備失敗: "+std::to_wstring(rva)+L" status="+std::to_wstring(result));return false;}
    gameplayHooks.push_back(target);return true;
}
void rollbackGameplayHooks(){for(auto target:gameplayHooks)MH_DisableHook(target);for(auto target:gameplayHooks)MH_RemoveHook(target);gameplayHooks.clear();}
using CountFn=bool(*)(Obj,uint64_t,Obj,void*);CountFn originalCount=nullptr;
bool countAction(Obj session,uint64_t from,Obj reader,void* method){auto result=originalCount(session,from,reader,method);if(active&&authority()&&result)hostAck[from]=field<uint32_t>(session,0x338);return result;}
Str newString(const wchar_t* text){auto create=reinterpret_cast<Str(*)(const wchar_t*,int32_t)>(GetProcAddress(reinterpret_cast<HMODULE>(base),"il2cpp_string_new_utf16"));return create?create(text,int32_t(wcslen(text))):nullptr;}
// Generated wrappers cover every protected economy entry point using inlined Balance.
#include "economy_hooks.inc"
// Called only from a Unity scene/MonoBehaviour callback on the game thread.
// An autoloaded plugin may arrive before il2cpp_init: never call managed game APIs on its worker.
void installGameHooks(){
    auto instance=fn<Obj(*)(void*)>(game::SessionInstance)(nullptr);
    sawMenu=!instance||!field<Obj>(instance,game::SessionWorld);
    if(!sawMenu){
        log(L"セーブ読込後にModが読み込まれたため適用を拒否しました。ゲームを再起動してください。");
        return;
    }
    bool ok=hook(game::ui::ButtonPress,reinterpret_cast<void*>(ingame::onPress),reinterpret_cast<void**>(&ingame::originalPress));
    ok=hook(game::ui::TypingGet,reinterpret_cast<void*>(ingame::typing),reinterpret_cast<void**>(&ingame::originalTypingGet))&&ok;
    ok=hook(game::ui::TypingSet,reinterpret_cast<void*>(ingame::setTyping),reinterpret_cast<void**>(&ingame::originalTypingSet))&&ok;
    ok=hook(game::ui::BlocksGameplay,reinterpret_cast<void*>(ingame::blocks),reinterpret_cast<void**>(&ingame::originalBlocks))&&ok;
    ok=hook(game::ui::CursorLockSet,reinterpret_cast<void*>(ingame::setLock),reinterpret_cast<void**>(&ingame::originalLockSet))&&ok;
    ok=hook(game::ui::CursorVisibleSet,reinterpret_cast<void*>(ingame::setVisible),reinterpret_cast<void**>(&ingame::originalVisibleSet))&&ok;
    ok=hook(game::WalletSet,reinterpret_cast<void*>(setWallet),reinterpret_cast<void**>(&originalSet));
    void* ignored=nullptr;
    ok=hook(game::WalletGet,reinterpret_cast<void*>(walletGet),reinterpret_cast<void**>(&originalGet))&&ok;
    ok=hook(game::WalletDollars,reinterpret_cast<void*>(worldGet),&ignored)&&ok;
    ok=hook(game::SetTeamDollars,reinterpret_cast<void*>(setTeam),reinterpret_cast<void**>(&originalSetTeam))&&ok;
    ok=hook(game::RestoreEconomy,reinterpret_cast<void*>(restoreEconomy),reinterpret_cast<void**>(&originalRestore))&&ok;
    ok=hook(game::SnapshotEconomy,reinterpret_cast<void*>(snapshotEconomy),reinterpret_cast<void**>(&originalSnapshot))&&ok;
    ok=hook(0x7765D0,reinterpret_cast<void*>(countAction),reinterpret_cast<void**>(&originalCount))&&ok;
    ok=installEconomyHooks()&&ok;
    ok=hook(game::SessionUpdate,reinterpret_cast<void*>(sessionUpdate),reinterpret_cast<void**>(&originalUpdate))&&ok;
    if(ok){for(auto target:gameplayHooks){if(MH_EnableHook(target)!=MH_OK){ok=false;break;}}}
    if(!ok){rollbackGameplayHooks();log(L"フックの準備に失敗しました。Modは適用されていません。");return;}
    log(L"Moon Wallet v0.3.0 experimental: ゲーム内UIとウォレットのフックを適用しました。F8で開けます。");
}
using RuntimeInvoke=Obj(*)(Obj,Obj,void**,Obj*);
RuntimeInvoke originalInvoke=nullptr;void* invokeTarget=nullptr;
using MethodName=const char*(*)(Obj);MethodName methodName=nullptr;
using MethodClass=Obj(*)(Obj);MethodClass methodClass=nullptr;
using ClassImage=Obj(*)(Obj);ClassImage classImage=nullptr;
using ImageName=const char*(*)(Obj);ImageName imageName=nullptr;
std::atomic<bool> startupPending{true};
Obj runtimeInvoke(Obj method,Obj object,void** parameters,Obj* exception){
    if(startupPending.load(std::memory_order_acquire) && method){
        auto klass=methodClass(method);auto image=klass?classImage(klass):nullptr;
        if(wallet::startupCallback(methodName(method),image?imageName(image):nullptr) && startupPending.exchange(false)){
            // Disable future entries but retain the trampoline until process exit. An in-flight
            // invocation may still use originalInvoke, so do not remove this hook/uninitialize MH.
            MH_DisableHook(invokeTarget);
            installGameHooks();
        }
    }
    return originalInvoke(method,object,parameters,exception);
}
DWORD WINAPI initialize(void*){
    auto root=win::directory(win::modulePath());
    if(win::sha256(root+L"\\GameAssembly.dll")!=game::AssemblySha256 || win::sha256(root+L"\\MoonSweepers_Data\\il2cpp_data\\Metadata\\global-metadata.dat")!=game::MetadataSha256){
        log(L"ゲームのSHA-256が対応ビルドと一致しないため適用しませんでした。");
        return 0;
    }
    HMODULE module=nullptr;auto deadline=GetTickCount64()+30000;
    while(!(module=GetModuleHandleW(L"GameAssembly.dll")) && GetTickCount64()<deadline)Sleep(20);
    if(!module){log(L"GameAssembly.dllの読み込みが30秒以内に完了しませんでした。");return 0;}
    base=reinterpret_cast<uintptr_t>(module);Dec::base=base;
    invokeTarget=reinterpret_cast<void*>(GetProcAddress(module,"il2cpp_runtime_invoke"));
    methodName=reinterpret_cast<MethodName>(GetProcAddress(module,"il2cpp_method_get_name"));
    methodClass=reinterpret_cast<MethodClass>(GetProcAddress(module,"il2cpp_method_get_class"));
    classImage=reinterpret_cast<ClassImage>(GetProcAddress(module,"il2cpp_class_get_image"));
    imageName=reinterpret_cast<ImageName>(GetProcAddress(module,"il2cpp_image_get_name"));
    if(!invokeTarget||!methodName||!methodClass||!classImage||!imageName){log(L"IL2CPP起動APIを取得できません。");return 0;}
    if(MH_Initialize()!=MH_OK){log(L"MinHookを初期化できません。");return 0;}
    if(MH_CreateHook(invokeTarget,reinterpret_cast<void*>(runtimeInvoke),reinterpret_cast<void**>(&originalInvoke))!=MH_OK || MH_EnableHook(invokeTarget)!=MH_OK){
        log(L"IL2CPP起動フックを準備できません。Modは適用されていません。");return 0;
    }
    log(L"IL2CPP起動フックを準備しました。Unityのコールバックを待っています。");return 0;
}
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){ownModule=instance;DisableThreadLibraryCalls(instance);HANDLE h=CreateThread(nullptr,0,initialize,nullptr,0,nullptr);if(h)CloseHandle(h);}return TRUE;
}
