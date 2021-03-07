//Install express server
const express = require('express');
const path = require('path');
const app = express();
const http = require('http').createServer(app);
const PORT = 8000;

// Serve only the static files form the dist directory
app.use(express.static(__dirname + '/'));
app.use('/www', express.static(__dirname + '/www/'));
app.use('/snikta', express.static(__dirname + '/snikta/'));

http.listen(8000, function(){
    console.log('listening on *:' + PORT);
})