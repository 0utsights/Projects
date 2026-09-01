from random import choice

def select_word():
    with open("words.txt", mode="r") as words:
        word_list = words.readlines()
    return choice(word_list).strip()

def get_player_input():
    while true:
        player_input = input("Guess a letter!").lower()
        if validate_input(player_input, guessed_letters):
            return player_input
        else:
            print("Invalid input, make sure it's a new letter.")

import string

def _validate_input(player_input, guessed_letters):
    return (
        len(player_input) == 1
        and player_input in string.ascii_lowercase
        and player_input not in guessed_letters
    )

    

