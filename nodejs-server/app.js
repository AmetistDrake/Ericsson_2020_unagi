// Load the TCP Library
net = require('net');

// Keep track of the chat clients
let clients = [];

let vaccine_data = []; // ID = country ID, TPC = total production capacity, RV = reserved vaccines, ASID =
let safe = [];
let warn = [];

let test1 = [
    [Math.floor(Math.random()*100000), 3], // gameID, maxtick,
    [6, 4], // y, x
    [[0, 10, 10]], // country start data: countryID, TPC, RV
    [1569741360, 1785505948, 516548029, 1302116447],
    [[[1, 0, 5], [1, 1, 1], [1, 0, 5], [3, 0, 5]],
     [[0, 0, 5], [1, 0, 4], [3, 2, 1], [3, 0, 2]],
     [[0, 1, 1], [2, 0, 5], [3, 0, 5], [5, 0, 5]],
     [[0, 0, 2], [2, 1, 1], [2, 0, 4], [5, 2, 1]],
     [[0, 0, 5], [2, 0, 5], [5, 0, 4], [5, 0, 3]],
     [[4, 0, 5], [4, 1, 1], [4, 0, 2], [4, 0, 5]]]
];

function reqData(tick) {
    let test = test1;
    let response = "";

    if (tick >= test[0][1]) { // tick >= maxtick
        return response;
    }

    if(tick === -1) {
        response += "START " + String(test[0][0]) + " " + String(test[0][1]) + " " + String(test[2].length) + "\n"; // START <game-id> <max-tick> <countries-count>
        response += "FACTORS " + String(test[3][0]) + " " + String(test[3][1]) + " " + String(test[3][2]) + " " + String(test[3][3]) + "\n"; // FACTORS <factor-1> <factor-2> <factor-3> <factor-4>
        response += "FIELDS " + String(test[1][0]) + " " + String(test[1][1]) + "\n"; // FIELDS <rows> <columns>

        for (let y = 0; y < test[1][0]; y++) { // <rows>
            for (let x = 0; x < test[1][1]; x++) { // <columns>
                response += "FD " + String(y) + " " + String(x) + " " + String(test[4][y][x][0]) + " " + String(test[4][y][x][1]) + " " + String(test[4][y][x][2]) + "\n"; // FD <y> <x> <district> <inf_rate> <popul>
            }
        }
        response += ".";
        // console.log(response);
    }
    else
    {
        response = "REQ " + String(test[0][0]) + " " + String(tick) + " " + String(test[2][0][0]) + "\n"; // REQ <game-id> <tick-id> <your-country-id>

        for (let i = 0; i < test[2].length; i++) { // <country-start-data>
            response += String(test[2][i][0]) + " " + String(test[2][i][1]) + " " + String(test[2][i][2]) + "\n"; // <country-id> <TPC> <RV>
        }

        if (tick === 0) {
            for(let y = 0; y < test[1][0]; y++) { // <rows>
                for (let x = 0; x < test[1][1]; x++) { // <columns>
                    if (!vaccine_data[y]) {
                        vaccine_data[y] = [];
                    }
                    vaccine_data[y][x] = [0, 0];
                }
            }
        }

        for(let y = 0; y < test[1][0]; y++) { // <rows>
            for (let x = 0; x < test[1][1]; x++) { // <columns>
                response += "VAC " + String(y) + " " + String(x) + " " + vaccine_data[y][x][0] + " " + vaccine_data[y][x][1] + "\n";
            }
        }

        for(let i = 0; i < safe.length; i++) {
            response += "SAFE " + String(safe[i][0]) + " " + String(safe[i][1]) + "\n";
        }

        for(let i = 0; i < warn.length; i++) {
           response += "WARN " + warn[i] + "\n";
        }

        response += ".";
        // console.log(response);
    }

    return response;
}

// Start a TCP Server
const server = net.createServer(function (socket) {
    socket.name = socket.remoteAddress + ":" + socket.remotePort; // Identify this client
    clients.push(socket); // Put this new client in the list
    process.stdout.write(("Welcome " + socket.name + "\n")); // Send a nice welcome message and announce

    // Handle incoming messages from clients.
    socket.on('data', function (data) {
        let response;
        const message = data.toString().split("\n");

        for (let i = 0; i < message.length; i++) {
            //console.log(message[i]);
            if (message[i] === ".") {
                break;
            }
        }

        if (message[0].includes("START")) {
            response = reqData(-1);
            socket.write(response);
            response = reqData(0);
            socket.write(response);
        } else if (message[0].includes("RES")) {
            let words = message[0].split(" ");
            let tick = words[2];
            tick++;
            response = reqData(tick);
            if (response !== "") {
                socket.write(response);
            } else {
                socket.end();
            }
        }
    });

    // Remove the client from the list when it leaves
    socket.on('end', function () {
        clients.splice(clients.indexOf(socket), 1);
        process.stdout.write(socket.name + " is disconnected.\n");
    });

    socket.on('error', function(e){
        console.log(e);
    });
});

server.listen(1234, () => console.log(`Listening on port 1234!`));
