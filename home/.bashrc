PS1='\[\e[1;35m\]\w\[\e[m\] ($(echo $(git branch --show-current 2> /dev/null && git rev-parse --short HEAD))) '
alias big='ls -A | xargs du -sch | sort -h'
alias reload='ps -u$USER -opid= | xargs kill -9'
