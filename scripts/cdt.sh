#!/bin/sh

# Pick a directory and cd into it
# useful if you cannot type the name of the directory
function cdt() {

    files=()
    files+=(..) # always include parent dir first

    # makes */ not appear in the arrays
    shopt -s nullglob

    # hidden directories
    for i in .*/; do

        files+=("$i")
    done

    # other directories
    for i in */; do

        files+=("$i")
    done

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

