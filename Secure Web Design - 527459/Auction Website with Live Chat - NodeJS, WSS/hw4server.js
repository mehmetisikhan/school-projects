var express = require('express');
var session = require('express-session'); 
var cookieParser = require('cookie-parser');
var app = express();
var fs = require('fs');
var bodyParser = require('body-parser');
var urlencodedParser = bodyParser.urlencoded({ extended: false });
var WebSocket = require('ws');



app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());
app.use(cookieParser());
app.use(session({secret: "super secret message, its a secret!",resave: false, saveUninitialized:true}));

//user = {name:'a', family:'a', phoneNumber:'a', email:'a', username:'a', password:'a' };
//user2 = {name:'b', family:'b', phoneNumber:'b', email:'b', username:'b', password:'b' };
var users=[];
var messages=[];
var activeUsers = [];
var bids = [];
var activeAuction = {statu:"deactive", item:null, winner:null, winnerBid:null, activeUsersIn:activeUsers, bids:bids};
//bids.push({user:"admin", bid:100});
//bids.push({user:"test123", bid:50});
    
//activeAuction = {item:"test item", winner:"", winnerBid:"", activeUsersIn:activeUsers, bids:bids};
//users.push(user);
//users.push(user2);

function auth(req, res) {
    let a = fs.readFileSync('userlist.csv', 'utf8');
    users = cvsToObj(a);
    var tempUser = users.find(searchElement => searchElement.username == req.body.username);
    if(tempUser != null || tempUser !=undefined){
        //console.log("temp user password -> "+tempUser.password+" re user password -> "+req.body.password);
        if(tempUser.password == req.body.password){
            req.session.validUser=req.body.username;
            req.session.isAdmin = tempUser.isadmin;
            return true;
        }
    }
    return false;
}


app.get('/getmessages', function(req,res){
    if(req.session.validUser){
        res.send(messages);
    }else {
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","Connection Error !");
            res.send(data);
        });
    }
});
/*
app.post('/sendmessage', function(req,res){
    if(req.session.validUser){
        const now = new Date();
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const hour = hours+":"+minutes;
        if(req.body.message != ""){
            messages.push({user:req.session.validUser, message:req.body.message, date:hour});
            fs.readFile(__dirname + "/" + "profile.html", "utf8", function(err, data){
                if(err) throw err;
                data=data.replace("##UserName##",req.session.validUser);
                res.send(data);
            });
        }
    }else {
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","Connection Error !");
            res.send(data);
        });
    }
    
    //res.sendFile(__dirname + "/" + "index.html" );
});
*/

app.get('/', function(req,res){
    if(req.session.validUser){ 
        getProfilePage(req,res);
    }else {
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","");
            res.send(data);
        });
    }
    
	//res.sendFile(__dirname + "/" + "index.html" );
});
 
 
app.post('/submitregister', function (req, res) {
    let a = fs.readFileSync('userlist.csv', 'utf8');
    users = cvsToObj(a);
    var tempUserRegister = users.find(searchElement => searchElement.username == req.body.username);
    if(tempUserRegister != null || tempUserRegister != undefined){
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","Registration Fail ! This username is being used, please enter another username.");
            res.send(data);
        });
    }else{
        user = {name:req.body.name, family:req.body.family, phoneNumber:req.body.phoneNumber, email:req.body.email, username:req.body.username, password:req.body.password };
        //res.end(JSON.stringify(response)); 
        //users.push(user);
        fs.appendFile('userlist.csv', user.name+';'+user.family+';'+user.phoneNumber+';'+user.email+';'+user.username+';'+user.password+';0\r\n', function (err) {
          if (err) throw err;
          console.log('User Saved!');
        });
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","");
            res.send(data);
        });
    }
});

app.get('/exit', function (req, res) {
    req.session.destroy();
    fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
        if(err) throw err;
        data=data.replace("##MSG##","");
        res.send(data);
    });
});
 
 
app.post('/submitlogin', function (req, res) {
    if(req != null){
        if(auth(req,res)){
            getProfilePage(req,res);
        }else{
            fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
                if(err) throw err;
                data=data.replace("##MSG##","Wrong password or username !")
                res.send(data);
            });
        }
    }else{
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","")
            res.send(data);
        });
    }
});

app.get('/submitlogin', async function (req, res) {
    if(req.session.validUser != null || req.session.validUser !=undefined){
        getProfilePage(req,res);
    }else{
        fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace("##MSG##","")
            res.send(data);
        });
    }
});

app.get('/admin', async function (req, res) {
    if((req.session.validUser != null || req.session.validUser !=undefined) && req.session.isAdmin == 1){
        fs.readFile(__dirname + "/" + "admin.html", "utf8", function(err, data){
            if(err) throw err;
            data=data.replace(/##UserName##/g,req.session.validUser);
            if(req.session.isAdmin == 1)
                data=data.replace('<div class="admin-panel"></div>','<span class="navbar-text" style="padding-right: 15px;">'+
                      '<a class="nav-link active" aria-current="page" href="/"><i class="fa-solid fa-house"></i>Homepage</a>'+
                    '</span>');
            res.send(data);
        });
    }else{
        if(req.session.validUser != null || req.session.validUser !=undefined){
            getProfilePage(req,res);
        }else{
            fs.readFile(__dirname + "/" + "index.html", "utf8", function(err, data){
                if(err) throw err;
                data=data.replace("##MSG##","Please login !");
                res.send(data);
            });
        }
    }
});

var clients = [];
var server = app.listen(8080, function (){
    var host = server.address().address;
    var port = server.address().port;
    console.log("Example app listening at http://%s:%s", host, port);
}); 

const wss = new WebSocket.Server({server});
var i = 0;
wss.on('connection', function (ws,request) {
    //console.log("new connection");

    clients.push(ws);
    clients.forEach(function(client) {
        /*
        if(activeUsers != null || activeUsers != undefined){
            client.send(JSON.stringify({type:"activeUsers", users:activeUsers}));
        }
        */
        client.send(JSON.stringify({type:"activeAuction", auction: activeAuction}));
    });
    ws.on('message', function (data) {
        let message = JSON.parse(data.toString("utf-8"));
        if(message["type"] == "message"){
            const now = new Date();
            const hours = now.getHours();
            const minutes = now.getMinutes();
            const hour = hours+":"+minutes;
           
            messages.push({user:message['username'], message:message['message'], date:hour});
            clients.forEach(function(client) {
                client.send(JSON.stringify({type:"message",user:message['username'], message:message['message'], date:hour}));
            });
        }
        if(message["type"] == "newBid" && activeAuction.item != null && activeAuction.statu == "active"){
            let currentHighBid = 0;
            let usersHighBid = 0;
            bids.forEach(function (_bid){
                if(_bid.user == message["username"]){
                    if(parseInt(_bid.bid) > usersHighBid){
                        usersHighBid = parseInt(_bid.bid);
                    }
                }
                if(_bid.bid > currentHighBid){
                    currentHighBid = _bid.bid;
                }
            });
            if(parseInt(message["bid"]) > usersHighBid){
                bids.push({user:message["username"], bid:message["bid"]});
                //console.log("toppestbid : "+currentHighBid+" user bid:"+parseInt(message["bid"]));
                if(parseInt(message["bid"]) > currentHighBid){
                    activeAuction.winner = message["username"];
                    activeAuction.winnerBid = parseInt(parseInt(message["bid"]));
                    currentHighBid = parseInt(message["bid"]);
                }
                clients.forEach(function(client) {
                    client.send(JSON.stringify({type:"activeAuction", auction: activeAuction}));
                });
            }
        }

        if(message["type"] == "newAuction"){
            bids = [];
            activeAuction = {statu:"active", item:message["auctionName"], winner:"", winnerBid:"", activeUsersIn:activeUsers, bids:bids};
        }

        if(message["type"] == "stopAuction"){
            bids = [];
            activeAuction.bids = [];
            activeAuction.statu = "deactive";
        }

        if(message["type"] == "reStartAuction"){
            activeAuction.statu = "active";
        }
    });
    ws.on('error', console.error);
    //clients.push(ws);
});

function cvsToObj(csv) {
    let lines = csv.split('\n');
    let result = [];
    lines[0] = String(lines[0]).replace('\r','');
    let headers = lines[0].split(';');

    for (let i = 1; i < lines.length; i++) {
        if (!lines[i])
            continue;
        let obj = {};
        lines[i] = String(lines[i]).replace('\r','');
        let currentline = lines[i].split(';');
        for (let j = 0; j < headers.length; j++) {
            obj[headers[j]] = currentline[j];
        }
        result.push(obj);
    }
    return result;
}

function getProfilePage(req,res){
    fs.readFile(__dirname + "/" + "profile.html", "utf8", function(err, data){
        if(err) throw err;
        data=data.replace(/##UserName##/g,req.session.validUser);
        //console.log("user loggined "+req.session.validUser+", isadmin :"+req.session.isAdmin);
        if(activeUsers.indexOf(req.session.validUser) == -1) //https://stackoverflow.com/questions/1988349/array-push-if-does-not-exist
            activeUsers.push(req.session.validUser);
        if(req.session.isAdmin == 1)
            data=data.replace('<div class="admin-panel"></div>','<span class="navbar-text" style="padding-right: 15px;">'+
                  '<a class="nav-link active" aria-current="page" href="/admin"><i class="fa-solid fa-pen-to-square"></i>Admin Panel</a>'+
                '</span>');
        res.send(data);
    });
}
