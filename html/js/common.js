var cookieHostName = getHostnameCookie()
var hostname = (cookieHostName.length > 0) ? `${cookieHostName}.local` : "lienka.local";

function setCookie(name,value) {
    let days = 30;
    let expirationDate = new Date(Date.now() + days*24*60*60*1000);
    document.cookie = `${name}=${value}; expires=${expirationDate.toUTCString()};`;
}

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


function sendSteps(inputSteps) {
    parser = new Parser(inputSteps);
    validCode = parser.SyntaxAnalyse();
    if ( validCode === true) {
        $.ajax({
            url: `http://${hostname}/sendsteps`,
            type: "post",
            data: inputSteps,
            success: function(odp) {
                console.log("success");
            },
            cache: false,
            error: function () {
                alert = $("<div class='alert alert-danger' role='alert'></div>").text("Unexpected error").hide();
                $("#alertDiv").append(alert);
                alert.toggle("slow").delay(1500).toggle(1000, function () {$(this).remove(); });
            }
        });
    }
    else {
        console.log("Something went wrong");
        alert = $("<div class='alert alert-danger' role='alert'></div>").text(validCode).hide();
        $("#alertDiv").append(alert);
        alert.toggle("slow").delay(1500).toggle(1000, function () {$(this).remove(); });
    }
}
