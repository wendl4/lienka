var steps = "";

function sendSteps(inputSteps) {
	$.ajax({
		url: "/sendsteps.cgi",
		type: "post",
		data: { steps:inputSteps },
		success: function(odp) {
			steps = "";
			console.log("success");
		},
		cache: false,
		error: function () { alert('Nastala chyba.'); }
	});
}

$(document).ready(function(){
	$( ".up-arrow" ).click(function() {
		steps += "1";
	});
	$( ".down-arrow" ).click(function() {
		steps += "2";
	});
	$( ".right-arrow" ).click(function() {
		steps += "3";
	});
	$( ".left-arrow" ).click(function() {
		steps += "4";
	});
	$( ".go-button" ).click(function() {
		sendSteps(steps);
		steps = "";
	});
});
