var tiles = ["count2","count3","begin","end","up","down","right","left"];
var boardTiles = Array(10).fill(null);
var dir = "./img/controlboard/";
var selected = null;
var QRCode = new QRCode(document.getElementById("qrcode"), {
    text: "default",
    width: 1000,
    height: 1000,
});

$(document).ready(function(){
	renderScreen();
	setEvents();
	QRCode.clear();
});

function prepairImage (width, name, css_class, extension = ".png") {
	let image = document.createElement("IMG");
	image.src = dir + name + extension;
	image.width = width;
	if (css_class) $(image).addClass(css_class);
	return image;
}

function renderScreen() {
	//load tiles
	tiles.forEach(function (name) {
		image = prepairImage(90,name,"tile");
		image.id = name;
		$("#tiles").append(image);
	});
	
	//load board with empty places and button
	for (i = 0; i < 10; i++) {
		boardTile = prepairImage(90,"empty","empty");
		boardTile.id = i;
		$("#tile-board").append(boardTile);
	}

	$("#tile-board").append("<br>");

	button = prepairImage(90,"sendbutton");
	button.id = "send-button";
	$("#tile-board").append(button);
}

function setEvents() {
	$(".tile").click(function() {
		if (selected) selected.width = selected.width / 1.3;

		if (selected == this) selected = null;
		else {
			selected = this;
			selected.width = selected.width * 1.3;
		}
	});

	$(".tile").draggable({
		helper: "clone",
		stack: ".tile"
	});

	$(".empty").droppable({
		drop: function( event, ui ) {
			this.src = $(ui.draggable).attr('src');
			boardTiles[this.id] = $(ui.draggable).attr('id');
			$(this).removeClass("empty");
			$(this).addClass("filled");
		}
	});


	$(document).on('click', '.empty', function () {
		if (selected) {
			this.src = selected.src;
			boardTiles[this.id] = selected.id;
			$(this).removeClass("empty");
			$(this).addClass("filled")
		}
	});

	$(document).on('click', '.filled', function () {
		i = this.id;
		this.src = dir + "empty.png";
		$(this).removeClass("filled");
		$(this).addClass("empty");
		boardTiles[this.id] = null;
	});

	$("#send-button").click(function () {
		sendSteps(boardTiles.join(""));
	});

	$("#qrButton").click(function () {
		QRCode.makeCode(boardTiles.join(""));
		var a = document.createElement("a");
		$("#qrcode").append(a);
		a.href = $("#qrcode").find('img').attr("src");
		a.download = "qr.png";
		a.click();
		a.href = $("#qrcode").find('img').hide();
		a.remove();
	});
}
