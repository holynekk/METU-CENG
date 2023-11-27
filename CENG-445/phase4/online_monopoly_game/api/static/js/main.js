let ws = null;
function sendenter(e, f) {
    if (e.keyCode == 13) { f();}
    return false;
}
function addMess(mess) {
    ws.socket.send(mess);
}


class Ws {
    constructor(ipport) {
        this.socket = undefined;
        this.ipport = ipport;
        this.createwebsocket();
    }
    createwebsocket() {
        if (this.socket)	// if already connected close it and reopen
            this.socket.close();
        
        var socket = new WebSocket('ws://' + this.ipport);
        socket.onopen = function() {
            // send id list for notifications
            console.log("WS connection established!");
        }
        socket.onerror = function() {
            console.log("WS error!")
        }
        socket.onclose = function() {
            console.log("WS connection closed!")
            ws.socket = undefined;
            setTimeout(() => { 
                if (! ws.socket ) {
                    ws.createwebsocket();
                } }, 5000);
        }

        socket.onmessage = function wseventhandler (event) {
            console.log(event);
            var messages = event.data;
            console.log(messages)
            if (!Array.isArray(messages)) {
                messages = [messages];
            }
            for ( var mid in messages) {
                var messlist = document.getElementById('messagelist'),
                    messitem = document.createElement('li');
                messitem.innerHTML = messages[mid];
                messlist.appendChild(messitem);
            }
        }
        this.socket = socket
    }
}
window.onload = function () {
    ws = new Ws('127.0.0.1:30002')
}

// let is_connected = false;
// let cookies = document.cookie.split(";");
// for (let i = 0; i < cookies.length; i++) {
//     if (cookies[i].split("=")[0] === "is_connected") {
//         is_connected = true;
//         break;
//     } else;
// }
// if (is_connected) {
//     console.log("COOKIE: " + document.cookie);
// } else {
//     console.log("Initial websocket connection!");
//     document.cookie = "is_connected=yes";
//     ws = new Ws('127.0.0.1:30002');
// }