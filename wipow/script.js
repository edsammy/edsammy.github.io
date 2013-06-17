$('#btn').click(function () {
    var comm = "";
    $('.someclass').each(function () {
        if ($(this).prop('checked') == true) {
            comm = comm + "1";
        } else {
            comm = comm + "0";
        }
    });
    $('#checkStates').text(comm);
});