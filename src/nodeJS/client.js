var net = require('net');
var client = new net.Socket();
var fs = require('fs');

function generateRandomState(data1,data2,data3) {
      var rgbValues = {
         "1": data1,
         "2": data2,
         "3": data3
      };

      return {
         state: {
            desired: rgbValues
         }
      };
   }

client.connect(1337, '127.0.0.1', function() {
	console.log('Connected');
	client.write(JSON.stringify(generateRandomState(12345678901,2,6)));
});

client.on('data', function(data) {
	console.log('Received: ' + data);

	var obj = JSON.parse(data);
	console.log('kalrav got '+ obj.name);

	client.destroy(); // kill client after server's response
});



client.on('close', function() {
	console.log('Connection closed');
});
