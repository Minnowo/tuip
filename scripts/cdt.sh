#!/bin/sh

# source this file in your .bashrc, or copy the function below

# Pick a directory and cd into it
# useful if you cannot type the name of the directory
function cdt() {

    # makes */ not appear in the arrays
    shopt -s nullglob

    files=()
    files+=(..)  # always include parent dir first
    files+=(.*/) # hidden dirs
    files+=(*/)  # other dirs

    shopt -u nullglob

    # run the tui to pick one
    if tuip "${files[@]}"; then

        # get it's output choice
        path=$(cat /tmp/tuip)

        rm -f /tmp/tuip

        if [ -d "$path" ]; then 

            cd "$path"
        else

            echo "Path $path does not exist!"
        fi

    fi
}

