.kamps:use`kx.amps

h:.kamps.create[`replayPublisher;enlist `$("tcp://127.0.0.1:9027/amps/json");::]

publish:.kamps.publish[h;`$"messages-history";::];

i:1
do[1000;msg:.j.j ([orderId:i;symbol:`IBM;size:1000;price:190.01]);publish msg;i+:1];
