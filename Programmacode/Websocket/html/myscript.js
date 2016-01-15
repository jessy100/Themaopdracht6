var was = "";
var temp = "";
var inplan_delay;
var request_string;

$( document ).ready(function() {
    WebSocketConnect();	
   
});

$(".wasprogramma-detail" ).click(function(event) {
	alert( "Element clicked:" + event.target.id );
	was = event.target.id;
	showDetailFrom();
});

$("#inplannen" ).click(function(event) {
	alert( "Element clicked:" + event.target.id );
	showInplannen();
});

$("#submit" ).click(function(event) {
	alert( "Element clicked:" + event.target.id );
	event.preventDefault();
	sendRequest();
});


function showDetailFrom(){
	
	$(".wasprogramma").hide();
	$(".form-detail").show();	
}

function showInplannen(){
	$(".element-inplannen").show();
}


function handleClick(element){
	console.log(element);
}

function sendRequest() {
	temp = $( "#temperatuur" ).val();
	if(document.getElementById('inplannen').checked) {
		inplan_delay = $( "#inplan_delay" ).val();
		console.log("inplannen was checked on submit");
	}else{
		inplan_delay = 0;
	}
	request_string = "START_WAS" + "-"+ was + "-" + temp +"-" + inplan_delay;
	console.log(request_string);
	ws.send(request_string);
}


function closeConnection() {
	ws.close();
}

function onOpen (evt) {
	console.log("connection open");
}

function onMessage (evt){
	append(evt.data);
	console.log("msg recvd");
}

function onError (evt){
 	console.log("websocket error" + evt.data);
	alert("Error:" + evt.data);
}

function WebSocketConnect() {
  if ("WebSocket" in window) {
     ws = new WebSocket("ws://127.0.0.1:8080");
     ws.onopen = function(evt) { onOpen(evt) };
     ws.onclose = function(evt) { onClose(evt) };
     ws.onmessage = function(evt) { onMessage(evt) };
     ws.onerror = function(evt) { onError(evt) };
  }
  else  {
     alert("WebSocket NOT supported by your Browser!");
  }
}

