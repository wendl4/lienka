$(document).ready(function() {
    refreshHostnameValues()
});

function refreshHostnameValues() {
    $("#hostname-td").html(hostname);
    $("#addressInput").val(hostname);
    getStatus();
}

function getStatus() {
    $.ajax({
		url: `http://${hostname}/status`,
		type: "get",
        cache: false,
		complete: function(odp) {
            badge = "badge-success";
            status = "";
            mode = "";
            if (odp.responseText == "STA" || odp.responseText == "AP") {
                status = "connected"
                mode = odp.responseText
                $('#staButton').prop('disabled', false);
            }
            else {
                status = "N/A"
                badge = "badge-secondary"
                $('#staButton').prop('disabled', true);
            }
            $("#statusValue").html(`<span class='badge badge-pill ${badge}'>${status} ${mode}</span>`);
        },
    });
}


function checkName(name) {
    $.ajax({
		url: `http://${name}/status`,
        type: "get",
        cache: false,
		success: function(odp) {
            alert = $("<div class='alert alert-success' role='alert'></div>").text("Hostname was found and set as active.").hide();
            hostname = name;
            
        },
        error: function(xhr, textStatus, error) {
            alert = $("<div class='alert alert-danger' role='alert'></div>").text("Hostname wasn't found.").hide();

        },
        complete: function() {
            $("#alertDiv").append(alert);
            alert.toggle("slow").delay(1500).toggle(1000, function () {$(this).remove(); });
        },
        timeout: 3000
    });
}

function changeName(name) {
    $.ajax({
        url: `http://${hostname}/change-hostname`,
        type: "post",
        data: name,
        cache: false,
        success: function(odp) {
            hostname = `${odp}.local`;
            setCookie("hostname",odp);
            refreshHostnameValues();
            alert = $("<div class='alert alert-success' role='alert'></div>").text(`Hostname successfully changed to ${odp}`).hide();
        },
        error: function() {
            alert = $("<div class='alert alert-success' role='alert'></div>").text(`Hostname couldn't be changed`).hide();
        },
        complete: function() {
            $("#alertDiv").append(alert);
            alert.toggle("slow").delay(2500).toggle(1000, function () {$(this).remove(); });
        }
    });
}

function nameValid(name) {
    let properName = /^[a-z0-9]+$/;
    return properName.test(name);
}


$("#setupStaForm").submit(function () {
    $.ajax({
		url: `http://${hostname}/setup-sta`,
		type: "post",
		data: $("#setupStaForm").serialize(),
        cache: false,
        success: function (odp) {
            $("#networkModal").modal();
        },
        error: function () {
            console.log("setup-sta: Something went wrong")
        }
    });
    $(this).trigger("reset");
    return false;
});

$("#statusButton").click(function () {
    getStatus();
});

$("#resetButton").click(function () {
    $.ajax({
        url: `http://${hostname}/reset-nvs`,
		type: "post",
        cache: false,
        success: function(odp) {
            $("#afterResetModal").modal();
        },
        error: function() {
            console.log("resetNVS: Something went wrong");
        },
    });
});

$("#changeNameButton").click(function() {
    let name = $("#newNameInput").val();
    if (nameValid(name)) {
        changeName(name);
        $("#changeNameModal").modal('hide');
    }
});

$("#checkAddressButton").click(function() {
    let name = $("#addressInput").val();
    let nameOrAddress = /^\w+\.local$|^\d+\.\d+\.\d+\.\d+$/;
    if (nameOrAddress.test(name)) {
        checkName(name)
    }
    else {
        alert = $("<div class='alert alert-warning' role='alert'></div>").text("Bad name format. Enter either name with suffix '.local' or ip address").hide();
        $("#alertDiv").append(alert);
        alert.toggle("slow").delay(2500).toggle(1000, function () {$(this).remove(); });
    }
});

$("#newNameInput").keyup(function() {
    valid = nameValid($("#newNameInput").val())
    if (valid) {
        $(this).css("background-color","#BAFFBD");
    }
    else {
        $(this).css("background-color","#FFBABA");
    }
});