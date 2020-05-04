let payload = "";

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
    sendSteps(payload);
});
