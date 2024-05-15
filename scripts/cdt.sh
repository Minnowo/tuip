#!/bin/sh

# Pick a directory and cd into it
# useful if you cannot type the name of the directory
function cdt() {

    files=()
    files+=(..) # always include parent dir first

    # get all directories
    for i in */; do

        files+=("$i")

    done

    # run the tui to pick one
    if tuip "${files[@]}"; then

        # get it's output choice
        path=$(cat /tmp/tuip)

        echo $path

        if [ -d "$path" ]; then 

            cd "$path"
        else

            echo "Path $path does not exist!"
        fi

    fi
}

