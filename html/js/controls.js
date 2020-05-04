var steps = "";

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
		sendSteps(steps);
		steps = "";
	});
});
