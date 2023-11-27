$(document).ready(function () {
    function setCookie(name,value,days) {
        var expires = "";
        if (days) {
            var date = new Date();
            date.setTime(date.getTime() + (days*24*60*60*1000));
            expires = "; expires=" + date.toUTCString();
        }
        document.cookie = name + "=" + (value || "")  + expires + "; path=/";
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

    $("#sign-up-form").submit(function (event) {
      event.preventDefault();
      $.ajax({
        type: "POST",
        url: "/signup/",
        data: {
          'username': $('#username').val(),
          'password': $('#password').val(),
          'email': $('#email').val(),
          'fullname': $('#fullname').val(),
        },
        headers: {
            "X-CSRFToken": getCookie("csrftoken"),
        },
        success: function (e) {
            e = JSON.parse(e);
            if (e.status === 200) {
                setCookie("is_authenticated", "yes", 7);
                setCookie("username", $('#username').val(), 7);
                $("div.serverMessage").html("Message: " + e.message);
                // addMess("User has signed ip with username: " + $('#username').val());
                window.location.replace("/");
            } else {
                $("#username").val("");
                $("#password").val("");
                $('#email').val("");
                $('#fullname').val("");
                $("div.serverMessage").html("Message: " + e.message);
            }
        }
      });
      return false;
    });
  });
