.kamps:use`kx.amps

clientName:"orderSubscriber"

h:.kamps.create[`$clientName;enlist `$("tcp://127.0.0.1:9027/amps/json");::]

upd:{[x] 
    msg::x;
    order:.j.k "c"$x`data;
    -1"Process Order with OrderId = ",string order`orderId;
    .kamps.discard[h; x[`subId]; x[`bookmarkSeqNo]];
    }

.kamps.subscribe[h;`$clientName,"_sub";`$"messages-history";`upd;([bookmark:`0])]
