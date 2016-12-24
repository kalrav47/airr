/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

//node.js deps

//npm deps

//app deps
var awsIot = require('aws-iot-device-sdk');
var net = require('net');
var fs = require('fs');
var airrData = '';
//
// Simulate the interaction of a mobile device and a remote thing via the
// AWS IoT service.  The remote thing will be a dimmable color lamp, where
// the individual RGB channels can be set to an intensity between 0 and 255.  
// One process will simulate each side, with testMode being used to distinguish 
// between the mobile app (1) and the remote thing (2).  The remote thing
// will update its state periodically using an 'update thing shadow' operation,
// and the mobile device will listen to delta events to receive the updated
// state information.
//

   //
   // Instantiate the thing shadow class.
   //
   const thingShadows = awsIot.thingShadow({
      keyPath: 'private.pem.key',
      certPath: 'certificate.pem.crt',
      caPath: 'rootCA.pem.cert',
      clientId: 'kkkkkalravtest',
      region: 'us-west-2'
   });

   //
   // Operation timeout in milliseconds
   //
   const operationTimeout = 10000;

   const thingName = 'kalravnodejstest';

   var currentTimeout = null;

   //
   // For convenience, use a stack to keep track of the current client 
   // token; in this example app, this should never reach a depth of more 
   // than a single element, but if your application uses multiple thing
   // shadows simultaneously, you'll need some data structure to correlate 
   // client tokens with their respective thing shadows.
   //
   var stack = [];

   function genericOperation(operation, state) {
      var clientToken = thingShadows[operation](thingName, state);

      if (clientToken === null) {
         //
         // The thing shadow operation can't be performed because another one
         // is pending; if no other operation is pending, reschedule it after an 
         // interval which is greater than the thing shadow operation timeout.
         //
         if (currentTimeout !== null) {
            console.log('operation in progress, scheduling retry...');
            currentTimeout = setTimeout(
               function() {
                  genericOperation(operation, state);
               },
               operationTimeout * 2);
         }
      } else {
         //
         // Save the client token so that we know when the operation completes.
         //
         stack.push(clientToken);
      }
   }


   function deviceConnect() {
      thingShadows.register(thingName, {
         },
         function(err, failedTopics) {
               console.log('Device thing registered.');
         });
   }

      deviceConnect();

   function handleStatus(thingName, stat, clientToken, stateObject) {
      var expectedClientToken = stack.pop();

      if (expectedClientToken === clientToken) {
         console.log('got \'' + stat + '\' status on: ' + thingName);
      } else {
	genericOperation('update', airrData);
         console.log('(status) client token mismtach on: ' + thingName);
      }

         console.log('updated state to thing shadow'); }

	
function generateAirrState(stateObject) {
      var rgbValues = {
         "1": stateObject.state[1],
         "2": stateObject.state[2],
         "3": stateObject.state[3],
	 "4": stateObject.state[4],
         "5": stateObject.state[5],
         "6": stateObject.state[6],
	 "7": stateObject.state[7],
	 "8": stateObject.state[8],
         "9": stateObject.state[9],
         "10": stateObject.state[10],
	 "11": stateObject.state[11],
         "12": stateObject.state[12],
         "13": stateObject.state[13],
	 "14": stateObject.state[14],
         "15": stateObject.state[15],
         "16": stateObject.state[16],
	 "17": stateObject.state[17],
         "18": stateObject.state[18],
         "19": stateObject.state[19],
         "20": stateObject.state[20],
	 "21": stateObject.state[21],
         "22": stateObject.state[22],
         "23": stateObject.state[23],
         "24": stateObject.state[24],
         "25": stateObject.state[25],
      };

      return {
         state: {
            desired: rgbValues
         }
      };
   }

   function handleDelta(thingName, stateObject) {
         console.log('unexpected delta in device mode: ' + thingName+' '+JSON.stringify(stateObject));

	for(var i = 1; i<=25; i++)
	{
		if(airrData.state.desired[i] != stateObject.state[i])
		{
			console.log(i+' has changed to : '+stateObject.state[i]);
			informServer(stateObject.state[i]);
		}
	}

	airrData = generateAirrState(stateObject);
	console.log(JSON.stringify(airrData));
	fs.writeFile("airr.json",JSON.stringify(airrData), function(err) {
            if(err) {
                return console.log(err);
                }
        });
   }

   function handleTimeout(thingName, clientToken) {
      var expectedClientToken = stack.pop();

      if (expectedClientToken === clientToken) {
         console.log('timeout on: ' + thingName);
      } else {
         console.log('(timeout) client token mismtach on: ' + thingName);
      }
	genericOperation('update', airrData);
   }

   thingShadows.on('connect', function() {
      console.log('connected to AWS IoT');
   });

   thingShadows.on('close', function() {
      console.log('close');
      thingShadows.unregister(thingName);
   });

   thingShadows.on('reconnect', function() {
      console.log('reconnect');
   });

   thingShadows.on('offline', function() {
      //
      // If any timeout is currently pending, cancel it.
      //
      if (currentTimeout !== null) {
         clearTimeout(currentTimeout);
         currentTimeout = null;
      }
      //
      // If any operation is currently underway, cancel it.
      //
      while (stack.length) {
         stack.pop();
      }
      console.log('offline');
   });

   thingShadows.on('error', function(error) {
      console.log('error', error);
   });

   thingShadows.on('message', function(topic, payload) {
      console.log('message', topic, payload.toString());
   });

   thingShadows.on('status', function(thingName, stat, clientToken, stateObject) {
      handleStatus(thingName, stat, clientToken, stateObject);
   });

   thingShadows.on('delta', function(thingName, stateObject) {
      handleDelta(thingName, stateObject);
	
   });

   thingShadows.on('timeout', function(thingName, clientToken) {
      handleTimeout(thingName, clientToken);
   });

var server = net.createServer(function(socket) {
socket.on('data', function(data) {
        console.log('Received: ' + data);

        tmp = JSON.parse(data);
        console.log(airrData[1]);
        fs.writeFile("airr.json",data, function(err) {
            if(err) {
                return console.log(err);
                }
        });

	for(var i = 1; i<=25; i++)
        {
                if(airrData.state.desired[i] != tmp.state.desired[i])
                {
                        console.log(i+' has changed to : '+tmp.state.desired[i]);
			informServer(tmp.state.desired[i]);
                }
        }
	airrData = tmp;
	genericOperation('update', airrData);
});

socket.pipe(socket);
});
fs.readFile('airr.json', 'utf8', function (err, data) {
        if (err) throw err;
	airrData = JSON.parse(data);
        console.log(airrData.state.desired[1]);
});

server.listen(1337, '127.0.0.1');

function informServer(roomStat)
{
	var client = new net.Socket();
	
	client.connect(5000, '192.168.1.105', function() {
        console.log('Connected');
        client.write(String(roomStat));
});

client.on('data', function(data) {
        console.log('Received: ' + data);
        client.destroy(); // kill client after server's response
});



client.on('close', function() {
        console.log('Connection closed');
});
	

}
