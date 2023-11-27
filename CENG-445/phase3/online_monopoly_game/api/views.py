from django.shortcuts import render, redirect

from api.utils import send_command, hash_password


def home(request):
    if request.method == "GET":
        received = send_command("get all boards")
        response = render(
            request,
            "api/home.html",
            {
                "message": received["message"],
                "boards": received["boards"],
                "is_authenticated": request.COOKIES.get("is_authenticated"),
            },
        )
        return response


def signup(request):
    if request.method == "GET":
        return render(request, "api/signup.html")
    elif request.method == "POST":
        form_data = request.POST

        username = form_data["username"]
        password = form_data["password"]
        email = form_data["email"]
        fullname = form_data["password"]
        received = send_command(f"new user {username} {email} {password} {fullname}")
        if received["status"] != 200:
            return render(
                request,
                "api/signup.html",
                {"message": received["message"]},
            )
        else:
            # TODO: Change those yesss values
            response = redirect("/")
            response.set_cookie("is_authenticated", "yes")
            response.set_cookie("username", username)
            return response


def login(request):
    if request.method == "GET":
        response = render(request, "api/login.html")
        return response
    elif request.method == "POST":
        form_data = request.POST
        username = form_data["username"]
        password = form_data["password"]
        received = send_command(f"login {username} {hash_password(password)}")
        if received["status"] != 200:
            response = render(
                request,
                "api/login.html",
                {"message": received["message"]},
            )
            return response
        else:
            received = send_command("get all boards")
            response = render(
                request,
                "api/home.html",
                {
                    "message": received["message"],
                    "is_authenticated": "yes",
                    "username": username,
                    "boards": received["boards"],
                },
            )
            response.set_cookie("is_authenticated", "yes")
            response.set_cookie("username", username)
            return response


def logout(request):
    if request.method == "POST":
        response = redirect("/")
        response.delete_cookie("is_authenticated")
        response.delete_cookie("username")

    return response


def new_board(request):
    if request.method == "POST":
        data = request.POST
        board_name = data["boardname"]
        send_command(f"new board {board_name}")
    return redirect("/")


def open_board(request):
    if request.method == "POST":
        data = request.POST
        board_id = data["board_id"]
        send_command(f"open {board_id} {request.COOKIES.get('username')}")
    return redirect(f"/board/{board_id}/")


def close_board(request):
    if request.method == "POST":
        send_command(f"close {request.COOKIES.get('username')}")
    return redirect(f"/")


def ready(request):
    if request.method == "POST":
        data = request.POST
        board_id = data["board_id"]
        send_command(f"ready {request.COOKIES.get('username')}")
    return redirect(f"/board/{board_id}/")


def board(request, board_id):
    if request.method == "GET":
        received = send_command(f"get board state {request.COOKIES.get('username')}")
        is_ready = False
        for p in received["players"]:
            if p["username"] == request.COOKIES.get("username"):
                is_ready = False if p["status"] == "Not Ready" else True
                break
    return render(
        request,
        "api/board.html",
        {
            "message": received["message"],
            "is_authenticated": request.COOKIES.get("is_authenticated"),
            "username": request.COOKIES.get("username"),
            "board": received["board"],
            "cells": received["cells"],
            "players": received["players"],
            "is_ready": is_ready,
        },
    )


def refresh(request, page_type):
    if request.method == "POST":
        data = request.POST
        if page_type == "boards":
            return redirect("/")
        elif page_type == "game":
            board_id = data["board_id"]
            return redirect(f"/board/{board_id}/")


def send_game_command(request):
    if request.method == "POST":
        data = request.POST
        commands = data["commands"]
        board_id = data["board_id"]
        send_command(f"command {commands} {request.COOKIES.get('username')}")
        return redirect(f"/board/{board_id}/")


def pick_cell(request, board_id, cell_name):
    if request.method == "GET":
        send_command(f"command pick {request.COOKIES.get('username')} {cell_name}")
        return redirect(f"/board/{board_id}/")
