"""Generate Windows x64 forwarding wrappers for the verified build's economy methods."""
import json,sys,pathlib
s=json.load(open(sys.argv[1])); t=next(t for t in s if t['name']=='MoonCleaners.MoonWorld')
names='RetireDustDepots TryBuildDepot CheckConveyor RouteConveyor TryBuildConveyorRoute CrewBuildMoney ChargeBuild DevAddMoney PaySection CreditShare TryBuildStructure TryBuyVehicle RetireHexPieces TryRaiseLandmark CrewRaiseLandmark PayPremiums SaveProgress TryPayTeleport TryPurchaseContainer PayTrick CrewTrickMoney TryPurchaseTreeNode TryPurchaseTool RestoreUpgrades TryUpgradeVehicle RefundOldVehicleTree CrewPurchase'.split()
multiple={'CreditShare','PayPremiums','RestoreUpgrades'}
spend={'TryBuildDepot','CheckConveyor','RouteConveyor','TryBuildConveyorRoute','TryBuildStructure','TryBuyVehicle','TryRaiseLandmark','CrewRaiseLandmark','TryPayTeleport','TryPurchaseContainer','TryPurchaseTreeNode','TryPurchaseTool','TryUpgradeVehicle','CrewPurchase'}
def split(text):
 out=[];start=0;depth=0
 for i,c in enumerate(text):
  if c=='<':depth+=1
  if c=='>':depth-=1
  if c==',' and depth==0:out.append(text[start:i].strip());start=i+1
 if text:out.append(text[start:].strip())
 return out
def ctype(t):
 return {'System.Boolean':'bool','System.Void':'void','System.Int32':'int32_t','System.UInt32':'uint32_t','System.Int64':'int64_t','System.UInt64':'uint64_t','System.Single':'float','System.Double':'double'}.get(t,'Obj')
lines=['// Generated native-ABI wrappers; see tools/generate_hooks.py.']
install=[]
for i,m in enumerate(m for m in t['methods'] if m['name'] in names):
 sig=m['signature'];ret=sig.split(' ')[0]; params=sig[sig.index('(')+1:sig.rindex(')')];args=[]
 for p in split(params):
  p=p.split(' = ')[0];mod=''
  if p.startswith(('ref ','out ','in ')):mod=p.split()[0];p=p[len(mod)+1:]
  typ,n=p.rsplit(' ',1);ct=ctype(typ)
  if mod:ct=('Dec' if typ=='System.Decimal' else ct)+'*'
  args.append((ct,n))
 name=f'economy_{i}';proto=', '.join(['Obj world']+[a+' '+b for a,b in args]+['void* method']);types=', '.join(['Obj']+[a for a,b in args]+['void*']);call=', '.join(['world']+[b for a,b in args]+['method'])
 player=next((n for a,n in args if n in ['player','playerId','cleaner']),None)
 if not player and any(n=='builder' for a,n in args):player='(builder?fn<Str(*)(Obj,void*)>(game::WalkerPlayer)(builder,nullptr):nullptr)'
 lines+=[f'using {name}_fn={ctype(ret)}(*)({types});',f'{name}_fn {name}_original=nullptr;',f'{ctype(ret)} {name}({proto})'+' {']
 if m['name'] in spend:
  lines+=['    if(fatal || (active && !synchronized())) {']
  reason=next((n for a,n in args if n in ['reason','message']),None)
  for ct,n in args:
   if n!=reason and ct.endswith('*'):lines+=[f'        if({n})*{n}={{}};']
  if reason:lines+=[f'        if({reason})*{reason}=newString(L"Moon Wallet: ウォレットの同期を待ってから再試行してください。");']
  lines+=['        return false;', '    }']
 lines+=[f'    Scope s(world,{player or "nullptr"},{str(m["name"] in multiple or m["name"]=="SaveProgress").lower()},{str(m["name"]=="SaveProgress").lower()});']
 lines+=[f'    {"return " if ret!="System.Void" else ""}{name}_original({call});','}']
 install.append(f'    ok=hook(0x{m["rva"]:X},reinterpret_cast<void*>({name}),reinterpret_cast<void**>(&{name}_original))&&ok; // {m["name"]}')
lines+=['bool installEconomyHooks(){bool ok=true;']+install+['return ok;}']
pathlib.Path('src/economy_hooks.inc').write_text('\n'.join(lines)+'\n')
print('Generated',len(install),'economy wrappers')
