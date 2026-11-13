#pragma once
#include <Arduino.h>

static const char PAGE[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="nl"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Line follower</title>
<style>
*{box-sizing:border-box}
body{font-family:system-ui,sans-serif;margin:0 auto;padding:12px;max-width:560px;background:#121418;color:#e8e8e8}
h1{font-size:18px;margin:2px 0 10px;display:flex;justify-content:space-between;align-items:center}
#conn{font-size:13px;font-weight:400;color:#888}
#conn.on{color:#4caf50}
.row{display:flex;gap:10px;margin-bottom:12px}
button{border:0;border-radius:10px;padding:14px;font-size:16px;font-weight:600;color:#fff;background:#3a3f4a;touch-action:manipulation;cursor:pointer}
#go{background:#1f9d55;flex:1;font-size:22px;padding:18px}
#stop{background:#d63a3a;flex:1;font-size:22px;padding:18px}
.card{background:#1c1f25;border-radius:12px;padding:12px;margin-bottom:12px}
.card h2{font-size:14px;margin:0 0 6px;color:#9aa3b2;text-transform:uppercase;letter-spacing:.05em}
#bars{display:flex;gap:4px;height:60px;align-items:flex-end;margin:4px 0 8px}
#bars div{flex:1;background:#f5c518;border-radius:4px 4px 0 0;min-height:2px}
#err{position:relative;height:10px;background:#2a2e36;border-radius:5px;margin-bottom:8px}
#errDot{position:absolute;top:-3px;width:16px;height:16px;border-radius:8px;background:#4fc3f7;margin-left:-8px;left:50%}
.mot{display:flex;gap:8px;margin-bottom:6px}
.mot div{flex:1;font-size:13px}
.mbar{height:8px;background:#2a2e36;border-radius:4px;position:relative;overflow:hidden;margin-top:3px}
.mbar i{position:absolute;top:0;bottom:0;background:#4caf50}
#st{font-size:14px;line-height:1.6}
.warn{color:#ff8a65}
.p{display:flex;align-items:center;gap:6px;margin:7px 0}
.p label{flex:1;font-size:14px;color:#c4c9d2}
.p input[type=number]{width:96px;padding:9px;font-size:16px;border-radius:8px;border:1px solid #3a3f4a;background:#0f1114;color:#eee;text-align:right}
.p button{padding:9px 13px;font-size:16px}
.p input[type=checkbox]{width:24px;height:24px}
#toast{position:fixed;left:50%;bottom:18px;transform:translateX(-50%);background:#333a45;padding:10px 16px;border-radius:10px;font-size:14px;display:none;max-width:90%}
</style></head><body>
<h1>Line follower <span id="conn">verbinden...</span></h1>
<div class="row"><button id="go">START</button><button id="stop">STOP</button></div>

<div class="card">
<div id="bars"></div>
<div id="err"><div id="errDot"></div></div>
<div class="mot"><div>Links <b id="pl">0</b><div class="mbar"><i id="bl"></i></div></div>
<div>Rechts <b id="pr">0</b><div class="mbar"><i id="br"></i></div></div></div>
<div id="st">...</div>
</div>

<div class="card"><h2>Kalibratie</h2>
<div class="row" style="margin:0"><button id="c1" style="flex:1">Start kalibratie</button><button id="c0" style="flex:1">Stop kalibratie</button></div>
</div>

<div id="params"></div>

<div class="row"><button id="save" style="flex:2;background:#2563eb">Opslaan in robot</button><button id="def" style="flex:1">Standaard</button></div>
<div id="toast"></div>

<script>
const $=id=>document.getElementById(id);
let ws,defs={},inputs={};
for(let i=0;i<8;i++)$('bars').appendChild(document.createElement('div'));
function send(s){if(ws&&ws.readyState==1)ws.send(s);}
function toast(m){const t=$('toast');t.textContent=m;t.style.display='block';clearTimeout(t._h);t._h=setTimeout(()=>t.style.display='none',2500);}
function fmt(v){return +(+v).toPrecision(6);}
function build(p){
  const box=$('params');box.innerHTML='';inputs={};const groups={};
  p.forEach(([k,label,g,v,mn,mx,st,b])=>{
    defs[k]={mn,mx,st,b};
    if(!groups[g]){const c=document.createElement('div');c.className='card';c.innerHTML='<h2>'+g+'</h2>';box.appendChild(c);groups[g]=c;}
    const r=document.createElement('div');r.className='p';
    const l=document.createElement('label');l.textContent=label;r.appendChild(l);
    let inp;
    if(b){inp=document.createElement('input');inp.type='checkbox';inp.checked=v>=0.5;
      inp.onchange=()=>send('set '+k+' '+(inp.checked?1:0));r.appendChild(inp);}
    else{
      const m=document.createElement('button');m.textContent='−';
      inp=document.createElement('input');inp.type='number';inp.step='any';inp.inputMode='decimal';inp.value=fmt(v);
      const pl=document.createElement('button');pl.textContent='+';
      const bump=d=>{let x=parseFloat(inp.value)||0;x=Math.min(mx,Math.max(mn,x+d*st));inp.value=fmt(x);send('set '+k+' '+inp.value);};
      m.onclick=()=>bump(-1);pl.onclick=()=>bump(1);
      inp.onchange=()=>send('set '+k+' '+inp.value);
      r.appendChild(m);r.appendChild(inp);r.appendChild(pl);
    }
    inputs[k]=inp;groups[g].appendChild(r);
  });
}
function setVal(k,v){const i=inputs[k];if(!i)return;if(defs[k].b)i.checked=v>=0.5;else if(document.activeElement!==i)i.value=fmt(v);}
function bar(el,v){const w=Math.abs(v)/255*50;el.style.width=w+'%';el.style.left=(v>=0?50:50-w)+'%';el.style.background=v<0?'#ef5350':'#4caf50';}
function tele(m){
  [...$('bars').children].forEach((d,i)=>d.style.height=Math.max(2,m.s[i])+'%');
  $('errDot').style.left=(50+m.e/7*50)+'%';
  $('pl').textContent=m.l;$('pr').textContent=m.r;bar($('bl'),m.l);bar($('br'),m.r);
  let t=(m.run?'<b style="color:#4caf50">RIJDT</b>':'<b style="color:#ef5350">GESTOPT</b>')
    +' &nbsp; fout '+m.e.toFixed(2)+' &nbsp; basis '+m.b
    +(m.lost?' <span class="warn">LIJN KWIJT</span>':'')+(m.x?' <span class="warn">KRUISPUNT</span>':'')
    +'<br>'+m.fps+' frames/s &nbsp; rekentijd '+m.us+' µs';
  if(m.cal)t+='<br><span class="warn">KALIBREREN: beweeg alle sensoren over lijn en wit vlak, dan Stop kalibratie</span>';
  else if(!m.calok)t+='<br><span class="warn">Niet gekalibreerd</span>';
  if(!m.ok)t+='<br><span class="warn">Geen sensordata (bekabeling/baudrate?)</span>';
  $('st').innerHTML=t;
}
function conn(){
  ws=new WebSocket('ws://'+location.hostname+':81/');
  ws.onopen=()=>{$('conn').textContent='verbonden';$('conn').className='on';};
  ws.onclose=()=>{$('conn').textContent='verbinding weg (robot rijdt door)';$('conn').className='';setTimeout(conn,1000);};
  ws.onmessage=e=>{const m=JSON.parse(e.data);
    if(m.t=='T')tele(m);else if(m.t=='P')build(m.p);else if(m.t=='V')setVal(m.k,m.v);else if(m.t=='M')toast(m.m);};
}
$('go').onclick=()=>send('run');
$('stop').onclick=()=>send('stop');
$('c1').onclick=()=>send('cal1');
$('c0').onclick=()=>send('cal0');
$('save').onclick=()=>send('save');
$('def').onclick=()=>{if(confirm('Alle waarden terug naar standaard?'))send('def');};
document.addEventListener('keydown',e=>{if(e.code==='Space'&&document.activeElement.tagName!=='INPUT'){e.preventDefault();send('stop');}});
conn();
</script></body></html>)rawliteral";
