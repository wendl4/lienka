let cookieHostName = getHostnameCookie()
let hostname = (cookieHostName.length > 0) ? `${cookieHostName}.local` : "lienka.local";
let payload = "";

function getHostnameCookie() {
    let cookies = decodeURIComponent(document.cookie);
    let regex1 = /hostname=\w+;/;
    let regex2 = /hostname=\w+$/;
    let hostnameCookie = cookies.match(regex1);
    let hostnameValue = "";
    if (!!hostnameCookie) {
        hostnameValue = hostnameCookie[0].split("=")[1].slice(0,-1);
    }
    else {
        hostnameCookie = cookies.match(regex2);
        if (!!hostnameCookie) {
            hostnameValue = hostnameCookie[0].split("=")[1];
        }
    }
    
    return hostnameValue
}

function sendSteps() {

 	$.ajax({
		url: "http://lienka.local/sendsteps",
		type: "post",
		data: payload,
		success: function(odp) {
			steps = "";
			console.log("success");
		},
		cache: false,
		error: function () { console.log('Nastala chyba.'); }
	});
}

$("#lookButton").click(function () {
    $("#lookButton").attr('disabled', 'disabled');
    $('#cameraImage').html(`<img src='http://${hostname}/bmp?${new Date().getTime()}' />`);
    $('#loader').toggleClass("request-loader");
    $("#scan-result").html("");
    $.ajax({
		url: `http://${hostname}/check-qr`,
		type: "get",
        cache: false,
        success: function (odp) {
            $('#loader').toggleClass("request-loader");
            $("#scan-result").removeClass().addClass("badge");
            if (odp === "Error") {
                $("#scan-result").addClass("badge-danger").html("Error");
                $("#runButton").attr('disabled', 'disabled');
            }
            else {
                $("#scan-result").addClass("badge-success").html("Validated");
                payload = odp;
                $("#runButton").removeAttr('disabled');
                console.log(payload);
            }
        },
        error: function () {
            console.log("setup-sta: Something went wrong");
        },
        complete: function() {
            $("#lookButton").removeAttr('disabled');
        }
    });
});


$("#runButton").click(function() {
    sendSteps();
});
