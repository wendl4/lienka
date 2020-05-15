var tiles = ["count2","count3","begin","end","up","down","right","left"];
var directions = ["up","right","down","left"];
var rotation = {"up":0,"right":90,"down":180,"left": 270};
var directionMove = {"up":[0,-0.02],"right":[0.02,0],"down":[0,0.02],"left": [-0.02,0]};
var boardTiles = Array(10).fill(null);
var dir = "./img/controlboard/";
var selected = null;
var c = document.getElementById("tutorialCanvas");
var ctx = c.getContext("2d");
var taskNo = 0;
var lienka;
var lienkaTaskPosition = [[0,0],[0,0],[3,1],[1,2],[1,0],[4,0],[0,1],[0,2]];
var canvasObjects = [];
var tasks = [
    [['./img/tutorial/finish.png',4,0]],
    [['./img/tutorial/house.png',3,1]],
    [['./img/tutorial/house.png',3,1],['./img/tutorial/flower-purple.png',2,1]],
    [['./img/tutorial/face.png',1,0]],
    [['./img/tutorial/pigeon.png',0,0],['./img/tutorial/house.png',4,0]],
    [['./img/tutorial/key.png',1,0],['./img/tutorial/house.png',4,0]],
    [['./img/tutorial/flower-blue.png',1,0],['./img/tutorial/flower-purple.png',3,2]],
    [['./img/tutorial/bucket.png',0,2],['./img/tutorial/fire.png',3,2]],
];

$(document).ready(function(){
	renderScreen();
    setEvents();
    initCanvasObjects();
    lienka = new Lienka("right",0,0);
    drawCanvas();
});

class Lienka {

    constructor(direction,x=0,y=0) {
        this.x = x;
        this.y = y;    
        this.image = new Image(100, 100);
        this.image.src = './img/controls/beebot.png';
        this.image.onload = function() {
            this.draw();
        }.bind(this)
        this.direction = direction;
    }

    move(way) {
        if (way == "forward") {
            this.x += directionMove[this.direction][0];
            this.y += directionMove[this.direction][1];
        }
        else {
            this.x -= directionMove[this.direction][0];
            this.y -= directionMove[this.direction][1];          
        }
        drawCanvas();
        this.draw();
    }

    rotate(side) {
        if (side == "right") {
            this.direction = directions[(directions.indexOf(this.direction)+1)%4]
        }
        else {
            this.direction = directions[(directions.indexOf(this.direction)+3)%4]
        }
        drawCanvas();
        this.draw();
    }

    draw() {
        ctx.save();
        ctx.translate(this.x * 100 + 50,this.y * 100 +50);  
        ctx.rotate(Math.PI / 180 * rotation[this.direction]);
        ctx.drawImage(this.image, -this.image.width / 2, -this.image.height / 2, this.image.width, this.image.height);
        ctx.restore();
    }

    testNoBorder(way) {
        let x = this.x;
        let y = this.y;
        
        if (way == "up") {
            x += directionMove[this.direction][0];
            y += directionMove[this.direction][1];
        }
        else {
            x -= directionMove[this.direction][0];
            y -= directionMove[this.direction][1];          
        }
        if (x > 4 || x < 0 || y > 2 || y < 0) return false;
        return true;
    }
}

function initCanvasObjects() {
    canvasObjects = [];
    tasks[taskNo].forEach(function(obj) {
        var image = new Image(100, 100);
        canvasObjects.push([image,obj[1],obj[2]]);
        image.src = obj[0];
        image.onload = function() {
            ctx.drawImage(image,obj[1]*100,obj[2]*100);
        }
    });
}

function drawCanvas() {
    // draw grid
    ctx.clearRect(0, 0, c.width, c.height);
    // draw other objects depending on number of task
    canvasObjects.forEach(function(obj) {
        ctx.drawImage(obj[0],obj[1]*100,obj[2]*100);
    });
    //lienka.draw();
}

function makeLinearCode(code, position = 0) {
    output = [];
    while (position < code.length) {
        command = code[position];
        if (command == "count3") {
            loopContent = makeLinearCode(code,position+1);
            for (var i=0;i<2;i++) output = output.concat(loopContent[0]);
            position = loopContent[1];
        }
        else if (command == "count2") {
            loopContent = makeLinearCode(code,position+1);
            for (var i=0;i<1;i++) output = output.concat(loopContent[0]);
            position = loopContent[1];
        }
        else if (directions.includes(command)) {
            output.push(command);
        }
        else if (command == "end") {
            return [output,position];
        }
        position++;
    }
    return [output,position]
}

function runCanvasBot(instructions, position) {
    command = instructions[position]
    if ((command == "up" || command == "down") && lienka.testNoBorder(command)) {
        var i = 0;
        var way = (command == "up") ? "forward" : "backward";
        timer = setInterval(function() {
            lienka.move(way);
            i++;
            if (i==50) {
                lienka.x = Math.round(lienka.x);
                lienka.y = Math.round(lienka.y);
                clearInterval(timer);
                runCanvasBot(instructions, position+1)
            };
        },20);
    }

    if (command == "left" || command == "right") {
        var i = 0;
        timer = setInterval(function() {
            ctx.clearRect(0, 0, c.width, c.height);
            drawCanvas();
            ctx.save();
            ctx.translate(lienka.x * 100 + 50,lienka.y * 100 +50);  
            ctx.rotate(Math.PI / 180 * rotation[lienka.direction]);
            ctx.rotate(Math.PI / 180 * i);
            ctx.drawImage(lienka.image, -lienka.image.width / 2, -lienka.image.height / 2, lienka.image.width, lienka.image.height);
            ctx.restore();
            (command == "left") ? i-- : i++;
            if (Math.abs(i)==90) {
                clearInterval(timer);
                lienka.rotate(command);
                runCanvasBot(instructions, position+1)
            };
        },20);
    }
}












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
        instructions = makeLinearCode(boardTiles)[0];
        runCanvasBot(instructions,0);
    });
    
    $("#back-button").click(function () {
        if (taskNo > 0) {
            taskNo -= 1;
            initCanvasObjects();
            lienka = new Lienka("right",lienkaTaskPosition[taskNo][0],lienkaTaskPosition[taskNo][1]);
            drawCanvas();
            lienka.draw();
        }
    });
    
    $("#next-button").click(function () {
        if (taskNo < 7) {
            taskNo += 1;
            initCanvasObjects();
            lienka = new Lienka("right",lienkaTaskPosition[taskNo][0],lienkaTaskPosition[taskNo][1]);
            drawCanvas();
            lienka.draw();
        }
	});
}