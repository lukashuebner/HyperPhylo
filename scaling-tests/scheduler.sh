#/bin/bash

tmux start-server
tmux new-session -d -s scalingtests -n i10pc127
tmux new-window -t scalingtests:1 -n i10pc128
tmux new-window -t scalingtests:2 -n 10pc129
tmux new-window -t scalingtests:3 -n 10pc132
tmux new-window -t scalingtests:4 -n 10pc134

tmux send-keys -t scalingtests:0 "ssh i10pc127 'bash -l -s' < runSomeStuff.sh" C-m
tmux send-keys -t scalingtests:1 "ssh i10pc128 'bash -l -s' < runSomeStuff.sh" C-m
tmux send-keys -t scalingtests:2 "ssh i10pc129 'bash -l -s' < runSomeStuff.sh" C-m
tmux send-keys -t scalingtests:3 "ssh i10pc132 'bash -l -s' < runSomeStuff.sh" C-m
tmux send-keys -t scalingtests:4 "ssh i10pc134 'bash -l -s' < runSomeStuff.sh" C-m

tmux select-window -t scalingtests:1
tmux attach-session -t scalingtests
