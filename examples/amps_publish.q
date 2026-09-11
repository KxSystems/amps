.kamps:use`kx.amps

h:.kamps.create[`examplePublisher;enlist `$("tcp://127.0.0.1:9027/amps/json");::]

msg:.j.j ([message:"Hello,World!";client:1])

publish:.kamps.publish[h;`messages;::];

publish msg
