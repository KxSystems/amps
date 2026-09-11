.kamps:use`kx.amps

h:.kamps.create[`exampleSubscriber;enlist `$("tcp://127.0.0.1:9027/amps/json");::]

upd:{[x] msg::x; 0N!"c"$x`data }

.kamps.subscribe[h;`;`messages;`upd;::]
