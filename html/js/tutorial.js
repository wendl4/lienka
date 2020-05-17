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
var lienkaTaskPosition = [[0,0],[0,0],[0,0],[3,1],[1,2],[1,0],[4,0],[0,1],[0,2]];
var canvasObjects = [];
var tasks = [
    [],
    [['finish',4,0]],
    [['house',3,1]],
    [['house',3,1],['flower-purple',2,1]],
    [['face',1,0]],
    [['pigeon',0,0],['house',4,0]],
    [['key',1,0],['house',4,0]],
    [['flower-blue',1,0],['flower-purple',3,2]],
    [['bucket',0,2],['fire',3,2]],
];
var taskHints = [
    "This is basic playground where you can simulate Lienka's movement",
    "Lienka wants to get across the finish line as fast as possible, can you help her?",
    "Now after the run, Lienka is tired and wants to go home, please help her.",
    "Night passed and Lienka wants to go out and climb to her favorite flower, please show her the way.",
    "This is our friend John. He doesn't know how to brush his teeth. Make Lienka show him by going two steps forward and two steps backward and repeat this movement at least two times",
    "Oh no! There is a pigeon right behind Lienka! Please get her home to safety",
    "Ugh, pigeon gave it up. But Lienka lost her keys on her way home. Please, move back to pick up the keys and then move back home.",
    "As we already know, Lienka loves flowers. But she can't decide, which one is better. Please navigate her to both and then she will decide",
    "here is fire on the meadow! We need to extinguish it by going there and then back to the bucket of water and repeat this movement at least two times"
]

$(document).ready(function(){
renderScreen();
    setEvents();
    lienka = new Lienka("right");
    initCanvasObjects();
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
        image = document.getElementById(obj[0]);
        canvasObjects.push([image,obj[1],obj[2]]);
        ctx.drawImage(image,obj[1]*100,obj[2]*100);
    });
    lienka.draw();
}

function drawCanvas() {
    ctx.clearRect(0, 0, c.width, c.height);
    // draw other objects depending on number of task
    canvasObjects.forEach(function(obj) {
        ctx.drawImage(obj[0],obj[1]*100,obj[2]*100);
    });
}

function prepareForSimulation(code, position = 0) {
    output = [];
    while (position < code.length) {
        command = code[position];
        if (command == "count3") {
            loopContent = prepareForSimulation(code,position+1);
            for (var i=0;i<2;i++) output = output.concat(loopContent[0]);
            position = loopContent[1];
        }
        else if (command == "count2") {
            loopContent = prepareForSimulation(code,position+1);
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

    button = prepairImage(90,"hintbutton");
 	button.id = "hint-button";
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
        parser = new Parser(boardTiles.join(""));
        validCode = parser.SyntaxAnalyse();
        if ( validCode === true) {
            instructions = prepareForSimulation(boardTiles)[0];
            runCanvasBot(instructions,0);
        }
        else {
            alert = $("<div class='alert alert-danger' role='alert'></div>").text(validCode).hide();
            $("#alertDiv").append(alert);
            alert.toggle("slow").delay(1500).toggle(1000, function () {$(this).remove(); });
        }
     });

    $("#hint-button").click(function () {
        $("#modalText").html(taskHints[taskNo]);
        $("#hintModal").modal();
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
        if (taskNo < tasks.length - 1) {
            taskNo += 1;
            initCanvasObjects();
            lienka = new Lienka("right",lienkaTaskPosition[taskNo][0],lienkaTaskPosition[taskNo][1]);        
            drawCanvas();
            lienka.draw();
        }
 	});
} 