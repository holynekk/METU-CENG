$(document).ready(function () {
    function setCookie(name,value,days) {
        var expires = "";
        if (days) {
            var date = new Date();
            date.setTime(date.getTime() + (days*24*60*60*1000));
            expires = "; expires=" + date.toUTCString();
        }
        document.cookie = name + "=" + (value || "")  + expires + "; path=/login";
    }

    function getCookie(c_name) {
        if(document.cookie.length > 0) {
            c_start = document.cookie.indexOf(c_name + "=");
            if(c_start != -1) {
                c_start = c_start + c_name.length + 1;
                c_end = document.cookie.indexOf(";", c_start);
                if(c_end == -1) c_end = document.cookie.length;
                return unescape(document.cookie.substring(c_start,c_end));
            }
        }
        return "";
    }

    $("#ready-form").submit(function (e) {
        $.ajax({
          type: "POST",
          url: "/ready/",
          data: {
            'board_id': $('#board_id').val(),
          },
          headers: {
              "X-CSRFToken": getCookie("csrftoken"),
          },
          success: function (e) {
              e = JSON.parse(e);
              if (e.status === 200) {
                    $("div.serverMessage").html("Message: " + e.message);
                    window.location.replace("/board/" + $('#board_id').val());
              } else {
                    $("div.serverMessage").html("Message: " + e.message);
              }
          }
        });
        return false;
      });

});
