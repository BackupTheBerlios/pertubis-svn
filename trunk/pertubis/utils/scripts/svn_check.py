import sys,re

text= sys.stdin.read()
m=re.compile("Revision: \d+").search(text)

if m== None:
    print ""
else:
    line=text[m.start():m.end()]
    n=re.compile("\d+").search(line)
    print line[n.start():n.end()]
