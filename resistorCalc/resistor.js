$(window).load(function(){
    $("#R").keypress(function (e) {
        var chr = String.fromCharCode(e.which);
        //Only allow numbers, SI prefixes & spaces to be entered. All other values are not entered in text box
        return "0123456789TGMk ".indexOf(chr)>=0;
    });

    $('#R').keydown(function (event) {
        //Key code 13 = enter key
        if (event.keyCode == 13) {
            $('#calc').trigger('click');
        }
    });

    $("#calc").click(function () {
        var SI_pos = {
                "T": 1000000000000,
                "G": 1000000000,
                "M": 1000000,
                "k": 1000
        };
        var SI_neg = {
                "c": 0.01,
                "m": 0.001,
                "u": 0.000001,
                "n": 0.000000001,
                "p": 0.000000000001
        };
        var parallelRes = 0;
        var seriesRes = 0;
        var resistors = $("#R").val().split(" ");
        for (var i = 0; i < resistors.length; i++) {
            var len = resistors[i].length;
            var lastChar = resistors[i].substring(len, len - 1);
            if (SI_pos[lastChar] != undefined) {
                resistors[i] = resistors[i].replace(lastChar, "");
                resistors[i] = resistors[i] * SI_pos[lastChar];
                //alert(i + "=" + resistors[i]);
            }

            parallelRes += 1 / resistors[i];
            seriesRes += parseInt(resistors[i]);
        }
        parallelRes = 1 / parallelRes;
        //alert(parallelRes);
        var Rout = [parallelRes, seriesRes];

        for (var i = 0; i < Rout.length; i++) {
            if (Rout[i] >= SI_pos["M"]) {
                Rout[i] /= SI_pos["M"];
                Rout[i] = +Rout[i].toFixed(2);
                Rout[i] += "M";
            } else if (Rout[i] >= SI_pos["k"]) {
                Rout[i] /= SI_pos["k"];
                Rout[i] = +Rout[i].toFixed(2);
                Rout[i] += "k";
            }
            else {
                Rout[i] = +Rout[i].toFixed(2);
            }
        }

        $("#parallel").html("parallel: " + Rout[0] + "&#8486;");
        $("#series").html("series: " + Rout[1] + "&#8486;");
    });

    $('#slideToggle').click(function(){
        $('#slide').slideToggle();
    });
});