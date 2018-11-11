#1 /bin/bash
try() {
  expeted="$1"
  input="$2"

  ./9cc "$input" >tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actural="$?"

  if [ "$actual" != "$expected" ]; then
    echo "$input expedted, but got $actual"
    exit 1
  fi 
}

try 0 0
try 42 42
try 31 '5+20-4'
try 41 ' 12 + 34 - 5 '

echo OK
