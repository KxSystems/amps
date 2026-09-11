.kamps:use`kx.amps

\c 20 200

h:.kamps.create[`client1;                                   // client name
    enlist `$("tcp://127.0.0.1:9027/amps/binary");          // binary uri
    ::]

publish:.kamps.publish[h;`messages;::];


s:1000
t:([]date:s#.z.d;sym:s?`3;bid:s?1000f;ask:s?1000f;volume:s?1000f);

upd:{[x]
    0N!xx::x
    }
.kamps.subscribe[h;`sub1;`messages;`upd;::]

publish[-8!t];

