var was = "";
var temp = "";
var inplan_delay;
var request_string;
var obj = "";

$( document ).ready(function() {
    WebSocketConnect();	
    $('#tempMeterDiv').hide();
    $('#waterMeterDiv').hide();
});

$(".wasprogramma-detail" ).click(function(event) {
	//alert( "Element clicked:" + event.target.id );
	was = event.target.id;
	showDetailFrom();
});

$("#inplannen" ).click(function(event) {
	//alert( "Element clicked:" + event.target.id );
	showInplannen();
});

$("#submit" ).click(function(event) {
	//alert( "Element clicked:" + event.target.id );
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
	request_string = "START_WAS" + " "+ was + " " + temp +" " + inplan_delay;
	console.log(request_string);
	$(".form-detail").hide();	
	$('#tempMeterDiv').show();
    $('#waterMeterDiv').show();
	ws.send(request_string);
	

} 


function closeConnection() {
	//ws.close();
}

function onOpen (evt) {
	console.log("connection open");
}

function onMessage (evt){
	//append(evt.data);
	var json = evt.data,
    obj = JSON.parse(json);
 
	console.log(obj[0].temp);
	$tempMeter.changeValue(obj[0].temp);
	$waterMeter.changeValue(obj[0].water);
	console.log(evt.data);
}

function onError (evt){
 	console.log("websocket error" + evt.data);
	alert("Error:" + evt.data);
}

function onClose(event){
	ws.close();
	
}
function WebSocketConnect() {
  if ("WebSocket" in window) {
     ws = new WebSocket("ws://192.168.0.119:8080");
     ws.onopen = function(evt) { onOpen(evt) };
     ws.onclose = function(evt) { onClose(evt) };
     ws.onmessage = function(evt) { onMessage(evt) };
     ws.onerror = function(evt) { onError(evt) };
  }
  else  {
     alert("WebSocket NOT supported by your Browser!");
  }
}

