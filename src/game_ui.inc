// Native uGUI/TMP screen using Moon Sweepers' own workshop assets.
namespace ingame {
using gameui::Color;using gameui::Class;
using Key=gameui::RootKey;
gameui::Api api;walletui::Model model;
std::atomic<bool> inputHeld{false};
bool initialized=false,unavailable=false,open=false,built=false,priorVisible=false,priorTyping=false,resultOk=false,focusResult=false;
int priorLock=0;Obj previousWorld=nullptr;uint64_t nextRefresh=0,resultUntil=0,previousEpoch=0;
std::wstring notice,resultText;std::vector<gameui::GcHandle> roots;std::map<Obj,std::wstring> labelValues;
Class kitClass=nullptr,textClass=nullptr,componentClass=nullptr,goClass=nullptr,objectClass=nullptr,inputClass=nullptr,eventClass=nullptr,buttonClass=nullptr;
Color cream{},muted{},cyan{},orange{},green{},red{};
Key kit,frame,panelRoot,hudKit,hudRoot,hudButton,balanceText,rosterTitle,recipientTitle,recipientId,amountField,hintText,remainingText,pageText,reviewButton,statusText;
Key formRoot,reviewRoot,reviewName,reviewAmount,reviewRemaining,confirmButton,backButton,resultRoot,resultTitle,resultBody;
Key rowButtons[walletui::Model::Rows],prevButton,nextButton,ownedEventSystem,priorSelection,closeButton,againButton;
enum class Action{Toggle,Close,Select,Prev,Next,Quick,All,Review,Back,Confirm,Again};
struct Click{Action action;uint64_t value=0;};std::map<Obj,Click> clicks;
void refresh();
using Press=void(*)(Obj,void*);Press originalPress=nullptr;
using TypingGet=bool(*)(void*);TypingGet originalTypingGet=nullptr;
using TypingSet=void(*)(bool,void*);TypingSet originalTypingSet=nullptr;
using Blocks=bool(*)(Obj,void*);Blocks originalBlocks=nullptr;
using LockSet=void(*)(int,void*);LockSet originalLockSet=nullptr;
using VisibleSet=void(*)(bool,void*);VisibleSet originalVisibleSet=nullptr;
Key keep(Obj value){if(!value)throw std::runtime_error("game UI returned a null object");auto h=api.retain(value);roots.push_back(h);return {h};}
Obj obj(Key k){return api.target(k.handle);}
Obj call(Class c,const char* name,Obj self,std::initializer_list<void*> args={}){return api.call(c,name,self,args);}
Obj go(Obj c){return call(componentClass,"get_gameObject",c);}
bool alive(Obj v){return v&&api.value<bool>(call(objectClass,"op_Implicit",nullptr,{v}));}
void show(Key k,bool on){if(k.handle)call(goClass,"SetActive",go(obj(k)),{&on});}
void showRoot(Key k,bool on){if(k.handle)call(goClass,"SetActive",obj(k),{&on});}
void label(Key k,const std::wstring& value){auto target=obj(k);if(!target)return;auto found=labelValues.find(target);if(found!=labelValues.end()&&found->second==value)return;call(textClass,"set_text",target,{api.text(value)});labelValues[target]=value;}
void tint(Key k,Color c){call(textClass,"set_color",obj(k),{&c});}
std::wstring safeName(std::wstring v){for(auto& c:v){if(c==L'<')c=L'＜';if(c==L'>')c=L'＞';}return v;}
Obj add(Obj component,Class type){return call(goClass,"AddComponent",go(component),{api.reflectionType(type)});}
Obj rect(Obj p,const wchar_t* n,float x,float y,float w,float h){return call(kitClass,"Rect",obj(kit),{p,api.text(n),&x,&y,&w,&h});}
Key text(Obj p,const std::wstring& v,float x,float y,float w,float h,float size,Color c,bool head=false){
    auto font=api.field<Obj>(kitClass,head?"head":"text",obj(kit));int align=257;
    auto t=keep(call(kitClass,"Text",obj(kit),{p,api.text(v),&x,&y,&w,&h,&size,&c,font,&align}));bool rich=false;call(textClass,"set_richText",obj(t),{&rich});return t;
}
void plate(Obj p,const wchar_t* n,float x,float y,float w,float h,bool stripe=false){bool screws=true;call(kitClass,"Plate",obj(kit),{p,api.text(n),&x,&y,&w,&h,&stripe,&screws});}
Key button(Obj p,const std::wstring& title,const wchar_t* key,float x,float y,float w,float h,Action action,uint64_t value=0,bool primary=false){
    auto b=keep(call(kitClass,"Button",obj(kit),{p,api.text(title),api.text(key),&x,&y,&w,&h,nullptr,&primary}));clicks[obj(b)]={action,value};
    if(action==Action::Select){auto reflected=api.reflectionType(api.type("MoonCleaners","MoonWorkshopButton"));auto visual=call(goClass,"GetComponent",go(obj(b)),{reflected});
        if(visual){auto caption=api.field<Obj>(api.type("MoonCleaners","MoonWorkshopButton"),"label",visual);bool autosize=true;float minimum=14,maximum=23;int overflow=1;
            call(textClass,"set_enableAutoSizing",caption,{&autosize});call(textClass,"set_fontSizeMin",caption,{&minimum});call(textClass,"set_fontSizeMax",caption,{&maximum});call(textClass,"set_overflowMode",caption,{&overflow});}}
    return b;
}
void setButton(Key k,const std::wstring& title,bool enabled){call(kitClass,"SetButton",nullptr,{obj(k),api.text(title),&enabled});}
void focus(Key k){auto es=call(eventClass,"get_current",nullptr);if(es)call(eventClass,"SetSelectedGameObject",es,{go(obj(k))});}
std::wstring money(Dec v){return api.string(call(api.type("MoonCleaners","MoonWorld"),"FormatMoney",nullptr,{&v}));}
bool pressed(const char* key){auto c=api.type("UnityEngine.InputSystem","Keyboard");auto keyboard=call(c,"get_current",nullptr);if(!keyboard)return false;auto control=call(c,key,keyboard);if(!control)return false;return api.value<bool>(call(api.type("UnityEngine.InputSystem.Controls","ButtonControl"),"get_wasPressedThisFrame",control));}
void init(){
    log(L"ゲーム内UI: 初期化を開始します（64bit GCハンドル）。");
    api.init(reinterpret_cast<HMODULE>(base));
    for(auto& need:gameui::Contract){auto c=api.type(need.ns,need.type);if(need.method)api.method(c,need.method,need.count);}
    kitClass=api.type("MoonCleaners","MoonWorkshopKit");textClass=api.type("TMPro","TMP_Text");
    componentClass=api.type("UnityEngine","Component");goClass=api.type("UnityEngine","GameObject");objectClass=api.type("UnityEngine","Object");
    inputClass=api.type("TMPro","TMP_InputField");eventClass=api.type("UnityEngine.EventSystems","EventSystem");buttonClass=api.type("UnityEngine.UI","Button");
    cream=api.field<Color>(kitClass,"Cream",nullptr);muted=api.field<Color>(kitClass,"Muted",nullptr);cyan=api.field<Color>(kitClass,"Cyan",nullptr);
    orange=api.field<Color>(kitClass,"Orange",nullptr);green=api.field<Color>(kitClass,"Green",nullptr);red=api.field<Color>(kitClass,"Red",nullptr);initialized=true;
    log(L"ゲーム内UI: APIと配色の準備が完了しました。");
}
Key stage(Key& owner,Key& root,const wchar_t* name,int order,bool tint){owner=keep(api.create(kitClass));auto f=keep(call(kitClass,"Stage",obj(owner),{nullptr,api.text(name),&order,&tint}));root=keep(api.field<Obj>(kitClass,"root",obj(owner)));call(objectClass,"DontDestroyOnLoad",nullptr,{obj(root)});return f;}
void buildHud(){
    log(L"ゲーム内UI: HUDを作成します。");
    // Leave the bottom-right money/coin HUD unobstructed, with the same right margin.
    auto f=stage(hudKit,hudRoot,L"MoonWallet.Hud",120,false);float x=1634,y=874,w=236,h=42;bool primary=false;
    hudButton=keep(call(kitClass,"Button",obj(hudKit),{obj(f),api.text(L"ウォレット"),api.text(L"F8"),&x,&y,&w,&h,nullptr,&primary}));clicks[obj(hudButton)]={Action::Toggle,0};
    log(L"ゲーム内UI: HUDの作成が完了しました。");
}
void ensureEventSystem(){
    if(alive(call(eventClass,"get_current",nullptr)))return;
    ownedEventSystem=keep(api.create(goClass));call(objectClass,"set_name",obj(ownedEventSystem),{api.text(L"MoonWallet.EventSystem")});
    call(goClass,"AddComponent",obj(ownedEventSystem),{api.reflectionType(eventClass)});
    auto c=api.type("UnityEngine.InputSystem.UI","InputSystemUIInputModule");auto module=call(goClass,"AddComponent",obj(ownedEventSystem),{api.reflectionType(c)});call(c,"AssignDefaultActions",module);
}
void buildScreen(){
    log(L"ゲーム内UI: 送金画面を作成します。");
    frame=stage(kit,panelRoot,L"MoonWallet.Screen",900,true);auto root=obj(frame);
    plate(root,L"Wallet",362,138,1196,804,true);text(root,L"CREW / PERSONAL FUNDS",410,176,780,26,15,muted);
    text(root,L"ウォレット",408,210,750,60,46,cream,true);closeButton=button(root,L"閉じる",L"Esc",1332,198,172,48,Action::Close);
    plate(root,L"Balance",408,288,1104,112);float x=432,y=316,size=52;call(kitClass,"Coin",obj(kit),{root,&x,&y,&size});
    text(root,L"自分の残高",504,310,700,23,16,muted);balanceText=text(root,L"",500,338,956,52,38,cream,true);call(kitClass,"GoldText",nullptr,{obj(balanceText)});
    plate(root,L"Recipients",408,420,342,402);rosterTitle=text(root,L"送金先",434,444,286,32,22,cream,true);
    for(size_t i=0;i<walletui::Model::Rows;++i)rowButtons[i]=button(root,L"",L"",432,490+float(i)*51,294,43,Action::Select,i);
    prevButton=button(root,L"前へ",L"",432,758,82,38,Action::Prev);pageText=text(root,L"",525,766,100,24,16,muted);nextButton=button(root,L"次へ",L"",640,758,86,38,Action::Next);
    formRoot=keep(rect(root,L"TransferForm",774,420,738,402));auto form=obj(formRoot);plate(form,L"Transfer",0,0,738,402);
    recipientTitle=text(form,L"相手を選択してください",28,24,676,34,24,cream,true);recipientId=text(form,L"左の一覧から送金先を選べます",28,62,676,26,15,muted);text(form,L"送金額",28,104,680,25,17,muted);
    x=28;y=138;float w=682,h=64;auto background=call(kitClass,"Socket",obj(kit),{form,&x,&y,&w,&h});
    auto fieldRect=call(api.type("UnityEngine.UI","Graphic"),"get_rectTransform",background);amountField=keep(add(fieldRect,inputClass));auto viewport=rect(fieldRect,L"Viewport",18,6,646,52);add(viewport,api.type("UnityEngine.UI","RectMask2D"));
    auto entry=text(viewport,L"",0,0,646,52,30,cream),placeholder=text(viewport,L"0.00",0,0,646,52,30,muted);bool on=true;
    call(api.type("UnityEngine.UI","Graphic"),"set_raycastTarget",background,{&on});call(inputClass,"set_textViewport",obj(amountField),{viewport});call(inputClass,"set_textComponent",obj(amountField),{obj(entry)});
    call(inputClass,"set_placeholder",obj(amountField),{obj(placeholder)});call(inputClass,"set_targetGraphic",obj(amountField),{background});
    int content=3,limit=16;auto selection=cyan;selection.a=0.3f;
    call(inputClass,"set_contentType",obj(amountField),{&content});call(inputClass,"set_characterLimit",obj(amountField),{&limit});call(inputClass,"set_customCaretColor",obj(amountField),{&on});call(inputClass,"set_caretColor",obj(amountField),{&cyan});call(inputClass,"set_selectionColor",obj(amountField),{&selection});
    button(form,L"100",L"",28,218,154,40,Action::Quick,10000);button(form,L"500",L"",204,218,154,40,Action::Quick,50000);button(form,L"1,000",L"",380,218,154,40,Action::Quick,100000);button(form,L"全額",L"",556,218,154,40,Action::All);
    remainingText=text(form,L"",28,274,682,26,17,muted);reviewButton=button(form,L"内容を確認",L"Enter",430,326,280,50,Action::Review,0,true);hintText=text(form,L"",28,334,386,42,15,muted);
    reviewRoot=keep(rect(root,L"ReviewTransfer",774,420,738,402));auto review=obj(reviewRoot);plate(review,L"Review",0,0,738,402);
    text(review,L"この内容で送金しますか？",28,26,682,36,25,cream,true);reviewName=text(review,L"",28,91,682,40,27,cream,true);reviewAmount=text(review,L"",28,151,682,60,44,orange,true);reviewRemaining=text(review,L"",28,236,682,45,18,muted);
    backButton=button(review,L"戻って修正",L"Esc",28,326,246,50,Action::Back);confirmButton=button(review,L"送金を確定",L"",430,326,280,50,Action::Confirm,0,true);
    resultRoot=keep(rect(root,L"TransferResult",774,420,738,402));auto result=obj(resultRoot);plate(result,L"Result",0,0,738,402);
    resultTitle=text(result,L"送金が完了しました",28,40,682,48,32,green,true);resultBody=text(result,L"",28,112,682,140,22,cream);
    againButton=button(result,L"続けて送金",L"",28,326,302,50,Action::Again,0,true);button(result,L"ゲームに戻る",L"Esc",430,326,280,50,Action::Close);
    statusText=text(root,L"",410,846,1098,42,17,muted);text(root,L"F8  ウォレットの開閉     /     Esc  戻る・閉じる",410,894,1098,25,14,muted);
    showRoot(panelRoot,false);built=true;
    log(L"ゲーム内UI: 送金画面の作成が完了しました。");
}
void inputText(const std::wstring& value){model.input=value;call(inputClass,"SetTextWithoutNotify",obj(amountField),{api.text(value)});}
void close(){
    if(!open)return;
    open=false;inputHeld=false;bool clear=true;call(inputClass,"DeactivateInputField",obj(amountField),{&clear});showRoot(panelRoot,false);
    showRoot(hudRoot,active&&!fatal);
    originalTypingSet(priorTyping,nullptr);originalVisibleSet(priorVisible,nullptr);originalLockSet(priorLock,nullptr);
    auto es=call(eventClass,"get_current",nullptr);auto previous=obj(priorSelection);if(es)call(eventClass,"SetSelectedGameObject",es,{alive(previous)?previous:nullptr});
    if(priorSelection.handle){api.release(priorSelection.handle);priorSelection={};}
    if(ownedEventSystem.handle){call(objectClass,"Destroy",nullptr,{obj(ownedEventSystem)});auto old=ownedEventSystem.handle;api.release(old);roots.erase(std::remove(roots.begin(),roots.end(),old),roots.end());ownedEventSystem={};}
}
void openScreen(){
    if(open||!active||fatal)return;
    auto session=fn<Obj(*)(void*)>(game::SessionInstance)(nullptr);
    auto local=session?api.field<Obj>(api.type("MoonCleaners","MoonCrewSession"),"local",session):nullptr;
    auto pause=local?api.field<Obj>(api.type("MoonCleaners","MoonWalker"),"<PauseMenu>k__BackingField",local):nullptr;
    bool menu=pause&&api.value<bool>(call(api.type("MoonCleaners","MoonPauseMenu"),"get_IsOpen",pause));
    if(menu||originalTypingGet(nullptr)){notice=L"現在のメニューや入力を閉じてから開いてください。";resultUntil=GetTickCount64()+4000;return;}
    if(!built)buildScreen();
    auto cursor=api.type("UnityEngine","Cursor");priorVisible=api.value<bool>(call(cursor,"get_visible",nullptr));priorLock=api.value<int>(call(cursor,"get_lockState",nullptr));priorTyping=originalTypingGet(nullptr);
    ensureEventSystem();auto es=call(eventClass,"get_current",nullptr);auto selected=es?call(eventClass,"get_currentSelectedGameObject",es):nullptr;if(selected)priorSelection={api.retain(selected)};
    open=true;inputHeld=true;originalTypingSet(true,nullptr);originalLockSet(0,nullptr);originalVisibleSet(true,nullptr);showRoot(panelRoot,true);showRoot(hudRoot,false);notice.clear();nextRefresh=0;
    // Populate and enable the current group before selecting any of its controls.
    refresh();
    if(model.step==walletui::Step::Draft&&model.selected)call(inputClass,"ActivateInputField",obj(amountField));else if(model.step==walletui::Step::Confirm)focus(backButton);
    else if(model.step==walletui::Step::Result)focus(againButton);
    else if(model.step==walletui::Step::Draft&&!model.recipients.empty())focus(rowButtons[0]);
    else focus(closeButton);
}
const wchar_t* hint(walletui::Validation value){switch(value){
case walletui::Validation::NoRecipient:return L"送金先を選択してください";
case walletui::Validation::RecipientLeft:return L"相手が退出しました";
case walletui::Validation::EmptyAmount:return L"金額を入力してください";
case walletui::Validation::BadAmount:return L"0.01以上・小数2桁まで入力できます";
case walletui::Validation::Insufficient:return L"残高が不足しています";
case walletui::Validation::Syncing:return L"残高の同期を待っています";
default:return L"内容を確認して送金できます";
}}
void refresh(){
    std::vector<walletui::Recipient> peers;for(auto& p:roster)if(p.id!=self())peers.push_back({p.id,p.name.empty()?L"プレイヤー "+std::to_wstring(p.id%10000):safeName(p.name)});
    auto previous=model.review.target;model.roster(std::move(peers));model.available=walletui::spendable(ledger.balance(self()).text());
    if(previous&&!model.contains(previous)&&model.step==walletui::Step::Draft)notice=L"送金先が退出しました。別の相手を選択してください。";
    if(!built||!open)return;
    call(kitClass,"Fit",obj(kit));label(balanceText,money(ledger.balance(self())));label(rosterTitle,L"送金先  ·  "+std::to_wstring(model.recipients.size())+L"人");
    for(size_t row=0;row<walletui::Model::Rows;++row){size_t index=model.page*walletui::Model::Rows+row;bool exists=index<model.recipients.size();show(rowButtons[row],exists);
        if(exists){auto& peer=model.recipients[index];setButton(rowButtons[row],peer.name,model.step==walletui::Step::Draft);clicks[obj(rowButtons[row])]={Action::Select,peer.id};bool picked=peer.id==model.selected;call(kitClass,"MarkButton",nullptr,{obj(rowButtons[row]),&picked});}
    }
    auto pages=std::max(size_t(1),(model.recipients.size()+walletui::Model::Rows-1)/walletui::Model::Rows);
    label(pageText,std::to_wstring(model.page+1)+L" / "+std::to_wstring(pages));setButton(prevButton,L"前へ",model.step==walletui::Step::Draft&&model.page>0);setButton(nextButton,L"次へ",model.step==walletui::Step::Draft&&model.page+1<pages);
    bool draft=model.step==walletui::Step::Draft,review=model.step==walletui::Step::Confirm||model.step==walletui::Step::Sending;
    show(formRoot,draft);show(reviewRoot,review);show(resultRoot,model.step==walletui::Step::Result);
    if(draft){auto peer=model.recipient();label(recipientTitle,peer?peer->name:(model.recipients.empty()?L"参加者を待っています":L"相手を選択してください"));
        label(recipientId,peer?L"Steam ID  "+std::to_wstring(peer->id):L"左の一覧から送金先を選べます");auto valid=model.validation(synchronized());label(hintText,hint(valid));
        tint(hintText,valid==walletui::Validation::Insufficient||valid==walletui::Validation::BadAmount?red:muted);
        Dec after{};bool affordable=model.cents()&&Dec::sub(ledger.balance(self()),Dec::cents(model.cents()),after)&&!(after<Dec{});
        label(remainingText,affordable?L"送金後の残高  "+money(after):L"金額を選ぶか、直接入力してください");setButton(reviewButton,L"内容を確認",valid==walletui::Validation::Ready&&!pending.request);
    }else if(review){label(reviewName,model.review.name+L" さんへ");label(reviewAmount,money(Dec::cents(model.review.cents)));Dec after{};
        bool valid=model.review.cents<=model.available&&Dec::sub(ledger.balance(self()),Dec::cents(model.review.cents),after);
        label(reviewRemaining,valid?L"送金後の残高  "+money(after):L"残高が変わりました。戻って金額を修正してください。");bool sending=model.step==walletui::Step::Sending;
        setButton(confirmButton,sending?L"送金しています…":L"送金を確定",!sending&&valid&&synchronized()&&!pending.request);setButton(backButton,L"戻って修正",!sending);
    }else{label(resultTitle,resultOk?L"送金が完了しました":L"送金できませんでした");tint(resultTitle,resultOk?green:red);label(resultBody,resultText+(resultOk?L"\n現在の残高  "+money(ledger.balance(self())):L""));}
    std::wstring state=notice;if(state.empty()){
        if(fatal)state=status;
        else if(!synchronized())state=authority()?L"参加者のウォレット起動を待っています":L"ホストと残高を同期しています";
        else if(model.step==walletui::Step::Sending)state=L"結果を確認しています。この画面を閉じても処理は続きます。";
        else state=L"個人ウォレット · 他のプレイヤーの残高は共有されません";
    }label(statusText,state);
    if(focusResult&&model.step==walletui::Step::Result){focusResult=false;focus(againButton);}
}
void dispatch(Click click){
    if(click.action==Action::Toggle){if(open)close();else openScreen();return;}if(!open)return;
    model.available=walletui::spendable(ledger.balance(self()).text());
    if(click.action==Action::Review)model.input=api.string(call(inputClass,"get_text",obj(amountField)));
    switch(click.action){
    case Action::Close:close();return;
    case Action::Select:if(model.step==walletui::Step::Draft){model.selected=click.value;notice.clear();}break;
    case Action::Prev:if(model.page)--model.page;break;
    case Action::Next:if((model.page+1)*walletui::Model::Rows<model.recipients.size())++model.page;break;
    case Action::Quick:if(model.step==walletui::Step::Draft)inputText(walletui::amount(click.value));break;
    case Action::All:if(model.step==walletui::Step::Draft)inputText(walletui::amount(model.available));break;
    case Action::Review:if(model.confirm(synchronized())){notice.clear();bool clear=true;call(inputClass,"DeactivateInputField",obj(amountField),{&clear});}break;
    case Action::Back:model.back();break;
    case Action::Confirm:refresh();if(!pending.request&&model.beginSend(synchronized())){uiCommand={win::timeId(),model.review.target,0,int64_t(model.review.cents),0};notice.clear();}break;
    case Action::Again:model.again();notice.clear();inputText(L"");break;
    default:break;
    }nextRefresh=0;refresh();
    if(click.action==Action::Review&&model.step==walletui::Step::Confirm)focus(backButton);
    if(model.step==walletui::Step::Draft&&(click.action==Action::Select||click.action==Action::Quick||click.action==Action::All||click.action==Action::Back||click.action==Action::Again))call(inputClass,"ActivateInputField",obj(amountField));
}
void problem(const std::exception& e){
    log(L"ゲーム内UIエラー: "+win::utf16(e.what()));
    if(open){open=false;inputHeld=false;originalTypingSet(priorTyping,nullptr);originalVisibleSet(priorVisible,nullptr);originalLockSet(priorLock,nullptr);}
    try{if(panelRoot.handle)showRoot(panelRoot,false);if(hudRoot.handle)showRoot(hudRoot,false);}catch(...){}unavailable=true;
}
void onPress(Obj button,void* method){
    auto found=clicks.find(button);if(found==clicks.end()){originalPress(button,method);return;}
    try{bool enabled=api.value<bool>(call(buttonClass,"IsInteractable",button))&&api.value<bool>(call(buttonClass,"IsActive",button));
        auto action=found->second;originalPress(button,method);if(enabled)dispatch(action);
    }catch(const std::exception& e){problem(e);}
}
bool typing(void* m){return inputHeld.load()||originalTypingGet(m);}
void setTyping(bool value,void* m){originalTypingSet(value||inputHeld.load(),m);}
bool blocks(Obj pause,void* m){return inputHeld.load()||originalBlocks(pause,m);}
void setLock(int value,void* m){originalLockSet(inputHeld.load()?0:value,m);}
void setVisible(bool value,void* m){originalVisibleSet(inputHeld.load()?true:value,m);}
void transferFinished(Result result){
    if(model.step!=walletui::Step::Sending)return;model.finish();resultOk=result==Result::Ok;focusResult=true;
    resultText=resultOk?model.review.name+L" さんへ $"+Dec::cents(model.review.cents).text()+L" を送りました。":wallet::message(result);
    notice.clear();resultUntil=GetTickCount64()+5000;nextRefresh=0;
}
void transferAborted(const wchar_t* message){transferFinished(Result::NotReady);resultText=message;}
void tick(){
    if(unavailable)return;
    try{if(!active||fatal){if(open)close();if(hudRoot.handle)showRoot(hudRoot,false);return;}
        if(!initialized)init();
        if(previousWorld!=currentWorld||previousEpoch!=epoch){if(open)close();previousWorld=currentWorld;previousEpoch=epoch;model={};notice.clear();if(built)inputText(L"");}
        if(!hudRoot.handle)buildHud();showRoot(hudRoot,!open);call(kitClass,"Fit",obj(hudKit));
        if(pressed("get_f8Key")){if(open)close();else openScreen();}
        if(open&&pressed("get_escapeKey")){if(model.step==walletui::Step::Confirm)dispatch({Action::Back,0});else close();nextRefresh=0;}
        if(open&&model.step==walletui::Step::Draft){auto value=api.string(call(inputClass,"get_text",obj(amountField)));if(value!=model.input){model.input=value;notice.clear();nextRefresh=0;}
            // Enter only advances to review. Actual transfers require the explicit Confirm button.
            if(api.value<bool>(call(inputClass,"get_isFocused",obj(amountField)))&&(pressed("get_enterKey")||pressed("get_numpadEnterKey")))dispatch({Action::Review,0});}
        auto now=GetTickCount64();if(now>=nextRefresh){refresh();nextRefresh=now+150;auto title=resultUntil>now&&model.step==walletui::Step::Result?(resultOk?L"送金完了":L"結果を見る"):L"ウォレット";setButton(hudButton,title,true);}
    }catch(const std::exception& e){problem(e);}
}
}
