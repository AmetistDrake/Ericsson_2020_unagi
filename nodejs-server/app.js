// Load the TCP Library
net = require('net');
const GameDisplay = require('./GameDisplay');

// Keep track of the chat clients

const reader = new GameDisplay.Reader();

const PORT = "1234";
let clients = [];

let gameid = 1;
let maxtick = 44;
let coutries_count = 0;
let factor1 = 1569741360;
let factor2 = 1785505948;
let factor3 = 516548029;
let factor4 = 1302116447;

function reqData(tick) {
    let response = "REQ " + String(gameid) + " " + String(tick) + " " + String(coutries_count) + "\n.";
    console.log(response);
    factor2 = factor2 * 48271 % 0x7fffffff;
    factor3 = factor3 * 48271 % 0x7fffffff;
    factor4 = factor4 * 48271 % 0x7fffffff;
    console.log("<FACTORS AFTER GEN: " + factor1 + " " + factor2 + " " + factor3 + " " + factor4 + ">");
    return response;
}

function reqStartData() {
    let response = "REQ " + String(gameid) + " " + String(0) + " " + String(coutries_count) + "\n.";
    console.log(response);
    console.log("<FACTORS AFTER GEN: " + factor1 + " " + factor2 + " " + factor3 + " " + factor4 + ">");
    return response;
}

function startData() {
    let response = "";
        response += "START " + String(gameid) + " " + String(maxtick) + " " + String(coutries_count) + "\n";
        response += "FACTORS " + String(factor1) + " " + String(factor2) + " " + String(factor3) + " " + String(factor4) + "\n";
        response += "FIELDS 6 4\n";
        response += "FD " + String(0) + " " + String(0) + " " + String(1) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(0) + " " + String(1) + " " + String(1) + " " + String(1) + " " + String(1) + "\n";
        response += "FD " + String(0) + " " + String(2) + " " + String(1) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(0) + " " + String(3) + " " + String(3) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(1) + " " + String(0) + " " + String(0) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(1) + " " + String(1) + " " + String(1) + " " + String(0) + " " + String(4) + "\n";
        response += "FD " + String(1) + " " + String(2) + " " + String(3) + " " + String(2) + " " + String(1) + "\n";
        response += "FD " + String(1) + " " + String(3) + " " + String(3) + " " + String(0) + " " + String(2) + "\n";
        response += "FD " + String(2) + " " + String(0) + " " + String(0) + " " + String(1) + " " + String(1) + "\n";
        response += "FD " + String(2) + " " + String(1) + " " + String(2) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(2) + " " + String(2) + " " + String(3) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(2) + " " + String(3) + " " + String(5) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(3) + " " + String(0) + " " + String(0) + " " + String(0) + " " + String(2) + "\n";
        response += "FD " + String(3) + " " + String(1) + " " + String(2) + " " + String(1) + " " + String(1) + "\n";
        response += "FD " + String(3) + " " + String(2) + " " + String(2) + " " + String(0) + " " + String(4) + "\n";
        response += "FD " + String(3) + " " + String(3) + " " + String(5) + " " + String(2) + " " + String(1) + "\n";
        response += "FD " + String(4) + " " + String(0) + " " + String(0) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(4) + " " + String(1) + " " + String(2) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(4) + " " + String(2) + " " + String(5) + " " + String(0) + " " + String(4) + "\n";
        response += "FD " + String(4) + " " + String(3) + " " + String(5) + " " + String(0) + " " + String(3) + "\n";
        response += "FD " + String(5) + " " + String(0) + " " + String(4) + " " + String(0) + " " + String(5) + "\n";
        response += "FD " + String(5) + " " + String(1) + " " + String(4) + " " + String(1) + " " + String(1) + "\n";
        response += "FD " + String(5) + " " + String(2) + " " + String(4) + " " + String(0) + " " + String(2) + "\n";
        response += "FD " + String(5) + " " + String(3) + " " + String(4) + " " + String(0) + " " + String(5) + "\n";
        response += ".";
    console.log(response);
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
            console.log(message[i]);
            if (message[i] === ".") {
                break;
            }
        }

        if (message[0].includes("START")) {
            response = startData();

            reader.loadData(response);

            socket.write(response);
            response = reqStartData();
            socket.write(response);
        } else if (message[0].includes("RES")) {
            let words = message[0].split(" ");
            let tick = words[2];
            tick++;
            if (tick < 44) {
                response = reqData(tick);

                let infections = [];
                for (let j = 1; j < message.length; j++) {
                    words = message[j].split(" ");
                    infections.push(words);
                }

                for (let y = 0; y < reader.dimension[0]; y++) {
                    for (let x = 0; x < reader.dimension[1]; x++) {
                        if (!reader.gameHistory[tick]) {
                            reader.gameHistory[tick] = [];
                        }
                        if (!reader.gameHistory[tick][y]) {
                            reader.gameHistory[tick][y] = [];
                        }

                        reader.gameHistory[tick][y][x] = new GameDisplay.Area();
                        reader.gameHistory[tick][y][x].infectionRate = infections[y][x];
                    }
                }

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

server.listen(PORT, () => console.log(`Listening on port ${PORT}!`));