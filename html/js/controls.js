var steps = "";

function sendSteps() {
	inputSteps = steps;
	console.log(steps)

 	$.ajax({
		url: "http://lienka.local/sendsteps",
		type: "post",
		data: inputSteps,
		success: function(odp) {
			console.log("success");
		},
		cache: false,
		error: function () { console.log('Nastala chyba.'); }
	});
}

$(document).ready(function(){
	$( ".up-arrow" ).click(function() {
		steps += "up";
	});
	$( ".down-arrow" ).click(function() {
		steps += "down";
	});
	$( ".right-arrow" ).click(function() {
		steps += "right";
	});
	$( ".left-arrow" ).click(function() {
		steps += "left";
	});
	$( ".go-button" ).click(function() {
		sendSteps();
		steps = "";
	});
});
